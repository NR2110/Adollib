#include "player.h"

#include "../Adollib/Scripts/Main/Adollib.h"
#include "../Adollib/Scripts/Object/gameobject_manager.h"

#include "../Adollib/Scripts/Physics/ray.h"

#include "stage_manager.h"
#include "stage_base.h"

using namespace Adollib;

//手を伸ばす
void Player::reach_out_hands() {

	if (input->getKeyState(Key::LeftControl))return;

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

			//カメラの回転から腕を上げる高さを変える
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

			//quaternionが-Qの時、生成する回転が逆回転になるためlocalorintに - をかける
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

			float catch_obje_mass = 1;
			{
				// 肩
				auto& collider = colliders[i];
				Quaternion off = collider->transform->orientation * goal.inverse();
				float rad = off.radian();
				if (rad > PI)rad = 2 * PI - rad;
				float pow = ALClamp(rad *  hand_rot_pow, 0, hand_rot_max_pow);
				collider->add_torque(off.axis() * pow * catch_obje_mass * collider->physics_data.inertial_mass);
				collider->set_max_angula_velocity(hand_rot_max_speed);
			}
			{
				//腕
				auto& collider = colliders[i + 2];
				Quaternion off = collider->transform->orientation * goal.inverse();
				float rad = off.radian();
				if (rad > PI)rad = 2 * PI - rad;

				float pow = ALClamp(rad * hand_rot_pow, 0, hand_rot_max_pow);
				collider->add_torque(off.axis() * pow * catch_obje_mass * collider->physics_data.inertial_mass);
				collider->set_max_angula_velocity(hand_rot_max_speed * 0.8f);
			}
			//{
			//	//腕
			//	auto& collider = colliders[i + 4];
			//	Quaternion off = collider->transform->orientation * goal.inverse();
			//	float pow = ALClamp(off.radian() * hand_rot_pow, 0, hand_rot_max_pow);
			//	collider->add_torque(off.axis() * pow * 3 * collider->physics_data.inertial_mass);
			//	collider->set_max_angula_velocity(hand_rot_max_speed);
			//}
		}
	}

};

//物をつかむ
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
	bool* is_maked_joint[2] = {
		&is_maked_left_joint,
		&is_maked_right_joint
	};

	for (int i = 0; i < 2; i++) {
		const Mouse key = keys[i];
		auto& collider = colliders[i];
		Joint_base*& joint = *joints[i];
		//持つ
		if (input->getMouseState(key) && joint == nullptr && *is_maked_joint[i] == false) { //Keyが押されている jointが存在しない 同state中に物をつかんでいない
			collider->is_save_contacted_colls = true;
			auto& contacted_colls = collider->contacted_colliders;

			//衝突しているcolliderから一番近いものを探査
			Contacted_data* min_data = nullptr;
			for (auto& c_coll : contacted_colls) {
				if (min_data == nullptr || min_data->penetrate > c_coll.penetrate) {
					min_data = &c_coll;

				}
			}
			//jointを生成
			if (min_data != nullptr && min_data->coll->tag != Collider_tags::Human) {
				*is_maked_joint[i] = true;

				joint = Joint::add_balljoint(
					collider,
					min_data->coll,
					min_data->contacted_pointA,
					min_data->contacted_pointB
				);

				//elbowにjointを作る場合
				/*
				auto& elbow_collider = colliders[i + 2];

				Vector3 contacted_pointA_world = vector3_quatrotate(min_data->contacted_pointA, collider->transform->orientation) + collider->transform->position;
				Vector3 contacted_pointA_elbowcoord = vector3_quatrotate(contacted_pointA_world - elbow_collider->transform->position, elbow_collider->transform->orientation.inverse());

				Vector3 contacted_pointB_world = vector3_quatrotate(min_data->contacted_pointB, min_data->coll->transform->orientation) + min_data->coll->transform->position;
				Vector3 contacted_pointB_elbowcoord = vector3_quatrotate(contacted_pointA_world - elbow_collider->transform->position, elbow_collider->transform->orientation.inverse());

				Vector3 elbow0_point_world = elbow_collider->transform->position;
				Vector3 elbow0_point_collBcoord = vector3_quatrotate(elbow0_point_world - min_data->coll->transform->position, min_data->coll->transform->orientation.inverse());

				joint = Joint::add_balljoint(
					elbow_collider,
					min_data->coll,
					Vector3(0,0,0),
					elbow0_point_collBcoord
				);*/

			}
		}

		//離す
		{

			if (!input->getMouseState(key)) {
				*is_maked_joint[i] = false;
			}

			if (!input->getMouseState(key) && joint != nullptr) {
				joint->get_colliderB()->tag &= ~Collider_tags::Having_Stage;
				joint->get_colliderB()->tag |= Collider_tags::Jumpable_Stage;
				Joint::delete_joint(joint);
			}


			if (joint != nullptr) {
				joint->adapt_anchor();
				Vector3 world_posA = joint->get_colliderA()->transform->position + vector3_quatrotate(joint->get_anchors()[0].posA, joint->get_colliderA()->transform->orientation);
				Vector3 world_posB = joint->get_colliderB()->transform->position + vector3_quatrotate(joint->get_anchors()[0].posB, joint->get_colliderB()->transform->orientation);

				if((world_posA - world_posB).norm() > 3 * 3){
					joint->get_colliderB()->tag &= ~Collider_tags::Having_Stage;
					joint->get_colliderB()->tag |= Collider_tags::Jumpable_Stage;
					Joint::delete_joint(joint);
				}

			}

		}

	}

	//TODO : つかんでいるもののStage tagを消して、乗っているときにjumpできないようにしている
	for (int i = 0; i < 2; i++) {
		Joint_base*& joint = *joints[i];
		if (joint == nullptr)continue;
		joint->get_colliderB()->tag |= Collider_tags::Having_Stage;
		joint->get_colliderB()->tag &= ~Collider_tags::Jumpable_Stage;
	}
};

