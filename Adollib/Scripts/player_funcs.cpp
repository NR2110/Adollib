#include "player.h"

#include "../Adollib/Scripts/Main/Adollib.h"
#include "../Adollib/Scripts/Main/defines.h"
#include "../Adollib/Scripts/Object/gameobject_manager.h"

#include "../Adollib/Scripts/Physics/ALP__physics_manager.h"
#include "../Adollib/Scripts/Physics/ray.h"
#include "../Adollib/Scripts/Physics/collider_rope.h"
#include "../Adollib/Scripts/Imgui/work_meter.h"

#include "../Adollib/Scripts/Renderer/material_manager.h"
#include "../Adollib/Scripts/Renderer/rope_renderer.h"
#include "../Adollib/Scripts/Renderer/directional_shadow.h"

#include "../../Scripts/Main/mono_audio.h"

#include "stage_manager.h"
#include "stage_base.h"
#include "camera.h"
#include "input_changer.h"
#include "se_manager.h"

#ifdef ON_DEBUG
#define Rope_Shot_Always
#endif
#define Rope_Shot_Always

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
	Quaternion* arm_rad_to_camera[2] = {
		&Rarm_rad_to_camera,
		&Larm_rad_to_camera
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

			Quaternion goal = quaternion_axis_angle(Vector3(0, 1, 0), sign[i] * 90.0f) * quaternion_axis_angle(Vector3(1, 0, 0), 90) * camera_off * Body_collider->transform->orientation;

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

			{
				//�r
				auto& collider = colliders[i + 2];
				Quaternion off = -collider->transform->orientation * goal.inverse();
				Vector3 axis = -off.axis();
				float rad = off.radian();
				if (rad > PI) {
					collider->transform->orientation *= -1;
					collider->transform->local_orient *= -1;

					off = -collider->transform->orientation * goal.inverse();
					axis = -off.axis();
					rad = off.radian();
				}
				*arm_rad_to_camera[i] = off; //���ݗp

				float pow = ALClamp(rad * hand_rot_pow, 0, hand_rot_max_pow);
				Vector3 world_axis = vector3_quatrotate(axis, collider->transform->orientation);
				collider->add_torque(world_axis * pow * collider->physics_data.inertial_mass);
				collider->set_max_angula_velocity(ALmin(hand_rot_max_speed, hand_rot_max_speed * ToAngle(rad) * 0.03f));
			}

			{
				// ��
				auto& collider = colliders[i];
				Quaternion off = -collider->transform->orientation * goal.inverse();
				Vector3 axis = -off.axis();
				float rad = off.radian();
				if (rad > PI) {
					collider->transform->orientation *= -1;
					collider->transform->local_orient *= -1;

					off = -collider->transform->orientation * goal.inverse();
					axis = -off.axis();
					rad = off.radian();
				}

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
	const Quaternion arm_rad_to_camera[2] = {
		Rarm_rad_to_camera,
		Larm_rad_to_camera
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

		// �r�̉�]�O�A��̌������v�Z
		Vector3 check_0 = vector3_quatrotate(Vector3(1, 0, 0), colliders[i + 2]->transform->orientation);
		Vector3 check_1 = vector3_quatrotate(check_0, arm_rad_to_camera[i]);
		//����
		if (
			input_arm[i] && joint == nullptr && *is_maked_joint[i] == false // Key��������Ă��� joint�����݂��Ȃ� ��state���ɕ�������ł��Ȃ�
			) {
			collider->is_save_contacted_colls = true;
			auto contacted_colls = collider->get_Contacted_data();

			//�Փ˂��Ă���collider�����ԋ߂����̂�T��
			Contacted_data const* min_data = nullptr;
			for (auto& c_coll : contacted_colls) {
				if (
					(min_data == nullptr || min_data->penetrate > c_coll.penetrate) && // �����̔�r
					!(c_coll.coll->tag & Collider_tags::Cant_catch) && //���ĂȂ����̂łȂ�
					(
						c_coll.coll->tag & Collider_tags::Catch_able_easy || //���݂₷�����̂�(���̓�̏����Ȃ��ł��߂����)
						check_1.x - check_0.x < 0 || //���Ɍ������Ƃ��Ă���
						arm_rad_to_camera[i].radian() < ToRadian(50) //�r�ƃJ�����̊p�x�����ȉ��ł���
						)
					) {
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
				joint->shrink_bias = 0.5f;
				joint->stretch_bias = 0.5f;

				se_manager->hand_pop_SE();
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


	{
		auto& jointL = *joints[0];
		auto& jointR = *joints[1];
		if (jointL != nullptr && jointR != nullptr) {
			Vector3 world_posL = jointL->get_colliderA()->transform->position + vector3_quatrotate(jointL->get_anchors()[0].posA, jointL->get_colliderA()->transform->orientation);
			Vector3 world_posR = jointR->get_colliderA()->transform->position + vector3_quatrotate(jointR->get_anchors()[0].posA, jointR->get_colliderA()->transform->orientation);

			Debug::set("vector3_distance(world_posL, world_posR)", vector3_distance_sqr(world_posL, world_posR));
			if (vector3_distance(world_posL, world_posR) > 200) {
				Joint::delete_joint(jointL);
				Joint::delete_joint(jointR);
			}
		}
	}

};

// rope������
void Player::shot_rope() {

	if (Waist_collider->concoll_enter(Collider_tags::Rope_event))is_shotable = true;
#ifndef Rope_Shot_Always
	if (is_shotable == false)return;
#endif

	const float rope_max_length = 90;

	// �Ƃ肠�����`�悳��Ȃ��悤�ɐ�����΂�
	{
		rope_hit_sphere->transform->local_pos = Vector3(10000, 10000, 10000000);
		rope_hit_cylinder->transform->local_pos = Vector3(10000, 10000, 10000000);
	}

	dither_timer = ALClamp(dither_timer, -0.3f, 1);

	// rope���˃{�^����������Ă���Ƃ�
	// �\�����\��,�J�������߂Â���player��dither
	if (input_changer->is_rope_state) {
		{
			// dither_timer����material��dither���s�� (shader�͂��炩���߃Z�b�g����Ă���̂�constant_buffer_data��ύX)
			// dither_timer��fov�𒲐�����camera�̋߂Â��Ă銴������
			dither_timer += time->deltaTime() * 2.0f;
			float dither_pow = ALClamp(dither_timer, 0, 1);

			//Waist->renderer->get_material()->constant_buffer_data[0] = Vector4(500, 1000, 0, 0);
			player_mterial_0->constant_buffer_data[0] = Vector4(5 + (500 - 5) * dither_pow, 4 + (1000 - 4) * dither_pow, 0, 0);
			player_mterial_1->constant_buffer_data[0] = Vector4(5 + (500 - 5) * dither_pow, 4 + (1000 - 4) * dither_pow, 0, 0);
			camera->gameobject->findComponent<Camera_component>()->fov = 60 + (40 - 60) * dither_pow;
		}

		// �\�����̕\�� camera�̒����_��player�̎肩��R���o��
		// �J���������raycast
		Ray camera_ray;
		camera_ray.direction = vector3_quatrotate(Vector3(0, 0, 1), camera->transform->orientation);
		camera_ray.position = camera->transform->position;

		Ray::Raycast_struct camera_ray_data;
		camera_ray_data.collider_tag = Collider_tags::Stage;
		Vector3 contact_point;

		if (camera_ray.sphere_cast(rope_sphere_r, contact_point, camera_ray_data)) {
			// �J������ray�̏Փ˓_������player�̎肩��raycast
			camera_ray_data.raymin += rope_sphere_r;
			rope_hit_sphere->transform->local_pos = camera_ray.position + camera_ray.direction * camera_ray_data.raymin;

			Ray arm_ray;
			arm_ray.direction = (rope_hit_sphere->transform->local_pos - Lhand->transform->position).unit_vect();
			arm_ray.position = Lhand->transform->position;

			Ray::Raycast_struct arm_ray_data;
			arm_ray_data.collider_tag = Collider_tags::Stage;

			if (arm_ray.sphere_cast(rope_sphere_r, contact_point, arm_ray_data)) {
				// �Փ˓_�֌����ė\�����̕\��
				arm_ray_data.raymin += rope_sphere_r;
				rope_hit_sphere->transform->local_pos = arm_ray.position + arm_ray.direction * arm_ray_data.raymin;

				rope_hit_cylinder->transform->local_pos = arm_ray.position + arm_ray.direction * arm_ray_data.raymin * 0.5f;
				rope_hit_cylinder->transform->local_orient = quaternion_from_to_rotate(Vector3(0, 1, 0), arm_ray.direction);
				rope_hit_cylinder->transform->local_scale.y = arm_ray_data.raymin * 0.5f;

				// �͂��Ă���Η΁A�͂��Ă��Ȃ���ΐ�
				if (arm_ray_data.raymin > rope_max_length) {
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
		// �����Ȃ����� dither timer��߂���fov,dither�����ɖ߂�
		dither_timer -= time->deltaTime() * 1.5f;
		float dither_pow = ALClamp(dither_timer, 0, 1);

		//Waist->renderer->get_material()->constant_buffer_data[0] = Vector4(500, 1000, 0, 0);
		player_mterial_0->constant_buffer_data[0] = Vector4(5 + (500 - 5) * dither_pow, 4 + (1000 - 4) * dither_pow, 0, 0);
		player_mterial_1->constant_buffer_data[0] = Vector4(5 + (500 - 5) * dither_pow, 4 + (1000 - 4) * dither_pow, 0, 0);
		camera->gameobject->findComponent<Camera_component>()->fov = 60 + (40 - 60) * dither_pow;

	}

	// ���˂��ꂽ�Ƃ� (���is_rope_state�Ɨ������Ȃ�)(TODO : ���g���\���Ă邩��K�����ꂢ�ɂ�����)
	if (input_changer->is_rope_releaced) {
		nearest_rope_collider_num = 1;
		dither_timer = ALmin(1, dither_timer);

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

		// �͂��Ă��Ȃ���ΓK���� return
		if (arm_ray_data.coll == nullptr || arm_ray_data.raymin > rope_max_length)return;

		// �O��rope���폜, joint���폜
		if (Lrope_go)Gameobject_manager::deleteGameobject(Lrope_go);
		if (Lblock_hand_joint)Joint::delete_joint(Lblock_hand_joint);

		// �n�_��arm�̍��W��rope GO�̐���
		Lrope_go = Gameobject_manager::create("rope", Scenelist::scene_player);
		Lrope_go->transform->local_pos = arm_ray.position;

		// collider�̃A�^�b�`&�����ݒ�
		Lrope_coll = Lrope_go->addComponent<Collider_Rope>();
		auto& coll = Lrope_coll;

		coll->sphere_size_r = rope_sphere_r;
		coll->sphree_offset_size = coll->sphere_size_r * 3;
		int sphere_count = (int)(arm_ray_data.raymin / coll->sphree_offset_size);
		sphere_count += 1;
		coll->sphere_num_size = sphere_count;
		//coll->sphere_num_size = 84;
		coll->start_rope_dir = arm_ray.direction;
		coll->tag |= Collider_tags::Human_rope;
		coll->ignore_tags |= Collider_tags::Human;
		coll->default_physics_data.inertial_mass = 0.4f;
		coll->create_rope();

		// �ꏊ�ɂ���Ď��ʂ𒲐�����
		for (int i = 0; i < coll->get_collider_size(); ++i) {
			auto physics_data = coll->get_vertex_data(i);
			physics_data.inertial_mass = 0.4f * (1 + (float)i / coll->get_collider_size());
			coll->set_vertex_data(i, physics_data);
		}

		// �r��rope���Ȃ�joint
		Joint::add_balljoint(
			Lhand_collider, coll->get_collider(0),
			Vector3(0), Vector3(0)
		);

		// �Ȃ�����̂�rope���Ȃ�joint
		Lblock_rope_joint = Joint::add_balljoint(
			arm_ray_data.coll, coll->get_collider(coll->get_collider_size() - 1),
			//vector3_quatrotate(contact_point - data.coll->transform->position, data.coll->transform->orientation.inverse()),
			vector3_quatrotate(arm_ray.position + arm_ray.direction * coll->sphree_offset_size * (float)(coll->sphere_num_size - 1) - arm_ray_data.coll->transform->position, arm_ray_data.coll->transform->orientation.inverse()),

			Vector3(0),
			0.1f
		);

		// block�ɂȂ�collider���Փ˂��Ȃ��悤��(�\��₷������)
		auto vertex_data = coll->get_vertex_data(coll->get_collider_size() - 1);
		vertex_data.is_hitable = false;
		coll->set_vertex_data(coll->get_collider_size() - 1, vertex_data);

		// renderer�̃A�^�b�`&�Z�b�g
		auto renderer = Lrope_go->addComponent<Rope_renderer>();
		Lrope_go->renderer = renderer;
		renderer->radius = rope_sphere_r * 0.6f;
		renderer->split_count = 20;
		renderer->set_meshoffset(coll->get_vertex_offset());

	}

	// �k�߂�
	if (input_changer->is_rope_shrink && Lrope_go != nullptr) {
		const auto& structural_type = Collider_Rope::Rope_constraint_type::sructural_spring;
		const auto& bending_type = Collider_Rope::Rope_constraint_type::bending_spring;

		// ����arm��rope�����ꂷ���Ă���Ώk�߂Ȃ�
		if (vector3_distance(Lhand_collider->transform->position, +Lrope_coll->get_vertex_offset()->at(0).first + Lrope_coll->gameobject->transform->position) < 0.4f)
		{
			{
				int joint_size = Lrope_coll->get_joint_count(structural_type);

				// ���߂�offset��0�o�Ȃ��Ƃ�����k�߂�
				for (int i = 0; i < joint_size; ++i) {
					if (Lrope_coll->get_joint_ptr(structural_type, i)->offset == 0)continue;
					Lrope_coll->get_joint_ptr(structural_type, i)->offset = ALmax(Lrope_coll->get_joint_ptr(structural_type, i)->offset - 10 * time->deltaTime(), 0);

					// ��ԍŌ�͎�Ɛڑ��_�𒼐ڂ������Ĉ��肳����
					if (i == joint_size - 1 && Lrope_coll->get_joint_ptr(structural_type, i)->offset == 0) {
						Lblock_hand_joint = Joint::add_balljoint(
							Lblock_rope_joint->get_colliderA(), Lhand_collider,
							Lblock_rope_joint->get_anchors()[0].posA, Vector3(0)
						);
						Lblock_hand_joint->offset = Lhand_collider->gameobject->transform->scale.x;

						Lrope_coll->get_joint_ptr(structural_type, i)->stretch_bias = 0;
						Lrope_coll->get_joint_ptr(structural_type, i)->shrink_bias = 0;

					}

					// offset��0�ɂȂ����Ƃ� ��ԋ߂�rope�̒��_�����̒��_�֍X�V
					if (Lrope_coll->get_joint_ptr(structural_type, i)->offset == 0 && i < joint_size - 1) nearest_rope_collider_num++;

					break;
				}
			}

			// �k�߂�ہAstage�ɑ̂��������Ă��邪 �ݒu���Ă��Ȃ��Ƃ�
			// ������֗͂������� ���₷���悤�ɂ���
			if (check_onplayer_coll->concoll_enter(Collider_tags::Stage) && onground_collider == nullptr)
			{
				// ������ɉ������
				float pow = fabsf(vector3_dot(Vector3(0, 1, 0), (Lrope_coll->get_vertex_offset()->at(nearest_rope_collider_num).first - Lrope_coll->get_vertex_offset()->at(0).first).unit_vect()));
				pow = 1 - pow;
				for (int i = 0; i < Human_collider_size; ++i) {
					Human_colliders[i]->add_force(Vector3(0, 1, 0) * 250 * pow);
				}
			}
		}

		{
			// bending joint(�Ȃ��΂�)�̒���
			int bending_size = Lrope_coll->get_joint_count(bending_type);
			if (bending_size != 0) {
				// ��ԋ߂� bending joint��offset��0�� (����joint�͏k�݂�������L�΂�joint �k�߂�joint�̏ꍇ 0�ɂ��� �L�΂��Ȃ��悤��)
				Lrope_coll->get_joint_ptr(bending_type, 0)->stretch_bias = 0;
				Lrope_coll->get_joint_ptr(bending_type, 0)->shrink_bias = 0;
			}
			if (bending_size > 1) {
				// ��ԋ߂� bending joint��offset��0�� (����joint�͏k�݂�������L�΂�joint �k�߂�joint�̏ꍇ 0�ɂ��� �L�΂��Ȃ��悤��)
				Lrope_coll->get_joint_ptr(bending_type, 1)->stretch_bias = 0;
				Lrope_coll->get_joint_ptr(bending_type, 1)->shrink_bias = 0;
			}
		}
	}

	// �폜�����Ƃ�
	if (input_changer->is_rope_delete) {
		if (Lrope_go)Gameobject_manager::deleteGameobject(Lrope_go);
		if (Lblock_hand_joint)Joint::delete_joint(Lblock_hand_joint);

		Lrope_go = nullptr;
		Lblock_hand_joint = nullptr;
	}

	// rope�̒�����0�ɂȂ����Ƃ� block��hand��joint�𐶐�����
	if (Lblock_hand_joint) {
		if (Lrope_go)Gameobject_manager::deleteGameobject(Lrope_go);
		Lrope_go = nullptr;
	}
	// ��L�Ő�������Lblock_hand_joint������Ƃ��ɁA�����������Ƃ����ꍇ�ALblock_hand_joint���폜����
	if (input_changer->is_Larm_state) {
		if (Lblock_hand_joint)Joint::delete_joint(Lblock_hand_joint);
		Lblock_hand_joint = nullptr;
	}

}

// ���悤�ɗ͂�������
void Player::add_pow_for_stand() {
	// ���̌���
	Vector3 player_vec = vector3_quatrotate(Vector3(0, 0, -1), rotate);

	if (is_gunyatto == false && gunyatto_pow != 0)
	{
		{
			//������������
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
			//�̂���������
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
			//�炪�Ԃ����Ȃ̂�����
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
			Head_collider->set_max_angula_velocity(ALmin(head_rot_max_speed, head_rot_max_speed * ToAngle(rad) * 0.03f)); //�p�x�̍��ɂ���čő呬�x�𒲐�
		}

	}
	// gunyatto���Ă�������
	else if (gunyatto_pow != 0) {
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

	if (onground_collider != nullptr) {

		float dis = vector3_dot(onground_contactpoint - Waist_collider->transform->position, Vector3(0, -1, 0));

		if (dis < onground_dis) {
			constexpr float mass = 10 * 4.5f; //�p�[�c�ɂ���Ď��ʂ��܂��܂��Ȃ̂� ���������̎���
			constexpr float stand_pow = 500;

			Vector3 normal = Vector3(0, 1, 0);
			float gravity_pow = Physics_function::Physics_manager::physicsParams.gravity * mass; //player���؋�o�����
			float force = (onground_dis * dot - dis) * stand_pow; //�߂荞�݂�������
			float fall_force = Waist_collider->linear_velocity().y * mass;

			Waist_collider->add_force(normal * (gravity_pow + force));

			// ����Ă�����̂ɏ�ɗ͂�������
			if (onground_collider->physics_data.is_kinmatic_linear == true)
				onground_collider->add_force(normal * -(gravity_pow + fall_force), onground_contactpoint);

			// ����Ă�����̂̑��x��Player�ɉ�����
			if (
				(catch_right_joint == nullptr || catch_right_joint->get_colliderB() != onground_collider) && //�E��Ŏ����Ă�����̂ł͂Ȃ�
				(catch_left_joint == nullptr || catch_left_joint->get_colliderB() != onground_collider) &&    //����Ŏ����Ă�����̂ł͂Ȃ�
				(Lblock_rope_joint == nullptr || Lblock_rope_joint->get_colliderA() != onground_collider) //rope�ł���ł�����̂ł͂Ȃ�
				) {
				Vector3 onground_velocity = onground_collider->get_point_velocity(onground_contactpoint, false);
				float onground_pow = onground_collider->physics_data.inertial_mass * 0.5f;
				if (onground_collider->physics_data.is_moveable == false ||
					onground_collider->physics_data.is_kinmatic_linear == false ||
					onground_collider->physics_data.is_kinmatic_anglar == false
					)onground_pow = 6;
				onground_pow = ALClamp(onground_pow, 0, 6);

				onground_velocity.y = 0;
				if (onground_velocity.norm() != 0) {
					for (int i = 0; i < Human_collider_size; ++i) {
						Human_colliders[i]->add_force(onground_velocity * onground_pow);
					}
				}
			}
		}
	}


}

// �ړ�
void Player::linear_move() {
	// �n�ʂɐڂ��Ă��� & ���͂�������
	//if (is_gunyatto == false && dir.norm() == 0 && onground_collider != nullptr) {
	if (is_gunyatto == false && dir.norm() == 0) {
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
		move_pow = 0.4f;
	}

	if (dir.norm() != 0)
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
			if (onground_collider->physics_data.inertial_mass > 50 && onground_collider->physics_data.is_kinmatic_linear == true) //�����Ƃ��ɏ����Ȑ΂���ɑS�͂������Ȃ�
				onground_collider->add_force(-dir * waist_move_pow * move_pow, onground_contactpoint);
		}

		if (onground_collider != nullptr)
			se_manager->step_SE();
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
		(catch_left_joint == nullptr || catch_left_joint->get_colliderB() != onground_collider)
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

			// ���͕����ɗ͂�������
			for (int i = 0; i < Human_collider_size; i++) {
				float pow = vector3_dot(Human_colliders[i]->linear_velocity(), dir) * Human_colliders[i]->physics_data.inertial_mass;
				Human_colliders[i]->add_force(dir * (jump_front_power - pow * 2));
			}
			// camera�����ɗ͂�������
			if (dir.norm() == 0 && (input_changer->is_Larm_state || input_changer->is_Rarm_state))
				for (int i = 0; i < Human_collider_size; i++) {
					Vector3 camera_dir = vector3_quatrotate(Vector3(0, 0, 1), camera->transform->orientation);
					camera_dir.y = 0;
					camera_dir = camera_dir.unit_vect();

					Human_colliders[i]->add_force(camera_dir * 100);
				}

			// �����̕��̂ɗ͂�������
			float mass = 1500;
			if (onground_collider->physics_data.is_kinmatic_linear == true)
				onground_collider->add_force(Vector3(0, -mass, 0) * jump_y_power, onground_contactpoint);


			is_jumping = true;
			coyote = -0.3f;

			se_manager->jump_SE();
		}
	}
};

// respown����
bool Player::check_respown() {
	if (stage_manager->now_stage == Stage_types::none)return false;

	auto stage = stage_manager->get_current_stage();

	// ���͂����� || stage�w���Y���W���Player���Ⴏ���respown
	if (input_changer->is_respown_trigger || Waist->world_position().y < stage->y_player_respown_limit) {

		// goal���Ă����� respown�������s�킸 stage�ؑ֏������Ă�
		if (stage->next_stage != Stage_types::none) {
			if (stage->next_stage == Stage_types::Title) {
				Scene_manager::set_active(Scenelist::scene_title);
				Scene_manager::set_inactive(Scenelist::scene_game);
				Scene_manager::set_inactive(Scenelist::scene_player);
				return true;
			}
			else {
				stage_manager->set_next_stage_type(stage->next_stage);
				return true;
			}
		}

		respown();
	}

	// goal�ɐG�ꂽ�Ƃ� gunyatto������ ����ł�����́Arope�������I�ɗ�����gunyatto������
	if (Waist_collider->concoll_enter(Collider_tags::Goal_stage)) {
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

		// rope�̍폜
		{
			if (Lrope_go)Gameobject_manager::deleteGameobject(Lrope_go);
			if (Lblock_hand_joint)Joint::delete_joint(Lblock_hand_joint);

			Lrope_go = nullptr;
			Lblock_hand_joint = nullptr;
			Lblock_rope_joint = nullptr;
		}

		respown_timer = 1;
	}

	// respowntimer��>0�Ȃ� respown��
	if (respown_timer > 0) {
		turn_gunyatto_dir();

		if (check_onplayer_coll->concoll_enter(Collider_tags::Stage)) {
			// respown�� stage�ɐڐG���Ă���� timer�����炷
			respown_timer -= Al_Global::second_per_frame;

			se_manager->landing_SE();
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

	// �����瓪�ւ̌���
	Vector3 waist_head_dir = (Head->world_position() - Waist->world_position()).unit_vect();

	// ��]��
	Vector3 rotate_axis = vector3_cross(Vector3(0, 1, 0), waist_head_dir);

	// ��]�p�x
	float radian = vector3_radian(Vector3(0, 1, 0), waist_head_dir);

	Vector3 front_dir = vector3_quatrotate(dir, quaternion_axis_radian(rotate_axis, radian));
	front_dir.y = 0;
	front_dir = front_dir.unit_vect();

	// ���̌���
	Vector3 player_vec = vector3_quatrotate(Vector3(0, 0, -1), rotate);

	// ���������߂�
	float angle = vector3_angle(front_dir, player_vec);

	// ��]����0,1,0�ɂ��������� �O�ςŉ�]�����̊m�F
	if (vector3_cross(player_vec, front_dir).y < 0)angle *= -1;

	rotate *= quaternion_axis_angle(Vector3(0, 1, 0), angle);

	Debug::set("waist_head_dir", waist_head_dir);
	Debug::set("front_dir", front_dir);
	Debug::set("player_vec", player_vec);
	Debug::set("angle", angle);

}

// drag�������l�ɂ���
void Player::set_default_data() {
	for (int i = 0; i < Human_collider_size; i++) {
		Human_colliders[i]->physics_data.drag = Human_default_drags[i];
	}

	player_mterial_0->constant_buffer_data[0] = Vector4(5, 4, 0, 0);
	player_mterial_1->constant_buffer_data[0] = Vector4(5, 4, 0, 0);
	camera->gameobject->findComponent<Camera_component>()->fov = 60;
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

// gunyatto�����邩�ǂ����̍X�V
void Player::update_gnyat_pow() {
	float gunyatto_pows[3] = { 1,1,1 };
	//gnyat_pow = 1;

	// �r�̏�񂩂�l���v�Z
	if (catch_left_joint != nullptr && catch_right_joint != nullptr &&
		onground_collider == nullptr &&
		is_jumping == false
		) {
		// ����ł���ł���&�ڒn���Ă��Ȃ�&jump��������ł͂Ȃ�
		hand_gunyatto_pow -= 3 * time->deltaTime(); //���ɂ���Ƃ����Ă���

	}
	else if (catch_left_joint != nullptr && catch_right_joint != nullptr) {
		// ����ł���ł���&�����������������Ă���ڒn���Ă���
		if (vector3_dot(Vector3(0, -1, 0), vector3_quatrotate(Vector3(0, -1, 0), Waist->transform->orientation)) > 0.7f)
			hand_gunyatto_pow += 3 * time->deltaTime(); //�������肳���Ă���
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

	if (onground_collider == nullptr && Lrope_go == nullptr)onground_gunyatto_pow -= time->deltaTime();
	else onground_gunyatto_pow = 1.4f;


	//if (
	//	(Waist_collider->concoll_enter(Collider_tags::Stage)) &&
	//	onground_collider == nullptr
	//	)gunyatto_pows[2] = 0.1f;

	gunyatto_pow = ALClamp(ALClamp(onground_gunyatto_pow, 0, 1) * hand_gunyatto_pow * ((gunyatto_pows[0] * gunyatto_pows[1] * gunyatto_pows[2])), 0, 1);
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
	if (stage_manager->now_stage == Stage_types::none)return;

	auto stage = stage_manager->get_current_stage();

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
	if (another_player_ptr != nullptr) {
		another_player_ptr->delete_joint_from_tag(player_collider_tag);
	}


	// rope�̍폜
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

	// �͂�reset& �y�����ɑ��x��������
	for (int i = 0; i < Human_collider_size; i++) {
		float y_speed = fabsf(Human_colliders[i]->linear_velocity().y);
		y_speed = ALClamp(y_speed * 0.5f, 1, 10);
		Human_colliders[i]->reset_force();
		Human_colliders[i]->linear_velocity(Vector3(0, -y_speed, 0)); //fleeze�Ɉ���������Ȃ��悤�Ɍy���͂�������
	}

	// ���W�ړ�
	Vector3 respown_pow = stage->player_respown_pos + Vector3(10, 0, 0) * player_num;
	Vector3 off = vector3_quatrotate(respown_pow - Waist->world_position(), Waist->parent()->world_orientate().inverse());
	for (int i = 0; i < Human_gameobject_size; i++) {
		Human_gameobjects[i]->transform->local_pos += off;
	}

	respown_timer = 3;

}

void Player::set_Tpause(const Vector3& position, const Quaternion& orient) {

	//::: gameobject�̍��W�ݒ� :::
	constexpr Vector3 head_size = Vector3(1, 1, 1);
	constexpr Vector3 body_size = Vector3(0.8f, 0.6f, 0.8f);
	constexpr Vector3 Waist_size = Vector3(0.81f, 0.7f, 0.81f);
	constexpr Vector3 sholder_size = Vector3(0.35f, 0.6f, 0.4f);
	constexpr Vector3 arm_size = sholder_size;
	constexpr Vector3 hand_size = Vector3(0.45f, 0.45f, 0.45f);
	constexpr float arm_y_pos = 0.18f;
	constexpr Vector3 Leg_size = Vector3(0.3f, 0.15f, 0.3f);
	constexpr Vector3 Foot_size = Vector3(0.4f, 0.25, 0.4f);
	constexpr float leg_x_pos = 0.6f;

	Head->transform->local_pos = Vector3(0, body_size.y + head_size.y + 0.2f, 0);
	Body->transform->local_pos = Vector3(0, 0, 0);
	Waist->transform->local_pos = Vector3(0, -0.7f * 2, 0);

	Lsholder->transform->local_pos = Vector3(+1.62f, arm_y_pos, -0.30f);
	Lelbow->transform->local_pos = Vector3(+(body_size.x + arm_size.y * 3 + 0.2f), arm_y_pos, -0.98f);
	Lhand->transform->local_pos = Vector3(+(body_size.x + arm_size.y * 4 - 0.2f), arm_y_pos - 0.15f, -1.07f);
	Rsholder->transform->local_pos = Vector3(-1.62f, arm_y_pos, -0.30f);
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
void Player::set_shadow_camera_pos(const Vector3& pos) {
	for (auto child : *gameobject->children()) {
		auto comp = child->findComponent<Camera_component>();
		if (comp == nullptr)continue;
		comp->directional_shadow->position = pos;
	}

}
void Player::set_shadow_camera_dir(const Vector3& dir) {
	for (auto child : *gameobject->children()) {
		auto comp = child->findComponent<Camera_component>();
		if (comp == nullptr)continue;
		comp->directional_shadow->direction = dir;
	}
}
void Player::set_is_shotable(bool is) {
	is_shotable = is;
}
void Player::delete_joint_from_tag(Collider_tagbit tag) {

	if (catch_right_joint != nullptr && catch_right_joint->get_colliderB()->tag & tag)	Joint::delete_joint(catch_right_joint);
	if (catch_left_joint != nullptr && catch_left_joint->get_colliderB()->tag & tag)	Joint::delete_joint(catch_left_joint);
	if (Lblock_hand_joint != nullptr && Lblock_hand_joint->get_colliderA()->tag & tag)	Joint::delete_joint(Lblock_hand_joint);
	if (Lblock_rope_joint != nullptr && Lblock_rope_joint->get_colliderA()->tag & tag)	Joint::delete_joint(Lblock_rope_joint);
}