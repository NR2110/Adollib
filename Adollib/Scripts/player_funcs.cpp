#include "player.h"

#include "player.h"

#include "../Adollib/Scripts/Main/Adollib.h"
#include "../Adollib/Scripts/Object/gameobject_manager.h"

#include "../Adollib/Scripts/Physics/ray.h"

using namespace Adollib;

//���L�΂�
void Player::reach_out_hands() {

	const Mouse keys[2] = {
		Mouse::LBUTTON ,
		Mouse::RBUTTON
	};
	Collider* colliders[6] = {
		Lsholder_collider,
		Rsholder_collider,
		Lelbow_collider,
		Relbow_collider,
		Lhand_collider,
		Rhand_collider
	};
	const int sign[2] = {
		+1,
		-1
	};
	Joint_base* joints[2] = {
		catch_left_joint,
		catch_right_joint
	};

	for (int i = 0; i < 2; i++) {
		const Mouse key = keys[i];
		if (input->getMouseState(key)) {
			Joint_base* joint = joints[i];

			//�J�����̉�]����r���グ�鍂����ς���
			Quaternion camera_off;
			{
				Vector3 camera_vec = vector3_quatrotate(Vector3(0, 0, 1), camera->orientation).unit_vect();
				Vector3 camera_vec_y0 = camera_vec;
				camera_vec_y0.y = 0;
				camera_vec_y0 = camera_vec_y0.unit_vect();

				Vector3 rot_axis = vector3_cross(camera_vec, camera_vec_y0).unit_vect();
				float radian = vector3_radian(camera_vec, camera_vec_y0) * hand_camera_rot_pow;
				if (camera_vec.y < 0)
					radian *= -1;
				radian = ALClamp(radian + hand_camera_rot_center * hand_camera_rot_pow, -hand_camera_max_rot, +hand_camera_max_rot);
				camera_off = quaternion_axis_radian(Vector3(1, 0, 0), radian);
			}

			Quaternion goal = quaternion_axis_angle(Vector3(0, 1, 0), sign[i] * 90) * quaternion_axis_angle(Vector3(1, 0, 0), 90) * camera_off * Body_collider->transform->orientation;

			//quaternion��-Q�̎��A���������]���t��]�ɂȂ邽��localorint�� - ��������
			{
				const Quaternion& off = colliders[i]->transform->orientation * goal.inverse();
				if (off.angle() < 180) {
					const Vector3& sholder_dir = vector3_quatrotate(Vector3(0, -1, 0), colliders[i]->transform->orientation);
					const Vector3& waist_dir = vector3_quatrotate(Vector3(0, -1, 0), Waist_collider->transform->orientation);
					if (vector3_dot(sholder_dir, waist_dir) > 0) {
						colliders[i]->transform->local_orient *= -1;
						colliders[i + 2]->transform->local_orient *= -1;
					}
				}

			}

			float debug = 1;
			//if (joint != nullptr && joint->get_colliderB()->physics_data.is_moveable == false) {
			//	debug = 3;
			//}

			{
				// ��
				auto& collider = colliders[i];
				Quaternion off = collider->transform->orientation * goal.inverse();
				float pow = ALClamp(off.radian() * hand_rot_pow, 0, hand_rot_max_pow);
				collider->add_torque(off.axis() * pow * 1 * collider->physics_data.inertial_mass);
				collider->set_max_angula_velocity(hand_rot_max_speed);
			}
			{
				//�r
				auto& collider = colliders[i + 2];
				Quaternion off = collider->transform->orientation * goal.inverse();
				float pow = ALClamp(off.radian() * hand_rot_pow, 0, hand_rot_max_pow);
				collider->add_torque(off.axis() * pow * 1 * collider->physics_data.inertial_mass);
				collider->set_max_angula_velocity(hand_rot_max_speed * 0.8f);
			}
			//{
			//	//�r
			//	auto& collider = colliders[i + 4];
			//	Quaternion off = collider->transform->orientation * goal.inverse();
			//	float pow = ALClamp(off.radian() * hand_rot_pow, 0, hand_rot_max_pow);
			//	collider->add_torque(off.axis() * pow * 3 * collider->physics_data.inertial_mass);
			//	collider->set_max_angula_velocity(hand_rot_max_speed);
			//}
		}
	}

	/*
	{
		const Vector3& sholder_dir = vector3_quatrotate(Vector3(0, -1, 0), colliders[0]->transform->orientation);
		const Vector3& waist_dir = vector3_quatrotate(Vector3(0, -1, 0), Waist_collider->transform->orientation);
		Debug::set("waist_dir", waist_dir);

		Quaternion camera_off;
		{
			Vector3 camera_vec = vector3_quatrotate(Vector3(0, 0, 1), camera->orientation).unit_vect();
			Vector3 camera_vec_y0 = camera_vec;
			camera_vec_y0.y = 0;
			camera_vec_y0 = camera_vec_y0.unit_vect();

			Vector3 rot_axis = vector3_cross(camera_vec, camera_vec_y0).unit_vect();
			float radian = ALClamp(vector3_radian(camera_vec, camera_vec_y0) * hand_camera_rot_pow, -hand_camera_max_rot, +hand_camera_max_rot);
			camera_off = quaternion_axis_radian(rot_axis, radian);

			Debug::set("rot_axis", rot_axis);
			Debug::set("radian", radian);
			Debug::set("camera_off", camera_off);
		}

		const Mouse key = keys[0];
		auto& collider = colliders[0];

		//addforce�̌�����K�v�ɉ����ċt�ɂ��Ȃ���΂����Ȃ� �v�C��
		Quaternion goal = quaternion_axis_angle(Vector3(0, 1, 0), sign[0] * 90) * quaternion_axis_angle(Vector3(1, 0, 0), 90) * camera_off * Body_collider->transform->orientation;
		Quaternion off = collider->transform->orientation * goal.inverse();

		Debug::set("Lsholder_orient", collider->transform->orientation);
		Debug::set("Lelbow_orient", colliders[2]->transform->orientation);
		Debug::set("Lhand_goal", goal);
		Debug::set("Body_collider->transform->orientation", Body_collider->transform->orientation);
		Debug::set("Lhand_start_eular", collider->transform->orientation.euler());
		Debug::set("Lhand_goal_vec", vector3_quatrotate(Vector3(0, -1, 0), goal));
		Debug::set("Lhand_axis", off.axis());
		Debug::set("Lhand_angle", off.angle());


	}
	*/


};