//腰を支えるwaist_pillarを調整
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

	//waist_sphereのy方向の調整
	float center_y = waist_pillar->center.y;
	{
		float anchor_move_vec = 0;
		if (!check_standable_collider->concoll_enter(Collider_tags::Stage) && is_jumping == false) {
			// しばらく地面にいなければ徐々にwaist_Sphereを上にあげる
			anchor_move_vec = -0.2f;
		}
		else anchor_move_vec = 3; // 接地していればwaist_Sphereを下げる

		//両手がstaticなものを持っているとき、
		if ((catch_left_joint != nullptr  && catch_left_joint ->get_colliderB()->tag & Collider_tags::Static_Stage) &&
			(catch_right_joint != nullptr && catch_right_joint->get_colliderB()->tag & Collider_tags::Static_Stage)
			) {
			// timerを設定する
			check_standable_collider_timer = 0.5f;
		}

		{
			check_standable_collider_timer -= Al_Global::second_per_frame;
			// timerが0より大きい時 waist_Sphereを上にあげる
			if (check_standable_collider_timer > 0)anchor_move_vec = -1;
		}

		center_y += -1 * anchor_move_vec * Al_Global::second_per_frame;
		center_y = ALClamp(center_y, Waist_pillar_max_y, -1.0f);
	}

	Vector3 center_xz = waist_pillar->center;
	{
		center_xz.y = 0;
		center_xz += vector3_quatrotate(-dir, Waist->transform->orientation) * 0.01f;

		if (center_xz.norm() > Waist_pillar_max_xz * Waist_pillar_max_xz)center_xz = center_xz.unit_vect() * Waist_pillar_max_xz;
	}

	//waist_pillar->center = Vector3(center_xz.x, center_y, center_xz.z);
	waist_pillar->center = Vector3(0, center_y, -0.3f);
	//waist_pillar->center = Vector3(0, center_y, -0);
	//waist_pillar->center = vector3_quatrotate(-dir, Waist->transform->orientation) +Vector3(0, waist_pillar->center.y,0);

	//if()

}

