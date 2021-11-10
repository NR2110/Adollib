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
		Mouse::LBUTTON,
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

	// stageが変更されたらgameobjectの削除でjointは消えているので jointを削除
	if (stage_manager->get_is_changed_stage()) {
		joints[0] = nullptr;
		joints[1] = nullptr;
		return;
	}

	for (int i = 0; i < 2; i++) {
		const Mouse key = keys[i];
		auto& collider = colliders[i];
		Joint_base*& joint = *joints[i];
		//持つ
		if (input->getMouseState(key) && joint == nullptr && *is_maked_joint[i] == false) { //Keyが押されている jointが存在しない 同state中に物をつかんでいない
			collider->is_save_contacted_colls = true;
			auto contacted_colls = collider->get_Contacted_data();

			//衝突しているcolliderから一番近いものを探査
			Contacted_data const* min_data = nullptr;
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

				joint->slop = 0.1f;
				//joint->slop = 10000000;

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

				if(
					(world_posA - world_posB).norm() > 3 * 3 ||
					(colliders[i]->transform->position - colliders[i + 2]->transform->position).norm() > 3 * 3
					){
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

//rayを飛ばして腰を立たせる
void Player::push_waist_for_stand() {
	waist_pillar->center = Vector3(0, 100, 0);

	float dot = vector3_dot(Vector3(0, -1, 0), vector3_quatrotate(Vector3(0, -1, 0), Waist_collider->transform->orientation));
	Debug::set("dot", dot);
	if (dot < 0)return; //腰が下を向いていなければreturn

	// rayを飛ばして
	Ray ray;
	ray.direction = Vector3(0, -1, 0);
	ray.position = Waist_collider->transform->position;

	// 距離によってaddforce
	constexpr float stand_dis = 2;
	constexpr float stand_pow = 7000;
	Ray::Raycast_struct data;
	data.collider_tag = Collider_tags::Stage;
	if (ray.ray_cast(data)) {
		if (data.raymin - stand_dis * dot < 0) {
			Vector3 force = Vector3(0, 1, 0) * (stand_dis * dot - data.raymin) * stand_pow;
			Waist_collider->add_force(force);
			data.coll->add_force_local(-force, ray.position + ray.direction * data.raymin);
		}
	};

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
		Waist_collider->physics_data.drag = 0.98f;
	}
	// 入力があれば動きやすいように0.1にする
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
			//腰をたたせる
			Quaternion off = rotate * Waist_collider->gameobject->transform->orientation.inverse();

			float rad = off.radian();
			if (rad > PI)rad = 2 * PI - rad;
			float pow = ALClamp(rad * waist_rot_pow, 0, waist_rot_max_pow);
			Waist_collider->add_torque(off.axis() * pow * gnyat_pow);

		}
		{
			//体をたたせる
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
		//Ray ray;
		//ray.direction = Vector3(0, -1, 0);
		//ray.position = Waist_collider->transform->position;
		//Ray::Raycast_struct data;
		//data.collider_tag = Collider_tags::Stage;
		//if (ray.ray_cast(data)) {
		//	data.coll->add_force_local(Vector3(0, -100, 0) * jump_power, ray.position + ray.direction * data.raymin);
		//}

		for (int i = 0; i < Human_collider_size; i++) {
			Human_colliders[i]->linear_velocity(Vector3(Human_colliders[i]->linear_velocity().x, jump_power, Human_colliders[i]->linear_velocity().z));
		}
		Lleg_collider->linear_velocity(Vector3(Lleg_collider->linear_velocity().x, jump_power * 0.1f, Lleg_collider->linear_velocity().z));
		Rleg_collider->linear_velocity(Vector3(Rleg_collider->linear_velocity().x, jump_power * 0.1f, Rleg_collider->linear_velocity().z));
		Lfoot_collider->linear_velocity(Vector3(Lfoot_collider->linear_velocity().x, jump_power * 0.1f, Lfoot_collider->linear_velocity().z));
		Rfoot_collider->linear_velocity(Vector3(Rfoot_collider->linear_velocity().x, jump_power * 0.1f, Rfoot_collider->linear_velocity().z));

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
		Vector3 off = vector3_quatrotate(stage->player_respown_pos - Waist->world_position(), Waist->parent()->world_orientate().inverse());
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
	Vector3 off = vector3_quatrotate(stage->player_respown_pos - Waist->world_position(), Waist->parent()->world_orientate().inverse());
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