//��������
void Player::catch_things() {
	const Mouse keys[2] = {
		Mouse::LBUTTON ,
		Mouse::RBUTTON
	};

	Collider* colliders[6] = {
		Lhand_collider,
		Rhand_collider,
		Lelbow_collider,
		Relbow_collider,
		Lsholder_collider,
		Rsholder_collider
	};
	Joint_base** joints[2] = {
		&catch_left_joint,
		&catch_right_joint
	};

	for (int i = 0; i < 2; i++) {
		const Mouse key = keys[i];
		auto& collider = colliders[i];
		Joint_base*& joint = *joints[i];
		//����
		if (input->getMouseState(key) && joint == nullptr) {
			collider->is_save_contacted_colls = true;
			auto& contacted_colls = collider->contacted_colliders;

			//�Փ˂��Ă���collider�����ԋ߂����̂�T��
			Contacted_data* min_data = nullptr;
			for (auto& c_coll : contacted_colls) {
				if (min_data == nullptr || min_data->penetrate > c_coll.penetrate) {
					min_data = &c_coll;
				}
			}
			if (min_data != nullptr && min_data->coll->tag != Collider_tags::Human) {
				//Joint�ɓo�^
				joint = Joint::add_balljoint(
					collider,
					min_data->coll,
					min_data->contacted_pointA,
					min_data->contacted_pointB
				);
				joint->slop = 0.1f;
			}
		}

		//����
		if (!input->getMouseState(key) && joint != nullptr) {
			joint->get_colliderB()->tag &= ~Collider_tags::Having_Stage;
			joint->get_colliderB()->tag |= Collider_tags::Stage;
			Joint::delete_joint(joint);

			//collider->physics_data.is_hitable = false;
		}

		// static�Ȃ��̂ɋ߂���(hand���珇)�Ɏ��ʂ�傫������
		if (joint != nullptr && joint->get_colliderB()->physics_data.is_moveable == false) {
			// static�Ȃ��̂ɋ߂���(hand���珇)�Ɏ��ʂ�傫������
			colliders[i]->physics_data.inertial_mass = shlder_mass * 10;
			colliders[i + 2]->physics_data.inertial_mass = elbow_mass * 10;
			colliders[i + 4]->physics_data.inertial_mass = hand_mass * 10;
		}
		else {
			// �����l�̎��ʂ�ݒ�
			colliders[i]->physics_data.inertial_mass = hand_mass;
			colliders[i + 2]->physics_data.inertial_mass = elbow_mass;
			colliders[i + 4]->physics_data.inertial_mass = shlder_mass;
			//colliders[i]->physics_data.inertial_mass = shlder_mass * 2;
			//colliders[i + 2]->physics_data.inertial_mass = elbow_mass * 2;
			//colliders[i + 4]->physics_data.inertial_mass = hand_mass * 2;
		}

	}

	//TODO : ����ł�����̂�Stage tag�������āA����Ă���Ƃ���jump�ł��Ȃ��悤�ɂ��Ă���
	for (int i = 0; i < 2; i++) {
		Joint_base*& joint = *joints[i];
		if (joint == nullptr)continue;
		joint->get_colliderB()->tag |= Collider_tags::Having_Stage;
		joint->get_colliderB()->tag &= ~Collider_tags::Stage;
	}
};