//移動
void Player::linear_move() {
	const float gnyat_pow = 0.9f;
	if (!input->getKeyState(Key::LeftControl))
	{
		//移動
		Waist_collider->add_force(dir * waist_move_pow * gnyat_pow);

		// 速度制限
		Vector3 speed = Waist_collider->linear_velocity();
		speed.y = 0;
		if (speed.norm() > waist_move_max_speed * waist_move_max_speed) {
			Vector3 Yspeed = Vector3(0, Waist_collider->linear_velocity().y, 0);
			Waist_collider->linear_velocity(speed.unit_vect() * waist_move_max_speed + Yspeed);
		}
	}
	// 地面に接している & 入力が無い時
	if (dir.norm() == 0 && onground_collider->concoll_enter(Collider_tags::Stage)) {
		Waist_collider->physics_data.drag = 0.8f;
	}
	else Waist_collider->physics_data.drag = 0.1;
};

//回転
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

		rot_vec = vector3_quatrotate(Vector3(0, 0, -1), camera->orientation);
		rot_vec.y = 0;
		rot_vec = rot_vec.unit_vect();

		Vector3 player_vec = vector3_quatrotate(Vector3(0, 0, 1), rotate);

		float angle = vector3_angle(rot_vec, player_vec);
		if (angle > turn_speed)angle = turn_speed;

		if (vector3_cross(player_vec, rot_vec).y < 0)angle *= -1;

		rotate *= quaternion_axis_angle(Vector3(0, 1, 0), angle * Al_Global::second_per_frame * 100);

		debug_coll->transform->local_pos = Waist_collider->transform->position + vector3_quatrotate(Vector3(0, 0, 1), rotate);
	}

};

//移動方向を計算
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
	//dir = vector3_quatrotate(dir, quaternion_from_euler(0, 180, 0));
	dir = dir.unit_vect();

	//debug_coll->transform->local_pos = Waist->world_position() + dir * 5;
};

//立つように力を加える
void Player::add_pow_for_stand() {

	float gnyat_pow = 0.9f;
	if (!input->getKeyState(Key::LeftControl) && is_gunyatto == false)
	{
		{
			//顔が赤ちゃんなのを治す
			Head_collider->physics_data.anglar_drag = 1.f;
			Quaternion off = Body_collider->gameobject->transform->orientation * Head_collider->gameobject->transform->orientation.inverse();
			float rad = off.radian();
			if (rad > PI)rad = 2 * PI - rad;
			float pow = ALClamp(rad * head_rot_pow, 0, head_rot_max_pow);
			Head_collider->add_torque(off.axis() * pow * gnyat_pow);

		}
		{
			//胴体をたたせる
			Quaternion off = rotate * Waist_collider->gameobject->transform->orientation.inverse();

			//float k = 1;
			//float t = inv60;
			//float omega = sqrtf(k / Waist_collider->physics_data.inertial_mass);
			//float next = off.radian() * k * cosf(t * omega);
			//float pow = k * (next * t + 0.5f * t * (off.radian() - next));
			//pow = ALClamp(pow * waist_rot_pow, 0, waist_rot_max_pow);

			float rad = off.radian();
			if (rad > PI)rad = 2 * PI - rad;
			float pow = ALClamp(rad * waist_rot_pow, 0, waist_rot_max_pow);
			Waist_collider->add_torque(off.axis() * pow * gnyat_pow);

		}
		{
			//胴体をたたせる
			Quaternion off = rotate * Body_collider->gameobject->transform->orientation.inverse();

			float rad = off.radian();
			if (rad > PI)rad = 2 * PI - rad;
			float pow = ALClamp(rad * body_rot_pow, 0, body_rot_max_pow);
			Body_collider->add_torque(off.axis() * pow * gnyat_pow);
		}

	}
	// gunyattoしていた時は
	else { turn_gunyatto_dir(); }

	{
		//jointでつなげると重力が弱くなるから & 一定のパーツは重力の影響を受けないようにしているから  下向きに力を加える
		Waist_collider->add_force(Vector3(0, -1, 0) * 150);
		Body_collider->add_force(Vector3(0, -1, 0) * 150);
		Lfoot_collider->add_force(Vector3(0, -1, 0) * 50);
		Rfoot_collider->add_force(Vector3(0, -1, 0) * 50);
	}

	is_gunyatto = false;
	//両手がstaticなものを持っているとき、ぐにゃっと
	if ((catch_left_joint  != nullptr && catch_left_joint ->get_colliderB()->tag & Collider_tags::Static_Stage) &&
		(catch_right_joint != nullptr && catch_right_joint->get_colliderB()->tag & Collider_tags::Static_Stage)
		) {
		is_gunyatto = true;
	}

	if (!onground_collider->concoll_enter(Collider_tags::Jumpable_Stage)) {
		Waist_collider->physics_data.dynamic_friction = 0;
		Body_collider->physics_data.dynamic_friction = 0;

	}
	else{
		Waist_collider->physics_data.dynamic_friction = 0.4f;
		Body_collider->physics_data.dynamic_friction =  0.4f;

	}
};

