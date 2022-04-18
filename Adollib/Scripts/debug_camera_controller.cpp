#include "../Adollib/Scripts/Main/systems.h"
#include "../Adollib/Scripts/Main/Adollib.h"
#include "../Adollib/Scripts/Imgui/debug.h"
#include "../Adollib/Scripts/Object/transform.h"
#include "../Adollib/Scripts/Object/gameobject_manager.h"
#include "../Adollib/Scripts/Object/component_camera.h"
#include "../Adollib/Scripts/Renderer/directional_shadow.h"


#include "debug_camera_controller.h"

#include "../Adollib/Imgui/imgui.h"

#include "../Adollib/Scripts/Object/component_camera.h"

#include "../Adollib/Scripts/Physics/ray.h"
#include "../Adollib/Scripts/Physics/ALP__physics_manager.h"

#include "input_changer.h"

namespace Adollib
{

	// 所属するシーンの初期化時に一度だけ呼ばれる
	void Debug_camera_controller::awake()
	{
#ifndef ON_DEBUG
		return;
#endif
		gameobject->transform->local_pos = Vector3(0, 30, -50);
		//input->setCursorPos(Al_Global::SCREEN_WIDTH * 0.5f, Al_Global::SCREEN_HEIGHT * 0.5f);

	}

	void Debug_camera_controller::start()
	{
#ifndef ON_DEBUG
		return;
#endif

//#ifndef ON_DEBUG
//		input_changer->set_lockcursol(true);
//#else
		ShowCursor(TRUE);
//#endif
		auto c = gameobject->findComponent<Camera_component>();
		c->directional_shadow->position = Vector3(300, 320, -200);
		c->directional_shadow->direction = Vector3(-1, -2, 1).unit_vect();
		c->directional_shadow->nearZ = 150;
		c->directional_shadow->nearZ = 0.1f;

	}

	// 毎フレーム呼ばれる更新処理
	void Debug_camera_controller::update()
	{
#ifndef ON_DEBUG
		return;
#endif

		//gameobject->aspect = (float)Al_Global::SCREEN_WIDTH / Al_Global::SCREEN_HEIGHT;

#ifdef UseImgui
		ImGuiWindowFlags flag = 0;
		//flag |= ImGuiWindowFlags_AlwaysAutoResize;
		flag |= ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysAutoResize;
		flag |= ImGuiWindowFlags_::ImGuiWindowFlags_NoDocking;
		if (ImGui::Begin("camera_manager", 0, flag)) {

			ImGui::InputFloat("rotate_speed", &rotate_speed, 0.01f, 10.0f, "%.2f");
			ImGui::InputFloat("linear_speed", &linear_speed, 1.0f, 300.0f, "%.2f");
		}
		ImGui::End();

#endif // UseImgui

		float timeStep = Al_Global::second_per_frame;

		 {

			Vector3 position = Vector3(0, 0, 0);
			Quaternion rotate = quaternion_identity();

			static bool set_carsol_stop = true;

			//カメラの回転
			if (input->getMouseTrigger(Mouse::RBUTTON)) {
				cursol_pos_save.x = (float)input->getCursorPosX();
				cursol_pos_save.y = (float)input->getCursorPosY();
			}
			if (input->getMouseState(Mouse::RBUTTON)) {
				//float rotate_pow = 70 * Al_Global::second_per_frame;
				float rotate_pow = rotate_speed;
				Vector3 rotate_vec = Vector3(0, 0, 0);

				rotate_vec.y = (input->getCursorPosX() - cursol_pos_save.x) * rotate_speed;
				rotate_vec.x = (input->getCursorPosY() - cursol_pos_save.y) * rotate_speed;
				cursol_pos_save.x = (float)input->getCursorPosX();
				cursol_pos_save.y = (float)input->getCursorPosY();

				rotate_vec.x = ALClamp(rotate_vec.x, max_rotate.x - rotate_vec_save.x, max_rotate.y - rotate_vec_save.x);
				rotate_vec_save += rotate_vec;

				rotate *= quaternion_axis_angle(Vector3(0, 1, 0), +rotate_vec.y);
				rotate *= quaternion_axis_angle(vector3_cross(Vector3(0, 1, 0), vector3_quatrotate(Vector3(0, 0, 1), camera_rot)).unit_vect(), +rotate_vec.x);

				transform->local_orient = camera_rot;
			}

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
				Vector3 eu = camera_rot.euler();
				Quaternion y_axis_rotate = quaternion_from_euler(0, eu.y, 0);
				position += vector3_quatrotate(move_vec, y_axis_rotate).unit_vect() * move_pow;

				//y軸方向の移動
				if (input->getKeyState(Key::Space))position += Vector3(0, 1, 0) * move_pow;
				if (input->getKeyState(Key::LeftControl))position += Vector3(0, -1, 0) * move_pow;
			}

			transform->local_pos += position;
			camera_rot *= rotate;

		}


		timeStep = 0;
	}

	// このスクリプトがアタッチされているGOのactiveSelfがtrueになった時呼ばれる
	void Debug_camera_controller::onEnable()
	{

	}

	// このスクリプトがアタッチされているGOのactiveSelfがfalseになった時呼ばれる
	void Debug_camera_controller::onDisable()
	{

	}

}
