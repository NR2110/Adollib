#include "player.h"

#include "../Adollib/Scripts/Main/Adollib.h"
#include "../Adollib/Scripts/Object/gameobject_manager.h"

#include "../Adollib/Scripts/Physics/ray.h"

namespace Adollib
{
	// 所属するシーンの初期化時に一度だけ呼ばれる
	void Player::awake() {
		rotate = quaternion_from_euler(0, 180, 0);
		pos = Vector3(-2.7f, 23, -52.2f);
	}

	void Player::start()
	{
		camera = Gameobject_manager::find("camera")->transform;
	}

	// 毎フレーム呼ばれる更新処理
	void Player::update()
	{
		Head_collider->physics_data.drag = 0;
		Rsholder_collider->physics_data.drag = 0;
		Relbow_collider->physics_data.drag = 0;
		Lsholder_collider->physics_data.drag = 0;
		Lelbow_collider->physics_data.drag = 0;
		Body_collider->physics_data.drag = 0;
		Waist_collider->physics_data.drag = 0;
		Rleg_collider->physics_data.drag = 0;
		Rfoot_collider->physics_data.drag = 0;
		Lleg_collider->physics_data.drag = 0;
		Lfoot_collider->physics_data.drag = 0;

		//if (Waist_capsule_collider->concoll_enter(Collider_tags::Stage) /*||
		//	Rfoot_collider->concoll_enter(Collider_tags::Stage) ||
		//	Lfoot_collider->concoll_enter(Collider_tags::Stage)*/
		//	) {
		//	gnyat_pow += Al_Global::second_per_frame() * 2;
		//}
		//else {
		//	gnyat_pow -= Al_Global::second_per_frame() * 0.5f;
		//}
		gnyat_pow = ALClamp(gnyat_pow, 0, 1);

		gnyat_pow = 0.9f;
		{
			{
				//顔が赤ちゃんなのを治す
				Head_collider->physics_data.anglar_drag = 1;
				Quaternion off = Body_collider->gameobject->transform->orientation * Head_collider->gameobject->transform->orientation.inverse();
				Head_collider->add_torque(off.axis() * off.radian() * 200000 * gnyat_pow * Al_Global::second_per_frame());
			}
			{
				//腰をたたせる
				Quaternion off_rot = rotate * Waist_collider->gameobject->transform->orientation.inverse();
				Waist_collider->add_torque(off_rot.axis() * off_rot.radian() * 600000 * gnyat_pow* Al_Global::second_per_frame());

				pos.y = Waist_collider->gameobject->transform->position.y;
				Vector3 off_pos = pos - Waist_collider->gameobject->transform->position;
				Waist_collider->add_force(off_pos * move_speed * gnyat_pow * Al_Global::second_per_frame());

			}
			{
				//胴体をたたせる
				Quaternion off = Waist_collider->gameobject->transform->orientation * Body_collider->gameobject->transform->orientation.inverse();
				Body_collider->add_torque(off.axis() * off.radian() * 100000 * gnyat_pow * Al_Global::second_per_frame());
			}
		}

		//回転
		{
			Vector3 rot_vec = Vector3(0);
			if (input->getKeyState(Key::W)) {
				rot_vec += Vector3(0, 0, -1);
			}
			if (input->getKeyState(Key::S)) {
				rot_vec += Vector3(0, 0, +1);
			}
			if (input->getKeyState(Key::A)) {
				rot_vec += Vector3(+1, 0, 0);
			}
			if (input->getKeyState(Key::D)) {
				rot_vec += Vector3(-1, 0, 0);
			}

			if (rot_vec.norm() != 0) {
				rot_vec = rot_vec.unit_vect();

				rot_vec = vector3_quatrotate(rot_vec, camera->orientation);
				rot_vec.y = 0;
				rot_vec = rot_vec.unit_vect();

				Vector3 player_vec = vector3_quatrotate(Vector3(0, 0, 1), rotate);

				float angle = vector3_angle(rot_vec, player_vec);
				if (angle > turn_speed)angle = turn_speed;

				if (vector3_cross(player_vec, rot_vec).y < 0)angle *= -1;

				rotate *= quaternion_axis_angle(Vector3(0, 1, 0), angle);

				vector3_angle(rot_vec, player_vec);

			}
		}

		//移動
		{
			Vector3 move_vec = Vector3(0);
			if (input->getKeyState(Key::W)) {
				move_vec += Vector3(0, 0, +1);
			}
			if (input->getKeyState(Key::S)) {
				move_vec += Vector3(0, 0, -1);
			}
			if (input->getKeyState(Key::A)) {
				move_vec += Vector3(-1, 0, 0);
			}
			if (input->getKeyState(Key::D)) {
				move_vec += Vector3(+1, 0, 0);
			}
			if (move_vec.norm() != 0) {
				move_vec = move_vec.unit_vect();
				move_vec = vector3_quatrotate(move_vec, camera->orientation);
				move_vec.y = 0;
				move_vec = move_vec.unit_vect();

				pos += move_vec * move_speed * Al_Global::second_per_frame();
			}

			if (move_vec.norm() == 0)pos = Waist_collider->gameobject->transform->position;

			float pow = 0.2f;
			if ((pos - Waist_collider->gameobject->transform->position).norm() > pow * pow)
				pos = Waist_collider->gameobject->transform->position + (pos - Waist_collider->gameobject->transform->position).unit_vect() * pow;
		}

		//ジャンプ
		{
			if (is_jumping == true)coyote += Al_Global::second_per_frame();
			if (coyote >= 0)is_jumping = false;
			if (is_jumping == false && Waist_capsule_collider->concoll_enter(Collider_tags::Stage)) coyote = 0.3f;
			if (is_jumping == false && !Waist_capsule_collider->concoll_enter(Collider_tags::Stage)) coyote -= Al_Global::second_per_frame();


			if (coyote >= 0 && input->getKeyTrigger(Key::Space)) {
				//Waist_collider->add_force(Vector3(0, 1, 0) * jump_power * Al_Global::second_per_frame());
				Waist_collider->linear_velocity(Vector3(Waist_collider->linear_velocity().x, 150, Waist_collider->linear_velocity().z));
				is_jumping = true;
				coyote = -0.3;
			}
		}
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