//足を動かす
void Player::move_legs() {
	if (input->getKeyState(Key::LeftControl))return;

	//Waist_collider->physics_data.is_moveable = false;
		//Rleg_collider->physics_data.is_moveable = false;
		//Lleg_collider->physics_data.is_moveable = false;
	if (dir.norm() != 0) {
		float front_value = ToRadian(30); //足は胴体より遅れるため 足を前に出すために余分にかける回転の力
		float max_theata = ToRadian(60) * 0.5f;
		float sin = sinf(move_timer * 1.5f * DirectX::XM_2PI); //timreから足の回転量を求める
		//float sin = sinf(move_timer * DirectX::XM_2PI * 0.5f); //timreから足の回転量を求める

		Vector3 waist_axis = Vector3(0, -1, 0);
		Vector3 rot_axis = Vector3(1, 0, 0);

		if (1) {
			//これをすれば足を横に動かせる はずだがうまくいかない
			rot_axis = vector3_cross(dir, -waist_axis).unit_vect();
			rot_axis = vector3_quatrotate(rot_axis, Waist_collider->transform->orientation.inverse());
		}




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

		for (int i = 0; i < 2; i++) {

			auto axis = off[i].axis();

			float rad = off[i].radian();
			if (rad > PI)rad = 2 * PI - rad;
			float pow = ALClamp(rad * leg_rot_pow, 0, leg_rot_max_pow);

			collider[i]->add_torque(axis * pow);
			collider[i]->set_max_angula_velocity(leg_rot_max_speed);
		}

		move_timer += Al_Global::second_per_frame;
	}
};

