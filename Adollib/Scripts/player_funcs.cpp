#include "player.h"

#include "../Adollib/Scripts/Main/Adollib.h"
#include "../Adollib/Scripts/Object/gameobject_manager.h"

#include "../Adollib/Scripts/Physics/ray.h"

#include "stage_manager.h"
#include "stage_base.h"

using namespace Adollib;

//è‚ğL‚Î‚·
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

			//ƒJƒƒ‰‚Ì‰ñ“]‚©‚ç˜r‚ğã‚°‚é‚‚³‚ğ•Ï‚¦‚é
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

			//quaternion‚ª-Q‚ÌA¶¬‚·‚é‰ñ“]‚ª‹t‰ñ“]‚É‚È‚é‚½‚ßlocalorint‚É - ‚ğ‚©‚¯‚é
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
				// Œ¨
				auto& collider = colliders[i];
				Quaternion off = collider->transform->orientation * goal.inverse();
				float rad = off.radian();
				if (rad > PI)rad = 2 * PI - rad;
				float pow = ALClamp(rad *  hand_rot_pow, 0, hand_rot_max_pow);
				collider->add_torque(off.axis() * pow * catch_obje_mass * collider->physics_data.inertial_mass);
				collider->set_max_angula_velocity(hand_rot_max_speed);
			}
			{
				//˜r
				auto& collider = colliders[i + 2];
				Quaternion off = collider->transform->orientation * goal.inverse();
				float rad = off.radian();
				if (rad > PI)rad = 2 * PI - rad;

				float pow = ALClamp(rad * hand_rot_pow, 0, hand_rot_max_pow);
				collider->add_torque(off.axis() * pow * catch_obje_mass * collider->physics_data.inertial_mass);
				collider->set_max_angula_velocity(hand_rot_max_speed * 0.8f);
			}
			//{
			//	//˜r
			//	auto& collider = colliders[i + 4];
			//	Quaternion off = collider->transform->orientation * goal.inverse();
			//	float pow = ALClamp(off.radian() * hand_rot_pow, 0, hand_rot_max_pow);
			//	collider->add_torque(off.axis() * pow * 3 * collider->physics_data.inertial_mass);
			//	collider->set_max_angula_velocity(hand_rot_max_speed);
			//}
		}
	}

};