//�����x����waist_pillar�𒲐�
void Player::tuning_waist_pillar() {
	const Mouse keys[2] = {
		Mouse::LBUTTON ,
		Mouse::RBUTTON
	};
	Collider* colliders[6] = {
		Lhand_collider,
		Rhand_collider,
		Lelbow_collider,
		Relbow_collider,
		Lsholder_collider,
		Rsholder_collider
	};
	Joint_base** joints[2] = {
		&catch_left_joint,
		&catch_right_joint
	};

	is_gunyatto = false;

	//waist_sphere��y�����̒���
	float center_y = waist_pillar->center.y;
	{
		float anchor_move_vec = 0;
		if (!check_standable_collider->concoll_enter(Collider_tags::Stage) && is_jumping == false) {
			anchor_move_vec = -0.2f;
		}
		else anchor_move_vec = 3;
		for (int i = 0; i < 2; i++) {
			const Mouse key = keys[i];
			auto& collider = colliders[i];
			Joint_base*& joint = *joints[i];
			//static�Ȃ��̂������Ă���Ƃ��A
			if (joint != nullptr && joint->get_colliderB()->physics_data.is_moveable == false) {
				//waist_sphere->center = Vector3(0);
				check_standable_collider_timer = 1;
				is_gunyatto = true;
			}
			else {
				check_standable_collider_timer -= Al_Global::second_per_frame;

				if (check_standable_collider_timer > 0)anchor_move_vec = 3;
			}
		}

		center_y += -1 * anchor_move_vec * Al_Global::second_per_frame;
		center_y = ALClamp(center_y, Waist_pillar_max_y, 0);
	}

	Vector3 center_xz = waist_pillar->center;
	{
		center_xz.y = 0;
		center_xz += vector3_quatrotate(-dir, Waist->transform->orientation) * 0.01f;

		if (center_xz.norm() > Waist_pillar_max_xz * Waist_pillar_max_xz)center_xz = center_xz.unit_vect() * Waist_pillar_max_xz;
	}

	//waist_pillar->center = Vector3(center_xz.x, center_y, center_xz.z);
	waist_pillar->center = Vector3(0, center_y, -0.3f);
	//waist_pillar->center = vector3_quatrotate(-dir, Waist->transform->orientation) +Vector3(0, waist_pillar->center.y,0);

	//if()
}

//�ړ�
void Player::linear_move() {
	const float gnyat_pow = 0.9f;
	if (!input->getKeyState(Key::LeftControl))
	{
		Debug::set("linear_velocity", Waist_collider->linear_velocity());
		//�ړ�
		Waist_collider->add_force(dir * waist_move_pow * gnyat_pow);

		// ���x����
		Vector3 speed = Waist_collider->linear_velocity();
		speed.y = 0;
		if (speed.norm() > waist_move_max_speed * waist_move_max_speed) {
			Vector3 Yspeed = Vector3(0, Waist_collider->linear_velocity().y, 0);
			Waist_collider->linear_velocity(speed.unit_vect() * waist_move_max_speed + Yspeed);
		}
	}
	// �n�ʂɐڂ��Ă��� & ���͂�������
	if (dir.norm() == 0 && onground_collider->concoll_enter(Collider_tags::Stage)) {
		Waist_collider->physics_data.drag = 0.8f;
	}
	else Waist_collider->physics_data.drag = 0.1;
};

//��]
void Player::angula_move() {
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

	}

	if (is_gunyatto == true) {
		turn_gunyatto_dir();
	}

};

//�ړ��������v�Z
void Player::accume_move_dir() {
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
	dir = vector3_quatrotate(move_vec, camera->orientation).unit_vect();
	dir.y = 0;
	dir = dir.unit_vect();
};

