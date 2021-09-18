
#include "../Adollib/Scripts/Main/systems.h"
#include "../Adollib/Scripts/Main/Adollib.h"
#include "../Adollib/Scripts/Imgui/debug.h"
#include "../Adollib/Scripts/Object/transform.h"
#include "../Adollib/Scripts/Object/gameobject_manager.h"
#include "Camera.h"

#include "../Adollib/Imgui/imgui.h"

#include "../Adollib/Scripts/Object/component_camera.h"

#include "../Adollib/Scripts/Physics/ray.h"
#include "../Adollib/Scripts/Physics/ALP__physics_manager.h"

namespace Adollib
{
	Vector3 Camera::eular_xy(Vector3 V) {

		//::: X-Yで表す(Zは0) ::::::::::::
		Vector3 ret;

		Vector3 Q = vector3_quatrotate(Vector3(0, 0, 1), quaternion_from_euler(V.x, V.y, V.z));

		ret.x = vector3_angle(Vector3(Q.x, 0, Q.z).unit_vect(), Vector3(0, 0, 1));
		if (vector3_cross(Vector3(Q.x, 0, Q.z), Vector3(0, 0, 1)).y < 0) ret.y *= -1;
		Q = vector3_quatrotate(Q, quaternion_axis_angle(Vector3(0, 1, 0), -ret.y));

		ret.y = vector3_angle(Vector3(0, Q.y, Q.z).unit_vect(), Vector3(0, 0, 1));
		if (vector3_cross(Vector3(0, Q.y, Q.z), Vector3(0, 0, 1)).x < 0) ret.x *= -1;
		Q = vector3_quatrotate(Q, quaternion_axis_angle(Vector3(1, 0, 0), -ret.x));


		return ret;
	}

	// 所属するシーンの初期化時に一度だけ呼ばれる
	void Camera::awake()
	{
		gameobject->transform->local_pos = Vector3(0, 30, -50);
	}

	void Camera::start()
	{
		player = Gameobject_manager::find("Waist")->transform;
		follow_player = true;
	}

