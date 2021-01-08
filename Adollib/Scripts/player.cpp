
#include "../Adollib/Scripts/Main/Adollib.h"
#include "../Adollib/Scripts/Object/gameobject_manager.h"

#include "player.h"

namespace Adollib
{
	// 所属するシーンの初期化時に一度だけ呼ばれる
	void Player::awake() {

	}

	void Player::start()
	{
	}

	// 毎フレーム呼ばれる更新処理
	void Player::update()
	{
		Vector3 move_vec = Vector3(0, 0, 0);
		if (input->getKeyState(Key::W))move_vec += Vector3(0, 0, +1);
		if (input->getKeyState(Key::S))move_vec += Vector3(0, 0, -1);
		if (input->getKeyState(Key::D))move_vec += Vector3(+1, 0, 0);
		if (input->getKeyState(Key::A))move_vec += Vector3(-1, 0, 0);
		Vector3 rot = camera->transform->orientation.euler();
		rot.x = rot.z = 0;
		move_vec = vector3_quatrotate(move_vec, quaternion_from_euler(rot));
		collider->add_force(move_vec * move_speed);

		if (input->getKeyTrigger(Key::Space))collider->add_force(Vector3(0, 1, 0) * jump_power);

		is_running = input->getKeyTrigger(Key::LeftShift);

		{
			float k = linear_drag_xz / collider->physics_data.inertial_mass; //空気抵抗やらなんやらを考慮した値 のはずだけど適当に簡略化

			Vector3 linearvec_xz_save = collider->linear_velocity();
			float linearvec_y_save = linearvec_xz_save.y;
			linearvec_xz_save.y = 0;

			Vector3 Vel = linearvec_xz_save * exp(-k * time->deltaTime()); // 水平方向のみ空気抵抗を考慮した速度

			// 速度を最大値で
			if (is_running) { if (Vel.norm() > running_speed * running_speed) { Vel = Vel.unit_vect() * running_speed; } }
			else { if (Vel.norm() > walking_speed * walking_speed) { Vel = Vel.unit_vect() * walking_speed; } }

			if (linearvec_y_save > max_move_y_speed) { linearvec_y_save = max_move_y_speed; }

			Vel.y = linearvec_y_save;

			collider->linear_velocity(Vel);
		}

		if(ImGui::Begin("Player_debug")) {
			ImGui::DragFloat("walking_speed", &walking_speed, 0.1f);
			ImGui::DragFloat("running_speed", &running_speed, 0.1f);
			ImGui::DragFloat("jump_pow", &jump_power, 0.1f);
		}
		ImGui::End();
	}

	// このスクリプトがアタッチされているGOのactiveSelfがtrueになった時呼ばれる
	void Player::onEnable()
	{

	}

	// このスクリプトがアタッチされているGOのactiveSelfがfalseになった時呼ばれる
	void Player::onDisable()
	{

	}

}