//���悤�ɗ͂�������
void Player::add_pow_for_stand() {
	float gnyat_pow = 0.9f;
	if (!input->getKeyState(Key::LeftControl) && is_gunyatto == false)
	{
		{
			//�炪�Ԃ����Ȃ̂�����
			Head_collider->physics_data.anglar_drag = 1.f;
			Quaternion off = Body_collider->gameobject->transform->orientation * Head_collider->gameobject->transform->orientation.inverse();
			float pow = ALClamp(off.radian() * head_rot_pow, 0, head_rot_max_pow);
			Head_collider->add_torque(off.axis() * pow * gnyat_pow);
		}
		{
			//���̂���������
			Quaternion off = rotate * Waist_collider->gameobject->transform->orientation.inverse();

			//float k = 1;
			//float t = inv60;
			//float omega = sqrtf(k / Waist_collider->physics_data.inertial_mass);
			//float next = off.radian() * k * cosf(t * omega);
			//float pow = k * (next * t + 0.5f * t * (off.radian() - next));
			//pow = ALClamp(pow * waist_rot_pow, 0, waist_rot_max_pow);

			float pow = ALClamp(off.radian() * waist_rot_pow, 0, waist_rot_max_pow);
			Waist_collider->add_torque(off.axis() * pow * gnyat_pow);

		}
		{
			//���̂���������
			Quaternion off = rotate * Body_collider->gameobject->transform->orientation.inverse();
			float pow = ALClamp(off.radian() * body_rot_pow, 0, body_rot_max_pow);
			Body_collider->add_torque(off.axis() * pow * gnyat_pow);
		}

	}

	{
		//joint�łȂ���Əd�͂��キ�Ȃ邩�� & ���̃p�[�c�͏d�͂̉e�����󂯂Ȃ��悤�ɂ��Ă��邩��  �������ɗ͂�������
		Waist_collider->add_force(Vector3(0, -1, 0) * 150);
		Body_collider->add_force(Vector3(0, -1, 0) * 150);
		Lfoot_collider->add_force(Vector3(0, -1, 0) * 50);
		Rfoot_collider->add_force(Vector3(0, -1, 0) * 50);
	}
};

//���𓮂���
void Player::move_legs() {
	//Waist_collider->physics_data.is_moveable = false;
		//Rleg_collider->physics_data.is_moveable = false;
		//Lleg_collider->physics_data.is_moveable = false;
	if (dir.norm() != 0) {
		float front_value = ToRadian(30); //���͓��̂��x��邽�� ����O�ɏo�����߂ɗ]���ɂ������]�̗�
		float max_theata = ToRadian(60) * 0.5f;
		float sin = sinf(move_timer * 1.5f * DirectX::XM_2PI); //timre���瑫�̉�]�ʂ����߂�
		//float sin = sinf(move_timer * DirectX::XM_2PI * 0.5f); //timre���瑫�̉�]�ʂ����߂�

		Vector3 waist_axis = Vector3(0, -1, 0);
		Vector3 rot_axis = Vector3(1, 0, 0);

		//if (1) {
		//	//���������Α������ɓ������� �͂��������܂������Ȃ�
		//	rot_axis = vector3_cross(dir, -waist_axis).unit_vect();
		//	rot_axis = vector3_quatrotate(rot_axis, Waist_collider->transform->orientation.inverse());
		//}

		Debug::set("rot_axis_local", rot_axis);
		Debug::set("rot_axis_world", vector3_quatrotate(rot_axis, Waist_collider->transform->orientation.inverse()));



		Collider* collider[2]{
			Rleg_collider,
			Lleg_collider
		};
		Quaternion goal_rotate[2] = {
			quaternion_axis_radian(rot_axis, +sin * max_theata + front_value) * Waist_collider->transform->orientation,
			quaternion_axis_radian(rot_axis, -sin * max_theata + front_value) * Waist_collider->transform->orientation
		};
		//Quaternion goal_rotate[2] = {
		//	quaternion_axis_radian(rot_axis, +sin * max_theata + front_value),
		//	quaternion_axis_radian(rot_axis, -sin * max_theata + front_value)
		//};
		Vector3 goal_vec[2] = {
			vector3_quatrotate(waist_axis, goal_rotate[0].inverse()),
			vector3_quatrotate(waist_axis, goal_rotate[1].inverse())
		};
		Vector3 now_vec[2] = {
			vector3_quatrotate(Vector3(0, -1, 0), Rleg->transform->orientation),
			vector3_quatrotate(Vector3(0, -1, 0), Lleg->transform->orientation)
		};
		Vector3 axis[2] = {
			vector3_cross(now_vec[0], goal_vec[0]),
			vector3_cross(now_vec[1], goal_vec[1])
		};
		Quaternion off[2] = {
			goal_rotate[0] * collider[0]->transform->orientation.inverse(),
			goal_rotate[1] * collider[1]->transform->orientation.inverse()
		};

		//if (sinf(move_timer * DirectX::XM_2PI * 0.5f + PI_inv2) > 0) axis[0] *= -1;
		//else axis[1] *= -1;

		//Debug::set("sin", sin);
		//Debug::set("waist_axis", waist_axis);
		//Debug::set("rot_axis", rot_axis);
		//Debug::set("goal_vec[R]", goal_vec[0]);
		////Debug::set("goal_vec[L]", goal_vec[1]);
		//Debug::set("now_vec[R]", now_vec[0]);
		////Debug::set("now_vec[L]", now_vec[1]);

		for (int i = 0; i < 2; i++) {
			//float radian = vector3_radian(now_vec[i], goal_vec[i]);
			//if (axis[i].norm() < FLT_EPSILON)continue;
			//axis[i] = axis[i].unit_vect();

			auto axis = off[i].axis();

			float pow = ALClamp(off[i].radian() * leg_rot_pow, 0, leg_rot_max_pow);

			collider[i]->add_torque(axis * pow);
			collider[i]->set_max_angula_velocity(leg_rot_max_speed);
			//collider[i]->gameobject->transform->local_orient = goal_rotate[i];
			//collider[i]->physics_data.is_moveable = false;
			//Debug::set("pow", pow);
		}

		move_timer += Al_Global::second_per_frame;
	}
};