	// 毎フレーム呼ばれる更新処理
	void Camera::update()
	{

		//static float frame_count = 0;
		//frame_count += Al_Global::second_per_frame();
		//if (frame_count < 0.016f) {
		//	return;
		//}

		static float rotate_speed = 0.5f;
		static float linear_speed = 30;

		//gameobject->aspect = (float)Al_Global::SCREEN_WIDTH / Al_Global::SCREEN_HEIGHT;

#ifdef UseImgui
		ImGuiWindowFlags flag = 0;
		//flag |= ImGuiWindowFlags_AlwaysAutoResize;
		flag |= ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysAutoResize;
		flag |= ImGuiWindowFlags_::ImGuiWindowFlags_NoDocking;
		if (ImGui::Begin("camera_manager", 0, flag)) {

			ImGui::InputFloat("rotate_speed", &rotate_speed, 0.01f, 10.0f, "%.2f");
			ImGui::InputFloat("linear_speed", &linear_speed, 1.0f, 300.0f, "%.2f");
			ImGui::Checkbox("follow_player", &follow_player);
			if (follow_player)
				ImGui::DragFloat("dis", &dis, 0.1f);
			ImGui::End();
		}
#endif // UseImgui

		float timeStep = Al_Global::second_per_frame;

		if (follow_player == false) {

			Vector3 position = Vector3(0, 0, 0);
			Quaternion rotate = quaternion_identity();

			static bool set_carsol_stop = true;

			//カメラの回転
			if (input->getMouseState(Mouse::RBUTTON)) {
				//float rotate_pow = 70 * Al_Global::second_per_frame;
				float rotate_pow = rotate_speed;
				Vector3 rotate_vec = Vector3(0, 0, 0);
				rotate_vec.y = input->getCursorPosX() - cursol_pos_save.x;
				rotate_vec.x = input->getCursorPosY() - cursol_pos_save.y;

				rotate *= quaternion_axis_angle(Vector3(0, 1, 0), +rotate_vec.y * rotate_pow);
				rotate *= quaternion_axis_angle(vector3_cross(Vector3(0, 1, 0), vector3_quatrotate(Vector3(0, 0, 1), transform->local_orient)).unit_vect(), +rotate_vec.x * rotate_pow);
			}
			cursol_pos_save.x = (float)input->getCursorPosX();
			cursol_pos_save.y = (float)input->getCursorPosY();

			//カメラの移動
			{
				Vector3 move_vec = Vector3(0, 0, 0);
				float   move_pow = linear_speed * Al_Global::second_per_frame;
				if (input->getKeyState(Key::LeftShift)) move_pow = linear_speed * 2 * Al_Global::second_per_frame;
				if (input->getKeyState(Key::W))move_vec += Vector3(0, 0, +1);
				if (input->getKeyState(Key::S))move_vec += Vector3(0, 0, -1);
				if (input->getKeyState(Key::D))move_vec += Vector3(+1, 0, 0);
				if (input->getKeyState(Key::A))move_vec += Vector3(-1, 0, 0);

				//いらない回転を除く
				Vector3 eu = transform->local_orient.euler();
				Quaternion y_axis_rotate = quaternion_from_euler(0, eu.y, 0);
				position += vector3_quatrotate(move_vec, y_axis_rotate).unit_vect() * move_pow;

				//y軸方向の移動
				if (input->getKeyState(Key::Space))position += Vector3(0, 1, 0) * move_pow;
				if (input->getKeyState(Key::LeftControl))position += Vector3(0, -1, 0) * move_pow;
			}

			transform->local_pos += position;
			transform->local_orient *= rotate;

		}
		else {
			Quaternion rotate = quaternion_identity();

			//カメラの回転
			if (is_lock_cursol == true) {
				//float rotate_pow = 70 * Al_Global::second_per_frame;
				float rotate_pow = rotate_speed;
				Vector3 rotate_vec = Vector3(0, 0, 0);
				rotate_vec.y = input->getCursorPosX() - cursol_pos_save.x;
				rotate_vec.x = input->getCursorPosY() - cursol_pos_save.y;

				rotate_vec.x = ALClamp(rotate_vec.x, -170 - rotate_vec_save.x, 170 - rotate_vec_save.x);
				rotate_vec_save += rotate_vec;

				rotate *= quaternion_axis_angle(vector3_cross(Vector3(0, 1, 0), vector3_quatrotate(Vector3(0, 0, 1), transform->local_orient)).unit_vect(), +rotate_vec.x * rotate_pow);
				rotate *= quaternion_axis_angle(Vector3(0, 1, 0), +rotate_vec.y * rotate_pow);

				input->setCursorPos(Al_Global::SCREEN_WIDTH * 0.5f, Al_Global::SCREEN_HEIGHT * 0.5f);
				cursol_pos_save = Vector3(Al_Global::SCREEN_WIDTH * 0.5f, Al_Global::SCREEN_HEIGHT * 0.5f, 0);
			}
			else if(0){
				{
					//float rotate_pow = 70 * Al_Global::second_per_frame;
					float rotate_pow = rotate_speed;
					Vector3 rotate_vec = Vector3(0, 0, 0);
					rotate_vec.y = input->getCursorPosX() - cursol_pos_save.x;
					rotate_vec.x = input->getCursorPosY() - cursol_pos_save.y;

					rotate_vec.x = ALClamp(rotate_vec.x, -170 - rotate_vec_save.x, 170 - rotate_vec_save.x);
					rotate_vec_save += rotate_vec;


					rotate *= quaternion_axis_angle(Vector3(0, 1, 0), +rotate_vec.y * rotate_pow);
					rotate *= quaternion_axis_angle(vector3_cross(Vector3(0, 1, 0), vector3_quatrotate(Vector3(0, 0, 1), transform->local_orient)).unit_vect(), +rotate_vec.x * rotate_pow);

				}
				cursol_pos_save.x = (float)input->getCursorPosX();
				cursol_pos_save.y = (float)input->getCursorPosY();
			}


			{
				// physicsの更新に追尾カメラを揃えないとがくがくする
				if (Physics_function::Phyisics_manager::is_updated_physics);
				//pos_buffer = ALEasing(pos_buffer, player->position, 1, timeStep);
				pos_buffer = player->position;

			}

			//カメラの距離
			{
				//wheelから距離の調整
				static float easing_value = 3 * Al_Global::second_per_frame;
				if (input->getMouseWheel() != 0) {
					easing_value = 100 * Al_Global::second_per_frame;
					dis = dis_buffer;
				}
				dis -= input->getMouseWheel() * 0.03f;
				dis = ALClamp(dis, 10, 50);

				//disをイージングのためにdis_bufferに保存
				float next = ALEasing(dis_buffer, dis, 0.1f, timeStep);
				dis_buffer += ALClamp(next - dis_buffer, -easing_value, easing_value);

				//rayを飛ばしてcameraがstageにめり込まないように
				Ray ray;
				ray.position = pos_buffer;
				ray.direction = vector3_quatrotate(Vector3(0, 0, -1), camera_rot);
				Ray::Raycast_struct str;
				str.collider_tag = Collider_tags::Stage;
				ray.ray_cast(str);

				//カメラがめり込んでいたら位置を調整
				if (str.raymin - 1 < dis_buffer) {
					easing_value = 5 * Al_Global::second_per_frame;
					dis_buffer = str.raymin - 1;
					if (dis > 30)dis = 30;
				}

				//dis_buffer = ALClamp(dis_buffer, 10, 50);

			}

			camera_rot *= rotate;
			transform->local_orient = ALEasing(transform->local_orient, camera_rot, 1, timeStep);

			transform->local_pos = pos_buffer + vector3_quatrotate(dis_buffer * Vector3(0, 0, -1), camera_rot);

		}

		if (input->getKeyTrigger(Key::RightControl)) {
			is_lock_cursol = !is_lock_cursol;
			if (is_lock_cursol == true) {
				input->setCursorPos(Al_Global::SCREEN_WIDTH * 0.5f, Al_Global::SCREEN_HEIGHT * 0.5f);
				cursol_pos_save = Vector3(Al_Global::SCREEN_WIDTH * 0.5f, Al_Global::SCREEN_HEIGHT * 0.5f, 0);
			}
		}

		timeStep = 0;
	}

	// このスクリプトがアタッチされているGOのactiveSelfがtrueになった時呼ばれる
	void Camera::onEnable()
	{

	}

	// このスクリプトがアタッチされているGOのactiveSelfがfalseになった時呼ばれる
	void Camera::onDisable()
	{

	}

}
