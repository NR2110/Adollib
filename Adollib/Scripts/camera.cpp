
#include "../Adollib/Scripts/Main/systems.h"
#include "../Adollib/Scripts/Main/Adollib.h"
#include "../Adollib/Scripts/Imgui/debug.h"
#include "../Adollib/Scripts/Object/transform.h"
#include "../Adollib/Scripts/Object/gameobject_manager.h"
#include "../Adollib/Scripts/Object/component_camera.h"
#include "../Adollib/Scripts/Renderer/directional_shadow.h"

#include "Camera.h"

#include "../Adollib/Imgui/imgui.h"

#include "../Adollib/Scripts/Object/component_camera.h"

#include "../Adollib/Scripts/Physics/ray.h"
#include "../Adollib/Scripts/Physics/ALP__physics_manager.h"


namespace Adollib
{

	// 所属するシーンの初期化時に一度だけ呼ばれる
	void Camera::awake()
	{
		gameobject->transform->local_pos = Vector3(0, 30, -50);
		//input->setCursorPos(Al_Global::SCREEN_WIDTH * 0.5f, Al_Global::SCREEN_HEIGHT * 0.5f);
	}

	void Camera::start()
	{
		player = Gameobject_manager::find("Body")->transform;
		player = Gameobject_manager::find("Waist")->transform;
		follow_player = true;

		auto c = gameobject->findComponent<Camera_component>();
		c->directional_shadow->position = Vector3(200, 200, -200);
		c->directional_shadow->direction = Vector3(-1, -2, 1).unit_vect();
		c->directional_shadow->nearZ = 150;
		c->directional_shadow->nearZ = 0.1f;
	}

	// 毎フレーム呼ばれる更新処理
	void Camera::update()
	{

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
			if (follow_player) ImGui::DragFloat("dis", &dis, 0.1f);

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

				rotate_vec.x = ALClamp(rotate_vec.x, -100 - rotate_vec_save.x, 120 - rotate_vec_save.x);
				rotate_vec_save += rotate_vec;

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

				rotate_vec.x = ALClamp(rotate_vec.x, -100 - rotate_vec_save.x, 120 - rotate_vec_save.x);
				rotate_vec_save += rotate_vec;

				rotate *= quaternion_axis_angle(vector3_cross(Vector3(0, 1, 0), vector3_quatrotate(Vector3(0, 0, 1), transform->local_orient)).unit_vect(), +rotate_vec.x * rotate_pow);
				rotate *= quaternion_axis_angle(Vector3(0, 1, 0), +rotate_vec.y * rotate_pow);

				input->setCursorPos(Al_Global::SCREEN_WIDTH * 0.5f, Al_Global::SCREEN_HEIGHT * 0.5f);
				cursol_pos_save = Vector3(Al_Global::SCREEN_WIDTH * 0.5f, Al_Global::SCREEN_HEIGHT * 0.5f, 0);
			}

			{
				camera_rot *= rotate;
				transform->local_orient = ALEasing(transform->local_orient, camera_rot, 1, timeStep);
			}

			{
				//pos_buffer = ALEasing(pos_buffer, player->position, 1, timeStep);
				//Vector3 goal = player->position + vector3_quatrotate(Vector3(3 * ALClamp(0,1, dis_buffer / 50), 4, 0), transform->local_orient);
				Vector3 goal = player->position + Vector3(0, 4, 0);
				Vector3 dir = goal - pos_buffer;
				if (dir.norm() > pos_slop * pos_slop) {
					pos_buffer += dir.unit_vect() * (dir.norm_sqr() - pos_slop);
				}

				//pos_buffer = player->position + Vector3(0, 4, 0);

			}

			//カメラの距離
			{
				//wheelから距離の調整
				float easing_pow = 0.1f;
				float easing_value = 20 * timeStep; //イージングでの移動距離
				if (ray_timer > 0)easing_value = 0;
				ray_timer -= timeStep;

				// ホイールが動いていれば 現在の距離からwheelで動かしたところをdisにおく
				if (input->getMouseWheel() != 0) {
					easing_value = 10000 * timeStep; // 自然に戻るより早くする
					easing_pow = 0.8f;
					dis = dis_buffer;
					dis += -input->getMouseWheel() * 0.02f;

					if (dis_buffer < 10 && dis < dis_buffer)dis = dis_buffer;
					if (dis_buffer > 20 && dis > dis_buffer)dis = dis_buffer;

					if (dis_buffer == ALClamp(dis_buffer, 10, 50)) dis = ALClamp(dis, 10, 50);
				}
				else dis = ALClamp(dis, 10, 20); //マウスホイールで調整可能な最低距離 / 最大距離

				//disをイージングのためにdis_bufferに保存
				const float next = ALEasing(dis_buffer, dis, easing_pow, timeStep);
				dis_buffer += ALClamp(next - dis_buffer, -easing_value, easing_value);

				//rayを飛ばしてcameraがstageにめり込まないように
				Ray ray;
				ray.position = pos_buffer;
				ray.direction = vector3_quatrotate(Vector3(0, 0, -1), camera_rot);
				Ray::Raycast_struct str;
				str.collider_tag = Collider_tags::Caera_not_sunk_Stage;

				const float sphere_radius = 1.0f;

				ray.sphere_cast(sphere_radius, str);

				// カメラの後ろに壁があればtimerをset timerが>0の時自然に元の距離まで戻らない
				if (ray_timer > 0 && str.raymin - 1 < dis_buffer) ray_timer = 0.1f;
				//カメラがめり込んでいたら位置を調整
				if (str.raymin < dis_buffer) {
					easing_value = 5 * timeStep;
					dis_buffer = str.raymin;
					ray_timer = 0.1f;
					if (dis > 30)dis = 30; //最大距離
				}

			}

			transform->local_pos = pos_buffer + vector3_quatrotate(dis_buffer * Vector3(0, 0, -1), camera_rot);

		}

		if (input->getKeyTrigger(Key::RightControl)) {
			is_lock_cursol = !is_lock_cursol;
			if (is_lock_cursol) ShowCursor(FALSE);
			else ShowCursor(TRUE);

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