//jumpさせる
void Player::make_jump() {
	if (is_jumping == true)coyote += Al_Global::second_per_frame;
	if (coyote >= 0)is_jumping = false;
	if (is_jumping == false && onground_collider->concoll_enter(Collider_tags::Jumpable_Stage)) coyote = 0.3f;
	if (is_jumping == false && !onground_collider->concoll_enter(Collider_tags::Jumpable_Stage)) coyote -= Al_Global::second_per_frame;

	if (is_gunyatto == false && coyote >= 0 && input->getKeyTrigger(Key::Space)) {
		for (int i = 0; i < Human_collider_size; i++) {
			Human_colliders[i]->linear_velocity(Vector3(Human_colliders[i]->linear_velocity().x, jump_power, Human_colliders[i]->linear_velocity().z));
		}
		//Lsholder_collider->linear_velocity(Vector3(Lleg_collider->linear_velocity().x, jump_power * 2, Lleg_collider->linear_velocity().z));
		//Rsholder_collider->linear_velocity(Vector3(Lleg_collider->linear_velocity().x, jump_power * 2, Lleg_collider->linear_velocity().z));
		//Lelbow_collider->linear_velocity(Vector3(Lleg_collider->linear_velocity().x, jump_power * 2, Lleg_collider->linear_velocity().z));
		//Relbow_collider->linear_velocity(Vector3(Lleg_collider->linear_velocity().x, jump_power * 2, Lleg_collider->linear_velocity().z));
		//Lhand_collider->linear_velocity(Vector3(Lleg_collider->linear_velocity().x, jump_power * 2, Lleg_collider->linear_velocity().z));
		//Rhand_collider->linear_velocity(Vector3(Lleg_collider->linear_velocity().x, jump_power * 2, Lleg_collider->linear_velocity().z));

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

//respown処理
bool Player::check_respown() {

	if (respown_timer > 0) {
		if(Waist_collider->concoll_enter(Collider_tags::Stage))respown_timer -= Al_Global::second_per_frame;
		else if(Head_collider->concoll_enter(Collider_tags::Stage))respown_timer -= Al_Global::second_per_frame * 0.5f;
	}
	if (respown_timer > 0) {
		turn_gunyatto_dir();
		return true;
	}

	auto stage = stage_manager->get_current_stage();
	// stage指定のY座標よりPlayerが低ければrespown
	if (Waist->world_position().y < stage->y_player_respown_limit + 50) {

		// 座標移動
		Vector3 off = vector3_quatrotate(stage->player_respown_pos - Waist->world_position(), Waist->pearent()->world_orientate().inverse());
		for (int i = 0; i < Human_gameobject_size; i++) {
			Human_gameobjects[i]->transform->local_pos += off;
		}

		// 力のreset
		for (int i = 0; i < Human_collider_size; i++) {
			Human_colliders[i]->reset_force();
		}

		// 持っているものを離す
		Joint_base** joints[2] = {
			&catch_left_joint,
			&catch_right_joint
		};
		for (int i = 0; i < 2; i++) {
			Joint_base*& joint = *joints[i];
			if (joint != nullptr) {
				joint->get_colliderB()->tag &= ~Collider_tags::Having_Stage;
				joint->get_colliderB()->tag |= Collider_tags::Jumpable_Stage;
				Joint::delete_joint(joint);
			}
		}

		respown_timer = 1;

		return true;
	}

	return false;
}

//playerの向きを調整
void Player::turn_gunyatto_dir() {
	//Vector3 dir = vector3_quatrotate(Vector3(0, 0, 1), Waist->transform->orientation);

	// 腰の向き
	Vector3 dir = vector3_quatrotate(Vector3(0, 0, 1), Waist->transform->orientation);

	// 腰から頭への向き(この方向に正面を持っていきたい)
	Vector3 waist_head_dir = (Head->world_position() - Waist->world_position()).unit_vect();

	// 腹ばいか 仰向けか
	if (fabsf(dir.y) > 0.5f) {
		if (dir.y < 0) dir = +waist_head_dir;
		else           dir = -waist_head_dir;
	}
	// 立っているなら何もせずにreturn
	else return;

	// 今の向き
	Vector3 player_vec = vector3_quatrotate(Vector3(0, 0, 1), rotate);

	// 差分を求めて
	float angle = vector3_angle(dir, player_vec);

	// 回転軸は0,1,0にしたいから 外積で回転方向の確認
	if (vector3_cross(player_vec, dir).y < 0)angle *= -1;

	rotate *= quaternion_axis_angle(Vector3(0, 1, 0), angle);

}


//"物を持つ"jointを削除する
void Player::delete_catchjoint() {
	Joint_base** joints[2] = {
		&catch_left_joint,
		&catch_right_joint
	};
	for (int i = 0; i < 2; i++) {
		Joint_base*& joint = *joints[i];
		if (joint != nullptr) {
			joint->get_colliderB()->tag &= ~Collider_tags::Having_Stage;
			joint->get_colliderB()->tag |= Collider_tags::Jumpable_Stage;
			Joint::delete_joint(joint);
		}
	}
}

//respown処理
void Player::respown() {
	auto stage = stage_manager->get_current_stage();

	// 座標移動
	Vector3 off = vector3_quatrotate(stage->player_respown_pos - Waist->world_position(), Waist->pearent()->world_orientate().inverse());
	for (int i = 0; i < Human_gameobject_size; i++) {
		Human_gameobjects[i]->transform->local_pos += off;
	}

	// 力のreset
	for (int i = 0; i < Human_collider_size; i++) {
		Human_colliders[i]->reset_force();
	}

	// 持っているものを離す
	Joint_base** joints[2] = {
		&catch_left_joint,
		&catch_right_joint
	};
	for (int i = 0; i < 2; i++) {
		Joint_base*& joint = *joints[i];
		if (joint != nullptr) {
			joint->get_colliderB()->tag &= ~Collider_tags::Having_Stage;
			joint->get_colliderB()->tag |= Collider_tags::Jumpable_Stage;
			Joint::delete_joint(joint);
		}
	}
	respown_timer = 1;

}