//•¨‚ğ‚Â‚©‚Ş
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

	// stage‚ª•ÏX‚³‚ê‚½‚çgameobject‚Ìíœ‚Åjoint‚ÍÁ‚¦‚Ä‚¢‚é‚Ì‚Å joint‚ğíœ
	if (stage_manager->get_is_changed_stage()) {
		joints[0] = nullptr;
		joints[1] = nullptr;
		return;
	}

	for (int i = 0; i < 2; i++) {
		const Mouse key = keys[i];
		auto& collider = colliders[i];
		Joint_base*& joint = *joints[i];
		//‚Â
		if (input->getMouseState(key) && joint == nullptr && *is_maked_joint[i] == false) { //Key‚ª‰Ÿ‚³‚ê‚Ä‚¢‚é joint‚ª‘¶İ‚µ‚È‚¢ “¯state’†‚É•¨‚ğ‚Â‚©‚ñ‚Å‚¢‚È‚¢
			collider->is_save_contacted_colls = true;
			auto contacted_colls = collider->get_Contacted_data();

			//Õ“Ë‚µ‚Ä‚¢‚écollider‚©‚çˆê”Ô‹ß‚¢‚à‚Ì‚ğ’T¸
			Contacted_data const* min_data = nullptr;
			for (auto& c_coll : contacted_colls) {
				if (min_data == nullptr || min_data->penetrate > c_coll.penetrate) {
					min_data = &c_coll;

				}
			}
			//joint‚ğ¶¬
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

		//—£‚·
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

	//TODO : ‚Â‚©‚ñ‚Å‚¢‚é‚à‚Ì‚ÌStage tag‚ğÁ‚µ‚ÄAæ‚Á‚Ä‚¢‚é‚Æ‚«‚Éjump‚Å‚«‚È‚¢‚æ‚¤‚É‚µ‚Ä‚¢‚é
	for (int i = 0; i < 2; i++) {
		Joint_base*& joint = *joints[i];
		if (joint == nullptr)continue;
		joint->get_colliderB()->tag |= Collider_tags::Having_Stage;
		joint->get_colliderB()->tag &= ~Collider_tags::Jumpable_Stage;
	}
};

//ray‚ğ”ò‚Î‚µ‚Ä˜‚ğ—§‚½‚¹‚é
void Player::push_waist_for_stand() {
	waist_pillar->center = Vector3(0, 100, 0);

	float dot = vector3_dot(Vector3(0, -1, 0), vector3_quatrotate(Vector3(0, -1, 0), Waist_collider->transform->orientation));
	Debug::set("dot", dot);
	if (dot < 0)return; //˜‚ª‰º‚ğŒü‚¢‚Ä‚¢‚È‚¯‚ê‚Îreturn

	// ray‚ğ”ò‚Î‚µ‚Ä
	Ray ray;
	ray.direction = Vector3(0, -1, 0);
	ray.position = Waist_collider->transform->position;

	// ‹——£‚É‚æ‚Á‚Äaddforce
	constexpr float stand_dis = 2;
	constexpr float stand_pow = 7000;
	Ray::Raycast_struct data;
	data.collider_tag = Collider_tags::Stage;
	if (ray.ray_cast(data)) {
		if (data.raymin - stand_dis * dot < 0) {
			Waist_collider->add_force(Vector3(0, 1, 0) * (stand_dis * dot - data.raymin) * stand_pow);
		}
	};

}

//ˆÚ“®
void Player::linear_move() {
	const float gnyat_pow = 0.9f;
	if (!input->getKeyState(Key::LeftControl))
	{
		//ˆÚ“®
		Waist_collider->add_force(dir * waist_move_pow * gnyat_pow);

		// ‘¬“x§ŒÀ
		Vector3 speed = Waist_collider->linear_velocity();
		speed.y = 0;
		if (speed.norm() > waist_move_max_speed * waist_move_max_speed) {
			Vector3 Yspeed = Vector3(0, Waist_collider->linear_velocity().y, 0);
			Waist_collider->linear_velocity(speed.unit_vect() * waist_move_max_speed + Yspeed);
		}
	}
	// ’n–Ê‚ÉÚ‚µ‚Ä‚¢‚é & “ü—Í‚ª–³‚¢
	if (dir.norm() == 0 && onground_collider->concoll_enter(Collider_tags::Stage)) {
		Waist_collider->physics_data.drag = 0.8f;
	}
	// “ü—Í‚ª‚ ‚ê‚Î“®‚«‚â‚·‚¢‚æ‚¤‚É0.1‚É‚·‚é
	else Waist_collider->physics_data.drag = 0.1;
};

//‰ñ“]
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

//ˆÚ“®•ûŒü‚ğŒvZ
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

//—§‚Â‚æ‚¤‚É—Í‚ğ‰Á‚¦‚é
void Player::add_pow_for_stand() {

	float gnyat_pow = 0.9f;
	if (!input->getKeyState(Key::LeftControl) && is_gunyatto == false)
	{
		{
			//Šç‚ªÔ‚¿‚á‚ñ‚È‚Ì‚ğ¡‚·
			Head_collider->physics_data.anglar_drag = 1.f;
			Quaternion off = Body_collider->gameobject->transform->orientation * Head_collider->gameobject->transform->orientation.inverse();
			float rad = off.radian();
			if (rad > PI)rad = 2 * PI - rad;
			float pow = ALClamp(rad * head_rot_pow, 0, head_rot_max_pow);
			Head_collider->add_torque(off.axis() * pow * gnyat_pow);

		}
		{
			//“·‘Ì‚ğ‚½‚½‚¹‚é
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
			//“·‘Ì‚ğ‚½‚½‚¹‚é
			Quaternion off = rotate * Body_collider->gameobject->transform->orientation.inverse();

			float rad = off.radian();
			if (rad > PI)rad = 2 * PI - rad;
			float pow = ALClamp(rad * body_rot_pow, 0, body_rot_max_pow);
			Body_collider->add_torque(off.axis() * pow * gnyat_pow);
		}

	}
	// gunyatto‚µ‚Ä‚¢‚½‚Í
	else { turn_gunyatto_dir(); }

	{
		//joint‚Å‚Â‚È‚°‚é‚Æd—Í‚ªã‚­‚È‚é‚©‚ç & ˆê’è‚Ìƒp[ƒc‚Íd—Í‚Ì‰e‹¿‚ğó‚¯‚È‚¢‚æ‚¤‚É‚µ‚Ä‚¢‚é‚©‚ç  ‰ºŒü‚«‚É—Í‚ğ‰Á‚¦‚é
		Waist_collider->add_force(Vector3(0, -1, 0) * 150);
		Body_collider->add_force(Vector3(0, -1, 0) * 150);
		Lfoot_collider->add_force(Vector3(0, -1, 0) * 50);
		Rfoot_collider->add_force(Vector3(0, -1, 0) * 50);
	}

	is_gunyatto = false;
	//—¼è‚ªstatic‚È‚à‚Ì‚ğ‚Á‚Ä‚¢‚é‚Æ‚«A‚®‚É‚á‚Á‚Æ
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

//‘«‚ğ“®‚©‚·
void Player::move_legs() {
	if (input->getKeyState(Key::LeftControl))return;

	//Waist_collider->physics_data.is_moveable = false;
		//Rleg_collider->physics_data.is_moveable = false;
		//Lleg_collider->physics_data.is_moveable = false;
	if (dir.norm() != 0) {
		float front_value = ToRadian(30); //‘«‚Í“·‘Ì‚æ‚è’x‚ê‚é‚½‚ß ‘«‚ğ‘O‚Éo‚·‚½‚ß‚É—]•ª‚É‚©‚¯‚é‰ñ“]‚Ì—Í
		float max_theata = ToRadian(60) * 0.5f;
		float sin = sinf(move_timer * 1.5f * DirectX::XM_2PI); //timre‚©‚ç‘«‚Ì‰ñ“]—Ê‚ğ‹‚ß‚é
		//float sin = sinf(move_timer * DirectX::XM_2PI * 0.5f); //timre‚©‚ç‘«‚Ì‰ñ“]—Ê‚ğ‹‚ß‚é

		Vector3 waist_axis = Vector3(0, -1, 0);
		Vector3 rot_axis = Vector3(1, 0, 0);

		if (1) {
			//‚±‚ê‚ğ‚·‚ê‚Î‘«‚ğ‰¡‚É“®‚©‚¹‚é ‚Í‚¸‚¾‚ª‚¤‚Ü‚­‚¢‚©‚È‚¢
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

//jump‚³‚¹‚é
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

//respownˆ—
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
	// stagew’è‚ÌYÀ•W‚æ‚èPlayer‚ª’á‚¯‚ê‚Îrespown
	if (Waist->world_position().y < stage->y_player_respown_limit + 50) {

		// À•WˆÚ“®
		Vector3 off = vector3_quatrotate(stage->player_respown_pos - Waist->world_position(), Waist->parent()->world_orientate().inverse());
		for (int i = 0; i < Human_gameobject_size; i++) {
			Human_gameobjects[i]->transform->local_pos += off;
		}

		// —Í‚Ìreset
		for (int i = 0; i < Human_collider_size; i++) {
			Human_colliders[i]->reset_force();
		}

		// ‚Á‚Ä‚¢‚é‚à‚Ì‚ğ—£‚·
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

//player‚ÌŒü‚«‚ğ’²®
void Player::turn_gunyatto_dir() {
	//Vector3 dir = vector3_quatrotate(Vector3(0, 0, 1), Waist->transform->orientation);

	// ˜‚ÌŒü‚«
	Vector3 dir = vector3_quatrotate(Vector3(0, 0, 1), Waist->transform->orientation);

	// ˜‚©‚ç“ª‚Ö‚ÌŒü‚«(‚±‚Ì•ûŒü‚É³–Ê‚ğ‚Á‚Ä‚¢‚«‚½‚¢)
	Vector3 waist_head_dir = (Head->world_position() - Waist->world_position()).unit_vect();

	// • ‚Î‚¢‚© ‹ÂŒü‚¯‚©
	if (fabsf(dir.y) > 0.5f) {
		if (dir.y < 0) dir = +waist_head_dir;
		else           dir = -waist_head_dir;
	}
	// —§‚Á‚Ä‚¢‚é‚È‚ç‰½‚à‚¹‚¸‚Éreturn
	else return;

	// ¡‚ÌŒü‚«
	Vector3 player_vec = vector3_quatrotate(Vector3(0, 0, 1), rotate);

	// ·•ª‚ğ‹‚ß‚Ä
	float angle = vector3_angle(dir, player_vec);

	// ‰ñ“]²‚Í0,1,0‚É‚µ‚½‚¢‚©‚ç ŠOÏ‚Å‰ñ“]•ûŒü‚ÌŠm”F
	if (vector3_cross(player_vec, dir).y < 0)angle *= -1;

	rotate *= quaternion_axis_angle(Vector3(0, 1, 0), angle);

}


//"•¨‚ğ‚Â"joint‚ğíœ‚·‚é
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

//respownˆ—
void Player::respown() {
	auto stage = stage_manager->get_current_stage();

	// À•WˆÚ“®
	Vector3 off = vector3_quatrotate(stage->player_respown_pos - Waist->world_position(), Waist->parent()->world_orientate().inverse());
	for (int i = 0; i < Human_gameobject_size; i++) {
		Human_gameobjects[i]->transform->local_pos += off;
	}

	// —Í‚Ìreset
	for (int i = 0; i < Human_collider_size; i++) {
		Human_colliders[i]->reset_force();
	}

	// ‚Á‚Ä‚¢‚é‚à‚Ì‚ğ—£‚·
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