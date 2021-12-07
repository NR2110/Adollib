#include "player.h"

#include "../Adollib/Scripts/Main/Adollib.h"
#include "../Adollib/Scripts/Object/gameobject_manager.h"

#include "../Adollib/Scripts/Physics/ALP__physics_manager.h"
#include "../Adollib/Scripts/Physics/ray.h"
#include "../Adollib/Scripts/Imgui/work_meter.h"

#include "stage_manager.h"
#include "stage_base.h"
#include "camera.h"
#include "input_changer.h"



using namespace Adollib;

// ���L�΂�
void Player::reach_out_hands() {

	const bool input_arm[2] = {
		input_changer->is_Larm_state,
		input_changer->is_Rarm_state
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
		if (input_arm[i]) {
			Joint_base* joint = joints[i];

			//�J�����̉�]����r���グ�鍂����ς���
			Quaternion camera_off;
			{
				Vector3 camera_vec = vector3_quatrotate(Vector3(0, 0, 1), camera->camera_rot).unit_vect();
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

			//float elbow_pow = 0;
			{
				//�r
				auto& collider = colliders[i + 2];
				Quaternion off = collider->transform->orientation * goal.inverse();
				Vector3 axis = off.axis();
				float rad = off.radian();
				if (rad > PI) rad = 2 * PI - rad;

				float pow = ALClamp(rad * hand_rot_pow, 0, hand_rot_max_pow);
				Vector3 world_axis = vector3_quatrotate(axis, collider->transform->orientation);
				collider->add_torque(world_axis* pow * collider->physics_data.inertial_mass);
				collider->set_max_angula_velocity(ALmin(hand_rot_max_speed, hand_rot_max_speed * ToAngle(rad) * 0.03f));

			}

			{
				// ��
				auto& collider = colliders[i];
				Quaternion off = collider->transform->orientation * goal.inverse();
				Vector3 axis = off.axis();
				float rad = off.radian();
				if (rad > PI) rad = 2 * PI - rad;

				float pow = ALClamp(rad * hand_rot_pow,  0, hand_rot_max_pow);
				Vector3 world_axis = vector3_quatrotate(axis, collider->transform->orientation);
				collider->add_torque(world_axis * pow * collider->physics_data.inertial_mass);
				collider->set_max_angula_velocity(ALmin(hand_rot_max_speed, hand_rot_max_speed * ToAngle(rad) * 0.03f));

			}

		}
		else {
			colliders[i]->set_max_angula_velocity(FLT_MAX);
			colliders[i + 2]->set_max_angula_velocity(FLT_MAX);
		}
	}

};

// ��������
void Player::catch_things() {

	const bool input_arm[2] = {
		input_changer->is_Larm_state,
		input_changer->is_Rarm_state
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
	BallJoint* hand_joints[2] = {
		Lhand_joint,
		Rhand_joint
	};
	float hand_joint_ylength_default[2] = {
		Lhand_joint_ylength_default,
		Rhand_joint_ylength_default
	};

	bool* is_maked_joint[2] = {
		&is_maked_left_joint,
		&is_maked_right_joint
	};

	// stage���ύX���ꂽ��gameobject�̍폜��joint�͏����Ă���̂� joint���폜
	if (stage_manager->get_is_changed_stage()) {
		joints[0] = nullptr;
		joints[1] = nullptr;
		return;
	}

	for (int i = 0; i < 2; i++) {
		auto& collider = colliders[i];
		Joint_base*& joint = *joints[i];
		//����
		if (input_arm[i] && joint == nullptr && *is_maked_joint[i] == false) { //Key��������Ă��� joint�����݂��Ȃ� ��state���ɕ�������ł��Ȃ�
			collider->is_save_contacted_colls = true;
			auto contacted_colls = collider->get_Contacted_data();

			//�Փ˂��Ă���collider�����ԋ߂����̂�T��
			Contacted_data const* min_data = nullptr;
			for (auto& c_coll : contacted_colls) {
				if (min_data == nullptr || min_data->penetrate > c_coll.penetrate) {
					min_data = &c_coll;

				}
			}
			//joint�𐶐�
			if (min_data != nullptr && min_data->coll->tag != Collider_tags::Human) {
				*is_maked_joint[i] = true;

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
		{
			// ��������ł�����flag��false��(�����ł͂��ꂽ�Ƃ��A���񂾂Ƃ��ɉ������ςȂ��ł����܂Ȃ��悤��)
			if (!input_arm[i]) {
				*is_maked_joint[i] = false;
			}

			// �{�^����������Ă��Ȃ��āA��������ł�����
			if (!input_arm[i] && joint != nullptr) {
				Joint::delete_joint(joint);
			}

			// ��������
			if (joint != nullptr) {
				joint->adapt_anchor();
				Vector3 world_posA = joint->get_colliderA()->transform->position + vector3_quatrotate(joint->get_anchors()[0].posA, joint->get_colliderA()->transform->orientation);
				Vector3 world_posB = joint->get_colliderB()->transform->position + vector3_quatrotate(joint->get_anchors()[0].posB, joint->get_colliderB()->transform->orientation);

				if (
					(world_posA - world_posB).norm() > 2 * 2
					//(colliders[i]->transform->position - colliders[i + 2]->transform->position).norm() > 1 * 1
					) {
					Joint::delete_joint(joint);
				}

			}

		}

		// ����ł���Ƃ�hand��joint�̈ʒu��L�΂�
		if (joint != nullptr) {
			hand_joints[i]->anchor.posA.y -= 0.1f * time->deltaTime();
			if (hand_joints[i]->anchor.posA.y < -0.5f)hand_joints[i]->anchor.posA.y = -0.5f;
		}
		else {
			hand_joints[i]->anchor.posA.y += 0.1f * time->deltaTime();
			if (hand_joints[i]->anchor.posA.y > hand_joint_ylength_default[i]) hand_joints[i]->anchor.posA.y = hand_joint_ylength_default[i];
		}

	}
};

// ���悤�ɗ͂�������
void Player::add_pow_for_stand() {

	if (is_gunyatto == false)
	{
		{
			//������������
			Quaternion off = rotate * Waist_collider->gameobject->transform->orientation.inverse();

			float rad = off.radian();
			if (rad > PI)rad = 2 * PI - rad;
			float pow = ALClamp(rad * waist_rot_pow, 0, waist_rot_max_pow);
			Waist_collider->add_torque(off.axis() * pow * gunyatto_pow, true);
			Waist_collider->set_max_angula_velocity(waist_rot_max_speed);

		}
		{
			//�̂���������
			Quaternion off = rotate * Body_collider->gameobject->transform->orientation.inverse();
			//Quaternion off = Waist_collider->gameobject->transform->orientation * Body_collider->gameobject->transform->orientation.inverse();

			float rad = off.radian();
			if (rad > PI)rad = 2 * PI - rad;
			float pow = ALClamp(rad * body_rot_pow, 0, body_rot_max_pow);
			Body_collider->add_torque(off.axis() * pow * gunyatto_pow, true);
			Body_collider->set_max_angula_velocity(body_rot_max_speed);
			//Body_collider->set_max_angula_velocity(ALmin(body_rot_max_speed, body_rot_max_speed * ToAngle(rad) * 0.1f));
		}
		{
			//�炪�Ԃ����Ȃ̂�����
			Quaternion off = Body_collider->gameobject->transform->orientation * Head_collider->gameobject->transform->orientation.inverse();
			float rad = off.radian();
			if (rad > PI)rad = 2 * PI - rad;
			float pow = ALClamp(rad * head_rot_pow, 0, head_rot_max_pow);
			Head_collider->add_torque(off.axis() * pow * gunyatto_pow, true);
			Head_collider->set_max_angula_velocity(ALmin(head_rot_max_speed, head_rot_max_speed * ToAngle(rad) * 0.03f)); //�p�x�̍��ɂ���čő呬�x�𒲐�
		}

	}
	// gunyatto���Ă�������
	else {
		// �����̂ݍX�V
		turn_gunyatto_dir();
	}



};

// ray���΂��č��𗧂�����
void Player::push_waist_for_stand() {
	if (is_gunyatto == true)return;

	float dot = vector3_dot(Vector3(0, -1, 0), vector3_quatrotate(Vector3(0, -1, 0), Waist_collider->transform->orientation));
	if (dot < 0)return; //�������������Ă��Ȃ����return
	dot = dot * dot;
	//dot = 1;

	if (onground_collider != nullptr) {

		float dis = vector3_dot(onground_contactpoint - Waist_collider->transform->position, Vector3(0, -1, 0));

		if (dis < onground_dis) {
			constexpr float mass = 10 * 4.5f; //�p�[�c�ɂ���Ď��ʂ��܂��܂��Ȃ̂� ���������̎���
			constexpr float stand_pow = 1000;


			Debug::set("onground_contactpoint_vec", (onground_contactpoint - Waist_collider->transform->position).unit_vect());

			Vector3 gravity_pow = Vector3(0, 1, 0) * Physics_function::Physics_manager::physicsParams.gravity * mass; //player���؋�o�����
			Vector3 force = Vector3(0, 1, 0) * (onground_dis * dot - dis) * stand_pow; //�߂荞�݂�������
			Vector3 fall_force = Vector3(0, 1, 0) * Waist_collider->linear_velocity().y * mass;

			Waist_collider->add_force(gravity_pow + force);
			onground_ray_data.coll->add_force(-(gravity_pow + fall_force) , onground_contactpoint);

		}
			Debug::set("dis", dis);
	}

}

// �ړ�
void Player::linear_move() {
	// �n�ʂɐڂ��Ă��� & ���͂�������
	//if (is_gunyatto == false && dir.norm() == 0 && onground_collider != nullptr) {
	if (is_gunyatto == false && dir.norm() == 0 ) {
		Waist_collider->physics_data.drag = 0.985f;
		Body_collider->physics_data.drag = 0.985f;
	}
	// ���͂�����Γ����₷���悤��0.1�ɂ���
	else {
		Waist_collider->physics_data.drag = 0.1f;
		Body_collider->physics_data.drag = 0.1f;
	}

	float move_pow = 1;
	if (onground_collider == nullptr) {
		move_pow = 0.3f;
	}

	{
		//�ړ�
		Waist_collider->add_force(dir * waist_move_pow * move_pow);

		// ���x����
		Vector3 speed = Waist_collider->linear_velocity();
		speed.y = 0;
		if (speed.norm() > waist_move_max_speed * waist_move_max_speed) {
			Vector3 Yspeed = Vector3(0, Waist_collider->linear_velocity().y, 0);
			Waist_collider->linear_velocity(speed.unit_vect() * waist_move_max_speed + Yspeed);
		}

		if (onground_collider != nullptr) {
			if (onground_collider->physics_data.inertial_mass > 20) //�����Ƃ��ɏ����Ȑ΂���ɑS�͂������Ȃ�
				onground_collider->add_force(-dir * waist_move_pow * move_pow, onground_contactpoint);
		}

	}
};

// ��]
void Player::angula_move() {

	if (input_changer->dir.norm() != 0) {
		Vector3 rot_vec = vector3_quatrotate(Vector3(0, 0, -1), camera->camera_rot);
		rot_vec.y = 0;
		rot_vec = rot_vec.unit_vect();

		Vector3 player_vec = vector3_quatrotate(Vector3(0, 0, 1), rotate);

		float angle = vector3_angle(rot_vec, player_vec);
		if (angle > turn_speed)angle = turn_speed;

		if (vector3_cross(player_vec, rot_vec).y < 0)angle *= -1;

		rotate *= quaternion_axis_angle(Vector3(0, 1, 0), angle * Al_Global::second_per_frame * 100);
	}

};

// �ړ��������v�Z
void Player::accume_move_dir() {
	dir = vector3_quatrotate(input_changer->dir, camera->camera_rot).unit_vect();
	dir.y = 0;
	//dir = vector3_quatrotate(dir, quaternion_from_euler(0, 180, 0));
	dir = dir.unit_vect();

	//debug_coll->transform->local_pos = Waist->world_position() + dir * 5;
};

// ���𓮂���
void Player::move_legs() {
	//return;
	if (is_gunyatto)return;

	//Waist_collider->physics_data.is_moveable = false;
	//Rleg_collider->physics_data.is_moveable = false;
	//Lleg_collider->physics_data.is_moveable = false;
	if (dir.norm() != 0) {
		const float front_value = ToRadian(30); //���͓��̂��x��邽�� ����O�ɏo�����߂ɗ]���ɂ������]�̗�
		const float max_theata = ToRadian(60) * 0.5f;
		const float sin = sinf(move_timer * 1.5f * DirectX::XM_2PI); //timre���瑫�̉�]�ʂ����߂�
		//float sin = sinf(move_timer * DirectX::XM_2PI * 0.5f); //timre���瑫�̉�]�ʂ����߂�

		const Vector3 waist_axis = Vector3(0, -1, 0);
		Vector3 rot_axis = Vector3(1, 0, 0);

		// �����Ă�����������]���𒲐� ���ɕ�����悤��
		rot_axis = vector3_cross(dir, -waist_axis).unit_vect();
		rot_axis = vector3_quatrotate(rot_axis, Waist_collider->transform->orientation.inverse());

		Collider* collider[2]{
			Rleg_collider,
			Lleg_collider
		};
		Quaternion goal_rotate[2] = {
			quaternion_axis_radian(rot_axis, +sin * max_theata + front_value) * Waist_collider->transform->orientation,
			quaternion_axis_radian(rot_axis, -sin * max_theata + front_value) * Waist_collider->transform->orientation
		};
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

		for (int i = 0; i < 2; i++) {

			auto axis = off[i].axis();

			float rad = off[i].radian();
			if (rad > PI) {
				rad = 2 * PI - rad;
				axis *= -1;
			}
			float pow = ALClamp(rad * leg_rot_pow, 0, leg_rot_max_pow);

			collider[i]->add_torque(axis * pow, true);
			collider[i]->set_max_angula_velocity(ALmin(leg_rot_max_speed, leg_rot_max_speed * ToAngle(rad) * 0.1f));
		}

		move_timer += Al_Global::second_per_frame;
	}
};

// jump������
void Player::make_jump() {
	if (is_gunyatto)return;

	bool is_jumpable = false;
	if (onground_collider != nullptr && //�ڒn���Ă���
		(catch_right_joint == nullptr || catch_right_joint->get_colliderB() != onground_collider) && //�E��Ŏ����Ă�����̂ł͂Ȃ�
		(catch_left_joint == nullptr || catch_left_joint->get_colliderB() != onground_collider)    //����Ŏ����Ă�����̂ł͂Ȃ�
		)is_jumpable = true;

	if (is_jumping == true)coyote += Al_Global::second_per_frame;
	if (coyote >= 0)is_jumping = false;
	if (is_jumping == false && is_jumpable) coyote = 0.3f;
	if (is_jumping == false && !is_jumpable) coyote -= Al_Global::second_per_frame;

	if (is_gunyatto == false && coyote >= 0 && input_changer->is_jump_trigger) {

		if (onground_collider != nullptr) {

			// Y�����ɗ͂�������
			for (int i = 0; i < Human_collider_size; i++) {
				Human_colliders[i]->linear_velocity(Vector3(Human_colliders[i]->linear_velocity().x, jump_y_power, Human_colliders[i]->linear_velocity().z));
			}
			Lleg_collider->linear_velocity(Vector3(Lleg_collider->linear_velocity().x, jump_y_power * 0.1f, Lleg_collider->linear_velocity().z));
			Rleg_collider->linear_velocity(Vector3(Rleg_collider->linear_velocity().x, jump_y_power * 0.1f, Rleg_collider->linear_velocity().z));
			Lfoot_collider->linear_velocity(Vector3(Lfoot_collider->linear_velocity().x, jump_y_power * 0.1f, Lfoot_collider->linear_velocity().z));
			Rfoot_collider->linear_velocity(Vector3(Rfoot_collider->linear_velocity().x, jump_y_power * 0.1f, Rfoot_collider->linear_velocity().z));

			// Z�����ɗ͂�������
			for (int i = 0; i < Human_collider_size; i++) {
				float pow = vector3_dot(Human_colliders[i]->linear_velocity(), dir) * Human_colliders[i]->physics_data.inertial_mass;
				Human_colliders[i]->add_force(dir * (jump_front_power - pow * 2));
			}

			// �����̕��̂ɗ͂�������
			float mass = 1500;
			onground_collider->add_force(Vector3(0, -mass, 0) * jump_y_power, onground_contactpoint);


			is_jumping = true;
			coyote = -0.3f;

		}
	}
};

// respown����
bool Player::check_respown() {

	auto stage = stage_manager->get_current_stage();
	// ���͂����� || stage�w���Y���W���Player���Ⴏ���respown
	if (input_changer->is_respown_trigger || Waist->world_position().y < stage->y_player_respown_limit + 50) {

		// goal���Ă����� respown�������s�킸 stage�ؑ֏������Ă�
		if (stage->next_stage != Stage_types::none) {
			stage_manager->set_next_stage_type(stage->next_stage);
			return true;
		}

		// ���W�ړ�
		Vector3 off = vector3_quatrotate(stage->player_respown_pos - Waist->world_position(), Waist->parent()->world_orientate().inverse());
		for (int i = 0; i < Human_gameobject_size; i++) {
			Human_gameobjects[i]->transform->local_pos += off;
		}

		// �͂�reset
		for (int i = 0; i < Human_collider_size; i++) {
			Human_colliders[i]->reset_force();
		}

		// �����Ă�����̂𗣂�
		Joint_base** joints[2] = {
			&catch_left_joint,
			&catch_right_joint
		};
		for (int i = 0; i < 2; i++) {
			Joint_base*& joint = *joints[i];
			if (joint != nullptr) {
				Joint::delete_joint(joint);
			}
		}

		respown_timer = 3;
	}

	// respowntimer��>0�Ȃ� respown��
	if (respown_timer > 0) {

		if (check_onplayer_coll->concoll_enter(Collider_tags::Stage)) {
			// respown�� stage�ɐڐG���Ă���� timer�����炷
			respown_timer -= Al_Global::second_per_frame;
		}
		else {
			// respown�� stage�ɐڐG���Ă��Ȃ���� drag��0
			for (int i = 0; i < Human_collider_size; i++) {
				Human_colliders[i]->physics_data.drag = 0.2f;
			}
		}

		return true;
	}

	{
		//joint�łȂ���Əd�͂��キ�Ȃ邩�� & ���̃p�[�c�͏d�͂̉e�����󂯂Ȃ��悤�ɂ��Ă��邩��  �������ɗ͂�������
		Head_collider->add_force(Vector3(0, -1, 0) * 150);
		Waist_collider->add_force(Vector3(0, -1, 0) * 150);
		Body_collider->add_force(Vector3(0, -1, 0) * 150);
		Lfoot_collider->add_force(Vector3(0, -1, 0) * 50);
		Rfoot_collider->add_force(Vector3(0, -1, 0) * 50);
	}
	// ���ł�CTR�����Ă���Ƃ��ɂ��ɂ���Ƃ�����
	if (input_changer->is_gunyatto_state) {
		//for (int i = 0; i < Human_collider_size; i++) {
		//	Human_colliders[i]->physics_data.drag = 0.2f;
		//}
		return true;
	}

	return false;
}

// player�̌����𒲐�
void Player::turn_gunyatto_dir() {
	//Vector3 dir = vector3_quatrotate(Vector3(0, 0, 1), Waist->transform->orientation);

	// ���̌���
	Vector3 dir = vector3_quatrotate(Vector3(0, 0, 1), Waist->transform->orientation);

	// �����瓪�ւ̌���(���̕����ɐ��ʂ������Ă�������)
	Vector3 waist_head_dir = (Head->world_position() - Waist->world_position()).unit_vect();

	// ���΂��� ������
	if (fabsf(dir.y) > 0.5f) {
		if (dir.y < 0) dir = +waist_head_dir;
		else           dir = -waist_head_dir;
	}
	// �����Ă���Ȃ牽��������return
	else return;

	// ���̌���
	Vector3 player_vec = vector3_quatrotate(Vector3(0, 0, 1), rotate);

	// ���������߂�
	float angle = vector3_angle(dir, player_vec);

	// ��]����0,1,0�ɂ��������� �O�ςŉ�]�����̊m�F
	if (vector3_cross(player_vec, dir).y < 0)angle *= -1;

	rotate *= quaternion_axis_angle(Vector3(0, 1, 0), angle);

}

// drag�������l�ɂ���
void Player::set_default_drag() {
	for (int i = 0; i < Human_collider_size; i++) {
		Human_colliders[i]->physics_data.drag = Human_default_drags[i];
	}
}

// "��������"joint���폜����
void Player::delete_catchjoint() {
	Joint_base** joints[2] = {
		&catch_left_joint,
		&catch_right_joint
	};
	for (int i = 0; i < 2; i++) {
		Joint_base*& joint = *joints[i];
		if (joint != nullptr) {
			Joint::delete_joint(joint);
		}
	}
}

// respown����
void Player::respown() {
	auto stage = stage_manager->get_current_stage();

	// ���W�ړ�
	Vector3 off = vector3_quatrotate(stage->player_respown_pos - Waist->world_position(), Waist->parent()->world_orientate().inverse());
	for (int i = 0; i < Human_gameobject_size; i++) {
		Human_gameobjects[i]->transform->local_pos += off;
	}

	// �͂�reset& �y�����ɑ��x��������
	for (int i = 0; i < Human_collider_size; i++) {
		Human_colliders[i]->reset_force();
		Human_colliders[i]->linear_velocity(Vector3(0, -1, 0)); //fleeze�Ɉ���������Ȃ��悤�Ɍy���͂�������
	}

	// �����Ă�����̂𗣂�
	Joint_base** joints[2] = {
		&catch_left_joint,
		&catch_right_joint
	};
	for (int i = 0; i < 2; i++) {
		Joint_base*& joint = *joints[i];
		if (joint != nullptr) {
			Joint::delete_joint(joint);
		}
	}

	for (int i = 0; i < Human_collider_size; ++i) {
		Human_colliders[i]->set_max_linear_velocity(FLT_MAX);
		Human_colliders[i]->set_max_angula_velocity(FLT_MAX);
	}

	respown_timer = 3;

}

// onground_collider���X�V
void Player::update_onground() {

	onground_collider = nullptr;

	float dot = vector3_dot(Vector3(0, -1, 0), vector3_quatrotate(Vector3(0, -1, 0), Waist_collider->transform->orientation));
	if (dot < 0)return; //�������������Ă��Ȃ����return
	dot = dot * dot;

	// sphere���΂��Ċm�F
	Ray ray;
	ray.direction = Vector3(0, -1, 0);
	ray.position = Waist_collider->transform->position;
	onground_ray_data.collider_tag = Collider_tags::Stage;
	if (ray.sphere_cast(onground_radius, onground_contactpoint, onground_ray_data) && onground_ray_data.raymin < onground_dis) onground_collider = onground_ray_data.coll;

}

void Player::update_gnyat_pow() {
	float gunyatto_pows[3] = { 1,1,1 };
	//gnyat_pow = 1;

	// �r�̏�񂩂�l���v�Z
	if (catch_left_joint != nullptr && catch_right_joint != nullptr &&
		onground_collider == nullptr &&
		is_jumping == false
		) {
		// ����ł���ł���&�ڒn���Ă��Ȃ�&jump��������ł͂Ȃ�
		hand_gunyatto_pow -= 5 * time->deltaTime(); //���ɂ���Ƃ����Ă���

	}
	else if (catch_left_joint != nullptr && catch_right_joint != nullptr) {
		// ����ł���ł���&�����������������Ă���ڒn���Ă���
		if (vector3_dot(Vector3(0, -1, 0), vector3_quatrotate(Vector3(0, -1, 0), Waist->transform->orientation)) > 0.7f)
			hand_gunyatto_pow += 1 * time->deltaTime(); //�������肳���Ă���
	}

	else if (catch_left_joint == nullptr || catch_right_joint == nullptr) {
		// �Е��ł�����ł��Ȃ��Ƃ�
		hand_gunyatto_pow = 1; //�������肷��
	}
	hand_gunyatto_pow = ALClamp(hand_gunyatto_pow, -1, 1);

	// ���A�r�A����onground���Ă��Ȃ��̂Ɉ��������邱�Ƃ����X���邽�� ����
	if (
		(Lfoot_collider->concoll_enter(Collider_tags::Stage) || Rfoot_collider->concoll_enter(Collider_tags::Stage)) &&
		onground_collider == nullptr
		)gunyatto_pows[0] = 0.1f;

	if (
		(Lelbow_collider->concoll_enter(Collider_tags::Stage) || Relbow_collider->concoll_enter(Collider_tags::Stage)) &&
		onground_collider == nullptr
		)gunyatto_pows[1] = 0.1f;

	//if (
	//	(Waist_collider->concoll_enter(Collider_tags::Stage)) &&
	//	onground_collider == nullptr
	//	)gunyatto_pows[2] = 0.1f;

	gunyatto_pow = ALClamp(hand_gunyatto_pow * ((gunyatto_pows[0] * gunyatto_pows[1] * gunyatto_pows[2])), 0, 1);
	//gunyatto_pow = 1;
	////���肪static�Ȃ��̂������Ă���Ƃ��A���ɂ����
	//if ((catch_left_joint != nullptr && catch_left_joint->get_colliderB()->tag & Collider_tags::Static_Stage) &&
	//	(catch_right_joint != nullptr && catch_right_joint->get_colliderB()->tag & Collider_tags::Static_Stage) &&
	//	(is_catch_start_yplus = true && vector3_quatrotate(Vector3(0, 0, 1), camera->camera_rot).y < 0)
	//	) {
	//	gnyat_pow = 0;
	//}
	//is_gunyatto = false;
	//if (gnyat_pow == 0)is_gunyatto = true;
}