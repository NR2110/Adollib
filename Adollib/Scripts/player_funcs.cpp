#include "player.h"

#include "../Adollib/Scripts/Main/Adollib.h"
#include "../Adollib/Scripts/Object/gameobject_manager.h"

#include "../Adollib/Scripts/Physics/ALP__physics_manager.h"
#include "../Adollib/Scripts/Physics/ray.h"
#include "../Adollib/Scripts/Physics/collider_rope.h"
#include "../Adollib/Scripts/Imgui/work_meter.h"

#include "../Adollib/Scripts/Renderer/material_manager.h"
#include "../Adollib/Scripts/Renderer/rope_renderer.h"

#include "stage_manager.h"
#include "stage_base.h"
#include "camera.h"
#include "input_changer.h"



using namespace Adollib;

// 手を伸ばす
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

			//カメラの回転から腕を上げる高さを変える
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

			//{
			//	if (rad > PI) {
			//		rad = rad - 2 * PI;
			//		collider->gameobject->transform->local_orient *= -1;
			//		collider->gameobject->transform->orientation *= -1;
			//		off = rotate * collider->gameobject->transform->orientation.inverse();
			//	}
			//}

			float elbow_pow = 0;
			//if (fabsf(colliders[i]->transform->orientation.z * colliders[i + 2]->transform->orientation.z) > 0.2f) {
			//	colliders[i + 2]->transform->orientation *= -1;
			//	colliders[i + 2]->transform->local_orient *= -1;
			//}
			{
				//腕
				auto& collider = colliders[i + 2];
				Quaternion off = -collider->transform->orientation * goal.inverse();
				Vector3 axis = -off.axis();
				float rad = off.radian();
				Debug::set("rad_sholder_" + std::to_string(i), rad);
				if (rad > PI) {
					collider->transform->orientation *= -1;
					collider->transform->local_orient *= -1;

					off = -collider->transform->orientation * goal.inverse();
					axis = -off.axis();
					rad = off.radian();
				}
				Debug::set("elbow_orientation_" + std::to_string(i), collider->transform->orientation);

				float pow = ALClamp(rad * hand_rot_pow, 0, hand_rot_max_pow);
				Vector3 world_axis = vector3_quatrotate(axis, collider->transform->orientation);
				collider->add_torque(world_axis * pow * collider->physics_data.inertial_mass);
				collider->set_max_angula_velocity(ALmin(hand_rot_max_speed, hand_rot_max_speed * ToAngle(rad) * 0.03f));
			}

			{
				// 肩
				auto& collider = colliders[i];
				Quaternion off = -collider->transform->orientation * goal.inverse();
				Vector3 axis = -off.axis();
				float rad = off.radian();
				Debug::set("rad_sholder_" + std::to_string(i) + std::to_string(i), rad);
				if (rad > PI) {
					collider->transform->orientation *= -1;
					collider->transform->local_orient *= -1;

					off = -collider->transform->orientation * goal.inverse();
					axis = -off.axis();
					rad = off.radian();
				}
				Debug::set("sholder_orientation_" + std::to_string(i), collider->transform->orientation);

				float pow = ALClamp(rad * hand_rot_pow, 0, hand_rot_max_pow);
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

// 物をつかむ
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

	// stageが変更されたらgameobjectの削除でjointは消えているので jointを削除
	if (stage_manager->get_is_changed_stage()) {
		joints[0] = nullptr;
		joints[1] = nullptr;
		return;
	}

	for (int i = 0; i < 2; i++) {
		auto& collider = colliders[i];
		Joint_base*& joint = *joints[i];
		//持つ
		if (input_arm[i] && joint == nullptr && *is_maked_joint[i] == false) { //Keyが押されている jointが存在しない 同state中に物をつかんでいない
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
				joint->shrink_bias = 0.5f;
				joint->stretch_bias = 0.5f;
			}
		}

		//離す
		{
			// 何かつかんでいたらflagをfalseに(距離ではがれたとき、死んだときに押しっぱなしでもつかまないように)
			if (!input_arm[i]) {
				*is_maked_joint[i] = false;
			}

			// ボタンが押されていなくて、何かつかんでいたら
			if (!input_arm[i] && joint != nullptr) {
				Joint::delete_joint(joint);
			}

			// 距離制限
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

		// つかんでいるときhandのjointの位置を伸ばす
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

// ropeを撃つ
void Player::shot_rope() {
	if (rope_hit_sphere == nullptr) {
		rope_hit_sphere = Gameobject_manager::createSphere("rope_hit_sphere");
		rope_hit_sphere->transform->local_scale = Vector3(rope_sphere_r * 2);
		rope_hit_sphere->renderer->color = Vector4(1, 0, 0, 1);

		rope_hit_cylinder = Gameobject_manager::createCylinder("rope_hit_cylinder");
		rope_hit_cylinder->transform->local_scale = Vector3(rope_sphere_r * 0.5f);
		rope_hit_cylinder->renderer->color = Vector4(1, 0, 0, 1);
	}

	{
		rope_hit_sphere->transform->local_pos = Vector3(10000, 10000, 10000000);
		rope_hit_cylinder->transform->local_pos = Vector3(10000, 10000, 10000000);
	}

	dither_timer = ALClamp(dither_timer, -0.3f, 1);
	if (input_changer->is_rope_state) {
		{
			dither_timer += time->deltaTime() * 2.0f;
			float dither_pow = ALClamp(dither_timer, 0, 1);

			//Waist->renderer->get_material()->constant_buffer_data[0] = Vector4(500, 1000, 0, 0);
			Material_manager::find_material("player_material_01")->constant_buffer_data[0] = Vector4(5 + (500 - 5) * dither_pow, 4 + (1000 - 4) * dither_pow, 0, 0);
			Material_manager::find_material("player_material_02")->constant_buffer_data[0] = Vector4(5 + (500 - 5) * dither_pow, 4 + (1000 - 4) * dither_pow, 0, 0);
			camera->gameobject->findComponent<Camera_component>()->fov = 60 + (40 - 60) * dither_pow;
		}

		Ray camera_ray;
		camera_ray.direction = vector3_quatrotate(Vector3(0, 0, 1), camera->transform->orientation);
		camera_ray.position = camera->transform->position;

		Ray::Raycast_struct camera_ray_data;
		camera_ray_data.collider_tag = Collider_tags::Stage;
		Vector3 contact_point;

		if (camera_ray.sphere_cast(rope_sphere_r, contact_point, camera_ray_data)) {
			camera_ray_data.raymin += rope_sphere_r;
			rope_hit_sphere->transform->local_pos = camera_ray.position + camera_ray.direction * camera_ray_data.raymin;

			Ray arm_ray;
			arm_ray.direction = (rope_hit_sphere->transform->local_pos - Lhand->transform->position).unit_vect();
			arm_ray.position = Lhand->transform->position;

			Ray::Raycast_struct arm_ray_data;
			arm_ray_data.collider_tag = Collider_tags::Stage;

			if (arm_ray.sphere_cast(rope_sphere_r, contact_point, arm_ray_data)) {
				arm_ray_data.raymin += rope_sphere_r;
				rope_hit_sphere->transform->local_pos = arm_ray.position + arm_ray.direction * arm_ray_data.raymin;

				rope_hit_cylinder->transform->local_pos = arm_ray.position + arm_ray.direction * arm_ray_data.raymin * 0.5f;
				rope_hit_cylinder->transform->local_orient = quaternion_from_to_rotate(Vector3(0, 1, 0), arm_ray.direction);
				rope_hit_cylinder->transform->local_scale.y = arm_ray_data.raymin * 0.5f;

				if (arm_ray_data.raymin > 90) {
					rope_hit_cylinder->renderer->color = Vector4(1, 0, 0, 1);
					rope_hit_sphere->renderer->color = Vector4(1, 0, 0, 1);
				}
				else {
					rope_hit_cylinder->renderer->color = Vector4(0, 1, 0, 1);
					rope_hit_sphere->renderer->color = Vector4(0, 1, 0, 1);
				}

			}
		}
	}
	else {
		dither_timer -= time->deltaTime() * 1.5f;
		float dither_pow = ALClamp(dither_timer, 0, 1);

		//Waist->renderer->get_material()->constant_buffer_data[0] = Vector4(500, 1000, 0, 0);
		Material_manager::find_material("player_material_01")->constant_buffer_data[0] = Vector4(5 + (500 - 5) * dither_pow, 4 + (1000 - 4) * dither_pow, 0, 0);
		Material_manager::find_material("player_material_02")->constant_buffer_data[0] = Vector4(5 + (500 - 5) * dither_pow, 4 + (1000 - 4) * dither_pow, 0, 0);
		camera->gameobject->findComponent<Camera_component>()->fov = 60 + (40 - 60) * dither_pow;

	}

	if (input_changer->is_rope_releaced) {
		collider_num = 1;
		dither_timer = ALmin(1, dither_timer);

		//Ray ray;
		//ray.direction = vector3_quatrotate(Vector3(0, -1, 0), Lelbow->transform->orientation);
		//ray.position = Lhand->transform->position;

		//Ray::Raycast_struct data;
		//data.collider_tag = Collider_tags::Stage;
		//Vector3 contact_point;
		//ray.sphere_cast(rope_sphere_r, contact_point, data);
		//data.raymin += rope_sphere_r;

		Ray camera_ray;
		camera_ray.direction = vector3_quatrotate(Vector3(0, 0, 1), camera->transform->orientation);
		camera_ray.position = camera->transform->position;

		Ray::Raycast_struct camera_ray_data;
		camera_ray_data.collider_tag = Collider_tags::Stage;
		Vector3 contact_point;

		if (!camera_ray.sphere_cast(rope_sphere_r, contact_point, camera_ray_data))return;
		camera_ray_data.raymin += rope_sphere_r;
		rope_hit_sphere->transform->local_pos = camera_ray.position + camera_ray.direction * camera_ray_data.raymin;

		Ray arm_ray;
		arm_ray.direction = (rope_hit_sphere->transform->local_pos - Lhand->transform->position).unit_vect();
		arm_ray.position = Lhand->transform->position;

		Ray::Raycast_struct arm_ray_data;
		arm_ray_data.collider_tag = Collider_tags::Stage;

		if (!arm_ray.sphere_cast(rope_sphere_r, contact_point, arm_ray_data))return;
		arm_ray_data.raymin += rope_sphere_r;
		rope_hit_sphere->transform->local_pos = arm_ray.position + arm_ray.direction * arm_ray_data.raymin;


		if (arm_ray_data.raymin > 90)return;
		if (Lrope_go)Gameobject_manager::deleteGameobject(Lrope_go);
		if (Lblock_hand_joint)Joint::delete_joint(Lblock_hand_joint);

		Lrope_go = Gameobject_manager::create("rope");
		Lrope_go->transform->local_pos = arm_ray.position;

		Lrope_coll = Lrope_go->addComponent<Collider_Rope>();

		auto& coll = Lrope_coll;

		coll->sphere_size_r = rope_sphere_r;
		coll->sphree_offset_size = coll->sphere_size_r * 3;
		int sphere_count = arm_ray_data.raymin / coll->sphree_offset_size;
		sphere_count += 1;
		coll->sphere_num_size = sphere_count;
		coll->start_rope_dir = arm_ray.direction;
		coll->ignore_tags |= Collider_tags::Human;
		coll->default_physics_data.inertial_mass = 0.4f;
		coll->create_rope();

		for (int i = 0; i < coll->get_collider_size(); ++i) {
			auto physics_data = coll->get_vertex_data(i);
			physics_data.inertial_mass = 0.4f * (1 + (float)i / coll->get_collider_size());
			coll->set_vertex_data(i, physics_data);
		}

		Joint::add_balljoint(
			Lhand_collider, coll->get_collider(0),
			Vector3(0), Vector3(0)
		);

		Lblock_rope_joint = Joint::add_balljoint(
			arm_ray_data.coll, coll->get_collider(coll->get_collider_size() - 1),
			//vector3_quatrotate(contact_point - data.coll->transform->position, data.coll->transform->orientation.inverse()),
			vector3_quatrotate(arm_ray.position + arm_ray.direction * coll->sphree_offset_size * (coll->sphere_num_size - 1) - arm_ray_data.coll->transform->position, arm_ray_data.coll->transform->orientation.inverse()),

			Vector3(0),
			0.1f
		);

		auto vertex_data = coll->get_vertex_data(coll->get_collider_size() - 1);
		vertex_data.is_hitable = false;
		coll->set_vertex_data(coll->get_collider_size() - 1, vertex_data);

		auto renderer = Lrope_go->addComponent<Rope_renderer>();
		Lrope_go->renderer = renderer;
		renderer->radius = rope_sphere_r * 0.6f;
		renderer->split_count = 30;
		renderer->set_meshoffset(coll->get_vertex_offset());
	}

	// 縮める
	if (input_changer->is_rope_shrink && Lrope_go != nullptr) {
		const auto& structural_type = Collider_Rope::Rope_constraint_type::sructural_spring;
		const auto& bending_type = Collider_Rope::Rope_constraint_type::bending_spring;


		if (vector3_distance(Lhand_collider->transform->position, +Lrope_coll->get_vertex_offset()->at(0).first + Lrope_coll->gameobject->transform->position) < 0.2f)
		{
			{
				int joint_size = Lrope_coll->get_joint_count(structural_type);

				for (int i = 0; i < joint_size; ++i) {
					if (Lrope_coll->get_joint_ptr(structural_type, i)->offset == 0)continue;
					Lrope_coll->get_joint_ptr(structural_type, i)->offset = ALmax(Lrope_coll->get_joint_ptr(structural_type, i)->offset - 10 * time->deltaTime(), 0);

					// 一番最後は手と接続点を直接くっつけて安定させる
					if (i == joint_size - 1 && Lrope_coll->get_joint_ptr(structural_type, i)->offset == 0) {
						Lblock_hand_joint = Joint::add_balljoint(
							Lblock_rope_joint->get_colliderA(), Lhand_collider,
							Lblock_rope_joint->get_anchors()[0].posA, Vector3(0)
						);
						Lblock_hand_joint->offset = Lhand_collider->gameobject->transform->scale.x;

					}

					if (Lrope_coll->get_joint_ptr(structural_type, i)->offset == 0 && i < joint_size - 1) collider_num++;

					break;
				}
			}

			if (check_onplayer_coll->concoll_enter(Collider_tags::Stage) && onground_collider == nullptr)
			{
				float pow = fabsf(vector3_dot(Vector3(0, 1, 0), (Lrope_coll->get_vertex_offset()->at(collider_num).first - Lrope_coll->get_vertex_offset()->at(0).first).unit_vect()));
				pow = 1 - pow;
				for (int i = 0; i < Human_collider_size; ++i) {
					Human_colliders[i]->add_force(Vector3(0, 1, 0) * 250 * pow);
				}
			}
		}

		int bending_size = Lrope_coll->get_joint_count(bending_type);
		if (bending_size != 0) {
			Lrope_coll->get_joint_ptr(bending_type, 0)->offset =
				0;
		}
	}

	if (input_changer->is_rope_delete) {
		if (Lrope_go)Gameobject_manager::deleteGameobject(Lrope_go);
		if (Lblock_hand_joint)Joint::delete_joint(Lblock_hand_joint);

		Lrope_go = nullptr;
		Lblock_hand_joint = nullptr;
	}

	if (Lblock_hand_joint) {
		if (Lrope_go)Gameobject_manager::deleteGameobject(Lrope_go);
		Lrope_go = nullptr;
	}

	if (input_changer->is_Larm_state) {
		if (Lblock_hand_joint)Joint::delete_joint(Lblock_hand_joint);
		Lblock_hand_joint = nullptr;
	}

}

// 立つように力を加える
void Player::add_pow_for_stand() {
	// 今の向き
	Vector3 player_vec = vector3_quatrotate(Vector3(0, 0, -1), rotate);

	if (is_gunyatto == false)
	{
		{
			//腰をたたせる
			Quaternion off = rotate * Waist_collider->gameobject->transform->orientation.inverse();

			float rad = off.radian();
			if (rad > PI) {
				rad = rad - 2 * PI;
				Waist_collider->gameobject->transform->local_orient *= -1;
				Waist_collider->gameobject->transform->orientation *= -1;
				off = rotate * Waist_collider->gameobject->transform->orientation.inverse();
			}
			float pow = ALClamp(rad * waist_rot_pow, 0, waist_rot_max_pow);
			Waist_collider->add_torque(off.axis() * pow * 1, true);
			Waist_collider->set_max_angula_velocity(waist_rot_max_speed);
		}
		{
			//体をたたせる
			Quaternion off = rotate * Body_collider->gameobject->transform->orientation.inverse();
			//Quaternion off = Waist_collider->gameobject->transform->orientation * Body_collider->gameobject->transform->orientation.inverse();

			float rad = off.radian();
			if (rad > PI) {
				rad = rad - 2 * PI;
				Body_collider->gameobject->transform->local_orient *= -1;
				Body_collider->gameobject->transform->orientation *= -1;
				off = rotate * Body_collider->gameobject->transform->orientation.inverse();
			}
			float pow = ALClamp(rad * body_rot_pow, 0, body_rot_max_pow);
			Body_collider->add_torque(off.axis() * pow * gunyatto_pow, true);
			Body_collider->set_max_angula_velocity(body_rot_max_speed);
			//Body_collider->set_max_angula_velocity(ALmin(body_rot_max_speed, body_rot_max_speed * ToAngle(rad) * 0.1f));
		}
		{
			//顔が赤ちゃんなのを治す
			Quaternion off = Body_collider->gameobject->transform->orientation * Head_collider->gameobject->transform->orientation.inverse();
			float rad = off.radian();
			if (rad > PI) {
				rad = rad - 2 * PI;
				Head_collider->gameobject->transform->local_orient *= -1;
				Head_collider->gameobject->transform->orientation *= -1;
				off = rotate * Head_collider->gameobject->transform->orientation.inverse();
			}
			float pow = ALClamp(rad * head_rot_pow, 0, head_rot_max_pow);
			Head_collider->add_torque(off.axis() * pow * gunyatto_pow, true);
			Head_collider->set_max_angula_velocity(ALmin(head_rot_max_speed, head_rot_max_speed * ToAngle(rad) * 0.03f)); //角度の差によって最大速度を調整
		}

	}
	// gunyattoしていた時は
	else {
		// 向きのみ更新
		turn_gunyatto_dir();
	}



};

// rayを飛ばして腰を立たせる
void Player::push_waist_for_stand() {
	if (is_gunyatto == true)return;

	float dot = vector3_dot(Vector3(0, -1, 0), vector3_quatrotate(Vector3(0, -1, 0), Waist_collider->transform->orientation));
	if (dot < 0)return; //腰が下を向いていなければreturn
	dot = dot * dot;
	//dot = 1;

	if (onground_collider != nullptr) {

		float dis = vector3_dot(onground_contactpoint - Waist_collider->transform->position, Vector3(0, -1, 0));

		if (dis < onground_dis) {
			constexpr float mass = 10 * 4.5f; //パーツによって質量がまちまちなので だいたいの質量
			constexpr float stand_pow = 1000;

			Vector3 gravity_pow = Vector3(0, 1, 0) * Physics_function::Physics_manager::physicsParams.gravity * mass; //playerが滞空出来る力
			Vector3 force = Vector3(0, 1, 0) * (onground_dis * dot - dis) * stand_pow; //めり込みを治す力
			Vector3 fall_force = Vector3(0, 1, 0) * Waist_collider->linear_velocity().y * mass;

			Waist_collider->add_force(gravity_pow + force);
			onground_ray_data.coll->add_force(-(gravity_pow + fall_force), onground_contactpoint);

		}
	}

}

// 移動
void Player::linear_move() {
	// 地面に接している & 入力が無い時
	//if (is_gunyatto == false && dir.norm() == 0 && onground_collider != nullptr) {
	if (is_gunyatto == false && dir.norm() == 0) {
		Waist_collider->physics_data.drag = 0.985f;
		Body_collider->physics_data.drag = 0.985f;
	}
	// 入力があれば動きやすいように0.1にする
	else {
		Waist_collider->physics_data.drag = 0.1f;
		Body_collider->physics_data.drag = 0.1f;
	}

	float move_pow = 1;
	if (onground_collider == nullptr) {
		move_pow = 0.3f;
	}

	{
		//移動
		Waist_collider->add_force(dir * waist_move_pow * move_pow);

		// 速度制限
		Vector3 speed = Waist_collider->linear_velocity();
		speed.y = 0;
		if (speed.norm() > waist_move_max_speed * waist_move_max_speed) {
			Vector3 Yspeed = Vector3(0, Waist_collider->linear_velocity().y, 0);
			Waist_collider->linear_velocity(speed.unit_vect() * waist_move_max_speed + Yspeed);
		}

		if (onground_collider != nullptr) {
			if (onground_collider->physics_data.inertial_mass > 20) //歩くときに小さな石ころに全力を加えない
				onground_collider->add_force(-dir * waist_move_pow * move_pow, onground_contactpoint);
		}

	}
};

// 回転
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

// 移動方向を計算
void Player::accume_move_dir() {
	dir = vector3_quatrotate(input_changer->dir, camera->camera_rot).unit_vect();
	dir.y = 0;
	//dir = vector3_quatrotate(dir, quaternion_from_euler(0, 180, 0));
	dir = dir.unit_vect();

	//debug_coll->transform->local_pos = Waist->world_position() + dir * 5;
};

// 足を動かす
void Player::move_legs() {
	//return;
	if (is_gunyatto)return;

	//Waist_collider->physics_data.is_moveable = false;
	//Rleg_collider->physics_data.is_moveable = false;
	//Lleg_collider->physics_data.is_moveable = false;
	if (dir.norm() != 0) {
		const float front_value = ToRadian(30); //足は胴体より遅れるため 足を前に出すために余分にかける回転の力
		const float max_theata = ToRadian(60) * 0.5f;
		const float sin = sinf(move_timer * 1.5f * DirectX::XM_2PI); //timreから足の回転量を求める
		//float sin = sinf(move_timer * DirectX::XM_2PI * 0.5f); //timreから足の回転量を求める

		const Vector3 waist_axis = Vector3(0, -1, 0);
		Vector3 rot_axis = Vector3(1, 0, 0);

		// 向いている方向から回転軸を調整 横に歩けるように
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

// jumpさせる
void Player::make_jump() {
	if (is_gunyatto)return;

	bool is_jumpable = false;
	if (onground_collider != nullptr && //接地している
		(catch_right_joint == nullptr || catch_right_joint->get_colliderB() != onground_collider) && //右手で持っているものではない
		(catch_left_joint == nullptr || catch_left_joint->get_colliderB() != onground_collider)    //左手で持っているものではない
		)is_jumpable = true;

	if (is_jumping == true)coyote += Al_Global::second_per_frame;
	if (coyote >= 0)is_jumping = false;
	if (is_jumping == false && is_jumpable) coyote = 0.3f;
	if (is_jumping == false && !is_jumpable) coyote -= Al_Global::second_per_frame;

	if (is_gunyatto == false && coyote >= 0 && input_changer->is_jump_trigger) {

		if (onground_collider != nullptr) {

			// Y方向に力を加える
			for (int i = 0; i < Human_collider_size; i++) {
				Human_colliders[i]->linear_velocity(Vector3(Human_colliders[i]->linear_velocity().x, jump_y_power, Human_colliders[i]->linear_velocity().z));
			}
			Lleg_collider->linear_velocity(Vector3(Lleg_collider->linear_velocity().x, jump_y_power * 0.1f, Lleg_collider->linear_velocity().z));
			Rleg_collider->linear_velocity(Vector3(Rleg_collider->linear_velocity().x, jump_y_power * 0.1f, Rleg_collider->linear_velocity().z));
			Lfoot_collider->linear_velocity(Vector3(Lfoot_collider->linear_velocity().x, jump_y_power * 0.1f, Lfoot_collider->linear_velocity().z));
			Rfoot_collider->linear_velocity(Vector3(Rfoot_collider->linear_velocity().x, jump_y_power * 0.1f, Rfoot_collider->linear_velocity().z));

			// Z方向に力を加える
			for (int i = 0; i < Human_collider_size; i++) {
				float pow = vector3_dot(Human_colliders[i]->linear_velocity(), dir) * Human_colliders[i]->physics_data.inertial_mass;
				Human_colliders[i]->add_force(dir * (jump_front_power - pow * 2));
			}

			// 足元の物体に力を加える
			float mass = 1500;
			onground_collider->add_force(Vector3(0, -mass, 0) * jump_y_power, onground_contactpoint);


			is_jumping = true;
			coyote = -0.3f;

		}
	}
};

// respown処理
bool Player::check_respown() {

	auto stage = stage_manager->get_current_stage();
	// 入力がある || stage指定のY座標よりPlayerが低ければrespown
	if (input_changer->is_respown_trigger || Waist->world_position().y < stage->y_player_respown_limit + 50) {

		// goalしていたら respown処理を行わず stage切替処理を呼ぶ
		if (stage->next_stage != Stage_types::none) {
			stage_manager->set_next_stage_type(stage->next_stage);
			return true;
		}

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
				Joint::delete_joint(joint);
			}
		}

		respown_timer = 3;
	}

	// respowntimerが>0なら respown中
	if (respown_timer > 0) {
		turn_gunyatto_dir();

		if (check_onplayer_coll->concoll_enter(Collider_tags::Stage)) {
			// respown中 stageに接触していれば timerを減らす
			respown_timer -= Al_Global::second_per_frame;
		}
		else {
			// respown中 stageに接触していなければ dragを0
			for (int i = 0; i < Human_collider_size; i++) {
				Human_colliders[i]->physics_data.drag = 0.2f;
			}
		}

		return true;
	}

	{
		//jointでつなげると重力が弱くなるから & 一定のパーツは重力の影響を受けないようにしているから  下向きに力を加える
		Head_collider->add_force(Vector3(0, -1, 0) * 150);
		Waist_collider->add_force(Vector3(0, -1, 0) * 150);
		Body_collider->add_force(Vector3(0, -1, 0) * 150);
		Lfoot_collider->add_force(Vector3(0, -1, 0) * 50);
		Rfoot_collider->add_force(Vector3(0, -1, 0) * 50);
	}
	// ついでにCTR押しているときにぐにゃっとさせる
	if (input_changer->is_gunyatto_state) {
		//for (int i = 0; i < Human_collider_size; i++) {
		//	Human_colliders[i]->physics_data.drag = 0.2f;
		//}
		return true;
	}

	return false;
}

// playerの向きを調整
void Player::turn_gunyatto_dir() {
	//Vector3 dir = vector3_quatrotate(Vector3(0, 0, 1), Waist->transform->orientation);

	// 腰の向き
	Vector3 dir = vector3_quatrotate(Vector3(0, 0, 1), Waist->transform->orientation);

	// 腰から頭への向き
	Vector3 waist_head_dir = (Head->world_position() - Waist->world_position()).unit_vect();

	// 回転軸
	Vector3 rotate_axis = vector3_cross(Vector3(0, 1, 0), waist_head_dir);

	// 回転角度
	float radian = vector3_radian(Vector3(0, 1, 0), waist_head_dir);

	Vector3 front_dir = vector3_quatrotate(dir, quaternion_axis_radian(rotate_axis, radian));
	front_dir.y = 0;
	front_dir = front_dir.unit_vect();

	//// 腹ばいか 仰向けか
	//if (fabsf(dir.y) > 0.5f) {
	//	if (dir.y < 0) dir = +waist_head_dir;
	//	else           dir = -waist_head_dir;
	//}
	//// 立っているなら何もせずにreturn
	//else return;

	// 今の向き
	Vector3 player_vec = vector3_quatrotate(Vector3(0, 0, -1), rotate);

	// 差分を求めて
	float angle = vector3_angle(front_dir, player_vec);

	// 回転軸は0,1,0にしたいから 外積で回転方向の確認
	if (vector3_cross(player_vec, front_dir).y < 0)angle *= -1;

	rotate *= quaternion_axis_angle(Vector3(0, 1, 0), angle);

	Debug::set("waist_head_dir", waist_head_dir);
	Debug::set("front_dir", front_dir);
	Debug::set("player_vec", player_vec);
	Debug::set("angle", angle);

}

// dragを初期値にする
void Player::set_default_data() {
	for (int i = 0; i < Human_collider_size; i++) {
		Human_colliders[i]->physics_data.drag = Human_default_drags[i];
	}

	Material_manager::find_material("player_material_01")->constant_buffer_data[0] = Vector4(5, 4, 0, 0);
	Material_manager::find_material("player_material_02")->constant_buffer_data[0] = Vector4(5, 4, 0, 0);
	camera->gameobject->findComponent<Camera_component>()->fov = 60;
}

// onground_colliderを更新
void Player::update_onground() {

	onground_collider = nullptr;

	float dot = vector3_dot(Vector3(0, -1, 0), vector3_quatrotate(Vector3(0, -1, 0), Waist_collider->transform->orientation));
	if (dot < 0)return; //腰が下を向いていなければreturn
	dot = dot * dot;

	// sphereを飛ばして確認
	Ray ray;
	ray.direction = Vector3(0, -1, 0);
	ray.position = Waist_collider->transform->position;
	onground_ray_data.collider_tag = Collider_tags::Stage;
	if (ray.sphere_cast(onground_radius, onground_contactpoint, onground_ray_data) && onground_ray_data.raymin < onground_dis) onground_collider = onground_ray_data.coll;

}

void Player::update_gnyat_pow() {
	float gunyatto_pows[3] = { 1,1,1 };
	//gnyat_pow = 1;

	// 腕の情報から値を計算
	if (catch_left_joint != nullptr && catch_right_joint != nullptr &&
		onground_collider == nullptr &&
		is_jumping == false
		) {
		// 両手でつかんでいる&接地していない&jumpした直後ではない
		hand_gunyatto_pow -= 5 * time->deltaTime(); //ぐにゃっとさせていく

	}
	else if (catch_left_joint != nullptr && catch_right_joint != nullptr) {
		// 両手でつかんでいる&そこそこ腰が立っており接地している
		if (vector3_dot(Vector3(0, -1, 0), vector3_quatrotate(Vector3(0, -1, 0), Waist->transform->orientation)) > 0.7f)
			hand_gunyatto_pow += 1 * time->deltaTime(); //しっかりさせていく
	}

	else if (catch_left_joint == nullptr || catch_right_joint == nullptr) {
		// 片方でもつかんでいないとき
		hand_gunyatto_pow = 1; //しっかりする
	}
	hand_gunyatto_pow = ALClamp(hand_gunyatto_pow, -1, 1);

	// 足、腕、腰がongroundしていないのに引っかかることが多々あるため 調整
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
	////両手がstaticなものを持っているとき、ぐにゃっと
	//if ((catch_left_joint != nullptr && catch_left_joint->get_colliderB()->tag & Collider_tags::Static_Stage) &&
	//	(catch_right_joint != nullptr && catch_right_joint->get_colliderB()->tag & Collider_tags::Static_Stage) &&
	//	(is_catch_start_yplus = true && vector3_quatrotate(Vector3(0, 0, 1), camera->camera_rot).y < 0)
	//	) {
	//	gnyat_pow = 0;
	//}
	//is_gunyatto = false;
	//if (gnyat_pow == 0)is_gunyatto = true;
}

// "物を持つ"jointを削除する
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

// respown処理
void Player::respown() {
	auto stage = stage_manager->get_current_stage();

	// 座標移動
	Vector3 off = vector3_quatrotate(stage->player_respown_pos - Waist->world_position(), Waist->parent()->world_orientate().inverse());
	for (int i = 0; i < Human_gameobject_size; i++) {
		Human_gameobjects[i]->transform->local_pos += off;
	}

	// 力のreset& 軽く下に速度を加える
	for (int i = 0; i < Human_collider_size; i++) {
		Human_colliders[i]->reset_force();
		Human_colliders[i]->linear_velocity(Vector3(0, -1, 0)); //fleezeに引っかからないように軽く力を加える
	}

	// 持っているものを離す
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

	// ropeの削除
	{
		if (Lrope_go)Gameobject_manager::deleteGameobject(Lrope_go);
		if (Lblock_hand_joint)Joint::delete_joint(Lblock_hand_joint);

		Lrope_go = nullptr;
		Lblock_hand_joint = nullptr;
	}

	for (int i = 0; i < Human_collider_size; ++i) {
		Human_colliders[i]->set_max_linear_velocity(FLT_MAX);
		Human_colliders[i]->set_max_angula_velocity(FLT_MAX);
	}

	respown_timer = 3;

}

void Player::set_Tpause(const Vector3& position, const Quaternion& orient) {

	//::: gameobjectの座標設定 :::
	constexpr Vector3 head_size = Vector3(1, 1, 1);
	constexpr Vector3 body_size = Vector3(0.8f, 0.6f, 0.8f);
	constexpr Vector3 Waist_size = Vector3(0.81f, 0.7f, 0.81f);
	constexpr Vector3 sholder_size = Vector3(0.35f, 0.6f, 0.4f);
	constexpr Vector3 arm_size = sholder_size;
	constexpr Vector3 hand_size = Vector3(0.45f, 0.45f, 0.45f);
	constexpr float arm_y_pos = 0.18f;
	constexpr Vector3 Leg_size = Vector3(0.3f, 0.15, 0.3f);
	constexpr Vector3 Foot_size = Vector3(0.4f, 0.25, 0.4f);
	constexpr float leg_x_pos = 0.6f;

	Head->transform->local_pos = Vector3(0, body_size.y + head_size.y + 0.2f, 0);
	Body->transform->local_pos = Vector3(0, 0, 0);
	Waist->transform->local_pos = Vector3(0, -0.7f * 2, 0);

	Lsholder->transform->local_pos = Vector3(+1.62, arm_y_pos, -0.30f);
	Lelbow->transform->local_pos = Vector3(+(body_size.x + arm_size.y * 3 + 0.2f), arm_y_pos, -0.98f);
	Lhand->transform->local_pos = Vector3(+(body_size.x + arm_size.y * 4 - 0.2f), arm_y_pos - 0.15f, -1.07f);
	Rsholder->transform->local_pos = Vector3(-1.62, arm_y_pos, -0.30f);
	Relbow->transform->local_pos = Vector3(-(body_size.x + arm_size.y * 3 + 0.2f), arm_y_pos, -0.98f);
	Rhand->transform->local_pos = Vector3(-(body_size.x + arm_size.y * 4 - 0.2f), arm_y_pos - 0.15f, -1.07f);

	Rleg->transform->local_pos = Vector3(-leg_x_pos, -(body_size.y + Waist_size.y * 2 + Foot_size.y * 2), 0);
	Rfoot->transform->local_pos = Vector3(-leg_x_pos, -(body_size.y + Waist_size.y * 2 + Foot_size.y * 5), 0);
	Lleg->transform->local_pos = Vector3(+leg_x_pos, -(body_size.y + Waist_size.y * 2 + Foot_size.y * 2), 0);
	Lfoot->transform->local_pos = Vector3(+leg_x_pos, -(body_size.y + Waist_size.y * 2 + Foot_size.y * 5), 0);

	for (int i = 0; i < Human_gameobject_size; ++i) {
		Human_gameobjects[i]->transform->local_pos = vector3_quatrotate(Human_gameobjects[i]->transform->local_pos, orient);
		Human_gameobjects[i]->transform->local_pos += position;
	}


}

void Player::set_moveable(bool is_moveable) {
	for (int i = 0; i < Human_collider_size; ++i) {
		Human_colliders[i]->physics_data.is_moveable = is_moveable;
		Human_colliders[i]->physics_data.is_hitable = is_moveable;
	}
}