//jump������
void Player::make_jump() {
	if (is_jumping == true)coyote += Al_Global::second_per_frame;
	if (coyote >= 0)is_jumping = false;
	if (is_jumping == false && onground_collider->concoll_enter(Collider_tags::Stage)) coyote = 0.3f;
	if (is_jumping == false && !onground_collider->concoll_enter(Collider_tags::Stage)) coyote -= Al_Global::second_per_frame;


	if (coyote >= 0 && input->getKeyTrigger(Key::Space)) {
		for (int i = 0; i < Human_collider_size; i++) {
			Human_colliders[i]->linear_velocity(Vector3(Human_colliders[i]->linear_velocity().x, jump_power, Human_colliders[i]->linear_velocity().z));
		}
		Lleg_collider->linear_velocity(Vector3(Lleg_collider->linear_velocity().x, jump_power * 0.1f, Lleg_collider->linear_velocity().z));
		Rleg_collider->linear_velocity(Vector3(Rleg_collider->linear_velocity().x, jump_power * 0.1f, Rleg_collider->linear_velocity().z));
		Lfoot_collider->linear_velocity(Vector3(Lfoot_collider->linear_velocity().x, jump_power * 0.1f, Lfoot_collider->linear_velocity().z));
		Rfoot_collider->linear_velocity(Vector3(Rfoot_collider->linear_velocity().x, jump_power * 0.1f, Rfoot_collider->linear_velocity().z));
		//Head_collider->linear_velocity(Vector3(Head_collider->linear_velocity().x, jump_power, Head_collider->linear_velocity().z));
		//Body_collider->linear_velocity(Vector3(Body_collider->linear_velocity().x, jump_power, Body_collider->linear_velocity().z));
		//Waist_collider->linear_velocity(Vector3(Waist_collider->linear_velocity().x, jump_power, Waist_collider->linear_velocity().z));

		is_jumping = true;
		coyote = -0.3f;
	}
};



void Player::turn_gunyatto_dir() {
	Vector3 dir = vector3_quatrotate(Vector3(0, 0, 1), Waist->transform->orientation);

	if (dir.y < -0.9f) dir = vector3_quatrotate(Vector3(0, 1, 0), Waist->transform->orientation);
	else if(dir.y > 0.9f) dir = vector3_quatrotate(Vector3(0, -1, 0), Waist->transform->orientation);


	dir.y = 0;
	dir = dir.unit_vect();
	if (dir.norm() == 0) {
		return;
	}

	Vector3 player_vec = vector3_quatrotate(Vector3(0, 0, 1), rotate);

	float angle = vector3_angle(dir, player_vec);

	if (vector3_cross(player_vec, dir).y < 0)angle *= -1;

	rotate *= quaternion_axis_angle(Vector3(0, 1, 0), angle);

}