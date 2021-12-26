#include "stage_demo_2.h"

#include "../Adollib/Scripts/Object/gameobject_manager.h"
#include "../Adollib/Scripts/Scene/scene_manager.h"

#include "../Adollib/Scripts/Math/closest_func.h"
#include "../Adollib/Scripts/Imgui/imgui_all.h"
#include "../Adollib/Scripts/Physics/ALP__physics_manager.h"

#include "../Adollib/Scripts/Physics/ray.h"
#include "../Adollib/Scripts/Imgui/debug.h"
#include "../Adollib/Scripts/Physics/joint.h"

#include "buttan.h"
#include "kinematic_block.h"
#include "door.h"
#include "tag_andcircuit.h"
#include "touch_moveable.h"

namespace Adollib
{
	Gameobject* Stage_demo_02::set_tree(const Vector3& pos, const Vector3& scale, const Vector3& rotate, const float y_scale, const float tan_scale, Gameobject* pearent) {
		Gameobject* tree = nullptr;
		tree = Gameobject_manager::create("tree", GO_tag::None);
		if (pearent != nullptr)pearent->add_child(tree);

		Vector4 stem_color = Vector4(90, 47, 27, 255) / 255.0f;
		Vector4 reaf_color = Vector4(48, 188, 0, 255) / 255.0f;
		Vector4 floar_color = Vector4(188, 214, 54, 255) / 255.0f;

		tree->transform->local_pos = pos;
		tree->transform->local_scale = scale;
		tree->transform->local_orient = quaternion_from_euler(rotate);

		const int Tree_size = 7;
		Gameobject* tree_parts[Tree_size] = { nullptr };
		for (int i = 0; i < Tree_size; i++) {
			tree_parts[i] = Gameobject_manager::createCube("tree_part", GO_tag::Box);
		}

		tree_parts[0]->transform->local_pos = Vector3(0, 7, 0);
		tree_parts[0]->transform->local_scale = Vector3(1, 6.9f, 1);
		tree_parts[0]->renderer->color = stem_color;

		tree_parts[1]->transform->local_pos = Vector3(0, 5 + y_scale * 2 * 5, 0);
		tree_parts[1]->transform->local_scale = Vector3(1.5 * tan_scale, y_scale, 1.5 * tan_scale);
		tree_parts[1]->transform->local_orient = quaternion_from_euler(0, 0, 0);
		tree_parts[1]->renderer->color = reaf_color;

		tree_parts[2]->transform->local_pos = Vector3(0, 5 + y_scale * 2 * 4, 0);
		tree_parts[2]->transform->local_scale = Vector3(2.0 * tan_scale, y_scale, 2.0 * tan_scale);
		tree_parts[2]->transform->local_orient = quaternion_from_euler(0, 1.7f, 0);
		tree_parts[2]->renderer->color = reaf_color;

		tree_parts[3]->transform->local_pos = Vector3(0, 5 + y_scale * 2 * 3, 0);
		tree_parts[3]->transform->local_scale = Vector3(3.0 * tan_scale, y_scale, 3.0 * tan_scale);
		tree_parts[3]->transform->local_orient = quaternion_from_euler(0, 13, 0);
		tree_parts[3]->renderer->color = reaf_color;

		tree_parts[4]->transform->local_pos = Vector3(0, 5 + y_scale * 2 * 2, 0);
		tree_parts[4]->transform->local_scale = Vector3(3.5 * tan_scale, y_scale, 3.5 * tan_scale);
		tree_parts[4]->transform->local_orient = quaternion_from_euler(0, 28, 0);
		tree_parts[4]->renderer->color = reaf_color;

		tree_parts[5]->transform->local_pos = Vector3(0, 5 + y_scale * 2 * 1, 0);
		tree_parts[5]->transform->local_scale = Vector3(4.5 * tan_scale, y_scale, 4.5 * tan_scale);
		tree_parts[5]->transform->local_orient = quaternion_from_euler(0, 14, 0);
		tree_parts[5]->renderer->color = reaf_color;

		tree_parts[6]->transform->local_pos = Vector3(0, 0.2f, 0);
		tree_parts[6]->transform->local_scale = Vector3(3, 0.2f, 3);
		tree_parts[6]->transform->local_orient = quaternion_from_euler(0, 0, 0);
		tree_parts[6]->renderer->color = Vector4(grass_color, 1);

		for (int i = 0; i < Tree_size; i++) {
			tree->add_child(tree_parts[i]);
		}

		Box* boxes[Tree_size];
		Collider* coll = tree->addComponent<Collider>();
		coll->physics_data.inertial_mass = 120 * y_scale * tan_scale;
		coll->tag = Collider_tags::Stage | Collider_tags::Kinematic_Stage | Collider_tags::Caera_not_sunk_Stage;
		//coll->tag = Collider_tags::Stage | Collider_tags::Kinematic_Stage ;

		for (int i = 0; i < Tree_size; i++) {
			boxes[i] = coll->add_shape<Box>();
		}

		boxes[0]->center = Vector3(0, 7, 0);
		boxes[0]->size = Vector3(1, 7, 1);

		boxes[1]->center = Vector3(0, 5 + y_scale * 2 * 5, 0);
		boxes[1]->size = Vector3(1.5f * tan_scale, y_scale, 1.5f * tan_scale);
		boxes[1]->rotate = Vector3(0, 0, 0);

		boxes[2]->center = Vector3(0, 5 + y_scale * 2 * 4, 0);
		boxes[2]->size = Vector3(2.0f * tan_scale, y_scale, 2.0f * tan_scale);
		boxes[2]->rotate = Vector3(0, 1.7f, 0);

		boxes[3]->center = Vector3(0, 5 + y_scale * 2 * 3, 0);
		boxes[3]->size = Vector3(3.0f * tan_scale, y_scale, 3.0f * tan_scale);
		boxes[3]->rotate = Vector3(0, 13, 0);

		boxes[4]->center = Vector3(0, 5 + y_scale * 2 * 2, 0);
		boxes[4]->size = Vector3(3.5f * tan_scale, y_scale, 3.5f * tan_scale);
		boxes[4]->rotate = Vector3(0, 28, 0);

		boxes[5]->center = Vector3(0, 5 + y_scale * 2 * 1, 0);
		boxes[5]->size = Vector3(4.5f * tan_scale, y_scale, 4.5f * tan_scale);
		boxes[5]->rotate = Vector3(0, 14, 0);

		boxes[6]->center = Vector3(0, 0.2f, 0);
		boxes[6]->size = Vector3(3, 0.2f, 3);
		boxes[6]->rotate = Vector3(0, 0, 0);


		Vector3 barycenter = coll->get_barycenter();
		for (int i = 0; i < Tree_size; i++) {
			boxes[i]->center -= barycenter;
			tree_parts[i]->transform->local_pos -= barycenter;
		}
		tree->transform->local_pos += barycenter;
		tree->transform->local_pos += barycenter * (scale.y - 1);

		// 動くのであれば所定のcomponentをアタッチする
		auto comp = tree->addComponent<Stage_parts::Kinematic_block>();
		comp->this_stage = this;
		comp->this_coll = coll;
		comp->respown_pos = pos;
		comp->respown_rotate = quaternion_identity();

		return tree;
	}

	Gameobject* Stage_demo_02::set_pillar(const Vector3& pos, const Vector3& scale, const Vector3& rotate, Gameobject* pearent, const float base_length, const float pillar_length, bool moveable) {

		Gameobject* pillar_pearent = Gameobject_manager::create("pillar_pearent");

		pillar_pearent->transform->local_pos = pos;
		pillar_pearent->transform->local_orient = quaternion_from_euler(rotate);
		pillar_pearent->transform->local_scale = scale;

		Vector3 barycenter = Vector3(0, base_length * 2 + pillar_length, 0);
		pillar_pearent->transform->local_pos += barycenter;

		// 表示用
		{
			auto bot = Gameobject_manager::createCube("bot", GO_tag::Box);
			auto mid = Gameobject_manager::createCube("mid", GO_tag::Box);
			auto top = Gameobject_manager::createCube("top", GO_tag::Box);
			pillar_pearent->add_child(bot);
			pillar_pearent->add_child(mid);
			pillar_pearent->add_child(top);
			bot->transform->local_pos = Vector3(0, base_length, 0) - barycenter;
			mid->transform->local_pos = Vector3(0, base_length * 2 + pillar_length, 0) - barycenter;
			top->transform->local_pos = Vector3(0, base_length * 2 + pillar_length * 2 + 0.5f, 0) - barycenter;
			bot->transform->local_scale = Vector3(2, base_length, 2);
			mid->transform->local_scale = Vector3(1.5f, pillar_length, 1.5f);
			top->transform->local_scale = Vector3(2, 0.5f, 2);
			bot->renderer->color = Vector4(base_color * 0.9f, 1);
			mid->renderer->color = Vector4(base_color * 0.9f, 1);
			top->renderer->color = Vector4(base_color * 0.9f, 1);

		}
		// Collider
		auto coll = pillar_pearent->addComponent<Collider>();
		{
			// shapeのアタッチ
			auto bot = coll->add_shape<Box>();
			auto mid = coll->add_shape<Box>();
			auto top = coll->add_shape<Box>();
			bot->center = Vector3(0, base_length, 0) - barycenter;
			mid->center = Vector3(0, base_length * 2 + pillar_length, 0) - barycenter;
			top->center = Vector3(0, base_length * 2 + pillar_length * 2 + 0.5f, 0) - barycenter;
			bot->size = Vector3(2, base_length, 2);
			mid->size = Vector3(1.5f, pillar_length, 1.5f);
			top->size = Vector3(2, 0.5f, 2);
		}

		if (moveable) {
			// 動くのであれば所定のcomponentをアタッチする
			auto comp = pillar_pearent->addComponent<Stage_parts::Kinematic_block>();
			comp->this_stage = this;
			comp->this_coll = coll;
			comp->respown_pos = pos;
			comp->respown_rotate = quaternion_identity();

			coll->tag = Collider_tags::Box | Collider_tags::Stage | Collider_tags::Caera_not_sunk_Stage | Collider_tags::Kinematic_Stage;
			coll->physics_data.inertial_mass = 50;
		}
		else {
			coll->tag = Collider_tags::Box | Collider_tags::Stage | Collider_tags::Caera_not_sunk_Stage | Collider_tags::Static_Stage;
			coll->physics_data.is_moveable = false;
		}

		if (pearent != nullptr)pearent->add_child(pillar_pearent);
		return pillar_pearent;
	}

	Gameobject* Stage_demo_02::set_fence(const Vector3& pos, const Vector3& scale, const Vector3& rotate, Gameobject* pearent, const int fence_count) {

		Gameobject* fence_pearent = Gameobject_manager::create("fence_pearent");

		fence_pearent->transform->local_pos = pos;
		fence_pearent->transform->local_orient = quaternion_from_euler(rotate);
		fence_pearent->transform->local_scale = scale;

		set_box(Vector3(0, 4, 0), Vector3(6.5f, 0.5f, 1), Vector3(0), base_color * 0.9f, fence_pearent);

		set_box(Vector3(-4.5f, 2, 0), Vector3(0.5f, 2, 0.5f), Vector3(0), base_color * 0.9f, fence_pearent);
		set_box(Vector3(-1.5f, 2, 0), Vector3(0.5f, 2, 0.5f), Vector3(0), base_color * 0.9f, fence_pearent);
		set_box(Vector3(+1.5f, 2, 0), Vector3(0.5f, 2, 0.5f), Vector3(0), base_color * 0.9f, fence_pearent);
		set_box(Vector3(+4.5f, 2, 0), Vector3(0.5f, 2, 0.5f), Vector3(0), base_color * 0.9f, fence_pearent);

		if (pearent != nullptr)pearent->add_child(fence_pearent);

		return fence_pearent;
	}

	Gameobject* Stage_demo_02::set_door(const Vector3& pos, const Vector3& scale, const Vector3& rotate, const int is_left, Gameobject* pearent) {

		// joint用に動かないcolliderを作成
		auto door_joint = Gameobject_manager::create("door_joint");

		auto coll = door_joint->addComponent<Collider>();
		coll->physics_data.is_moveable = false;

		Collider* door = nullptr;

		//boxを生成
		{
			auto object = Gameobject_manager::createCube("Door", GO_tag::Box);
			object->renderer->color = Vector4(Vector3(90, 47, 27) / 255.0f, 1);

			object->transform->local_pos = pos;
			object->transform->local_orient = quaternion_from_euler(rotate);
			object->transform->local_scale = scale;

			door = object->addComponent<Collider>();
			auto box = door->add_shape<Box>();
			box->size = Vector3(1) * 0.9f; //突っかからないようにColliderのサイズを小さめにする

			door->tag = Collider_tags::Box | Collider_tags::Stage | Collider_tags::Caera_not_sunk_Stage | Collider_tags::Kinematic_Stage;

			if (pearent != nullptr)pearent->add_child(object);

			{
				auto totte = Gameobject_manager::createCube("Totte_front", GO_tag::Box);
				totte->renderer->color = Vector4(Vector3(207, 145, 64) / 255.0f, 1);
				object->add_child(totte);
				if (is_left) {
					totte->transform->local_pos = Vector3(+0.75f, -0.05f, -0.10f);
					totte->transform->local_scale = Vector3(+0.24f, +0.20f, 1);
				}
				else {
					totte->transform->local_pos = Vector3(-0.75f, -0.05f, -0.10f);
					totte->transform->local_scale = Vector3(+0.24f, +0.20f, 1);
				}
			}
			{
				auto totte = Gameobject_manager::createSphere("Totte_back", GO_tag::Box);
				totte->renderer->color = Vector4(Vector3(207, 145, 64) / 255.0f, 1);
				object->add_child(totte);
				if (is_left) {
					totte->transform->local_pos = Vector3(+0.75f, -0.05f, 1.28f);
					totte->transform->local_scale = Vector3(0.2f) / object->transform->local_scale;
				}
				else {
					totte->transform->local_pos = Vector3(-0.75f, -0.05f, 1.28f);
					totte->transform->local_scale = Vector3(0.2f) / object->transform->local_scale;
				}
			}
		}

		door->physics_data.inertial_mass = 20;
		door->physics_data.is_moveable = true;

		Vector3 joint_pos[2] = { scale, scale };
		if (is_left) { joint_pos[0].x *= -1; joint_pos[1].x *= -1; }

		// 変な方向に動きにくくするために大きめにとる
		joint_pos[0].y *= +5;
		joint_pos[1].y *= -5;

		Joint::add_Hingejoint(coll, door,
			vector3_quatrotate(joint_pos[0], door->transform->local_orient) + door->transform->local_pos,
			vector3_quatrotate(joint_pos[1], door->transform->local_orient) + door->transform->local_pos,
			joint_pos[0], joint_pos[1],
			0.05f
		);

		if (pearent != nullptr)pearent->add_child(door_joint);

		return door_joint;
	}

	Gameobject* Stage_demo_02::set_buttan(const Vector3& pos, const Vector3& scale, const Vector3& rotate, Stage_parts::Stageparts_tagbit tag, Gameobject* pearent, bool is_use_trigger) {
		auto coll = set_box(pos, scale, rotate, Vector3(1, 0, 0), pearent);
		auto go = coll->gameobject;
		auto buttan_comp = go->addComponent<Stage_parts::Buttan>();

		coll->tag &= ~Collider_tags::Caera_not_sunk_Stage;
		coll->physics_data.is_hitable = false;

		go->transform->local_scale.y += 0.2f;

		buttan_comp->tag = tag;
		buttan_comp->this_stage = this;
		buttan_comp->this_coll = coll;
		buttan_comp->base_scale_y = scale.y;
		buttan_comp->base_material_color = Vector3(1, 0, 0);
		buttan_comp->is_use_trigger = is_use_trigger;

		return go;
	}

	Gameobject* Stage_demo_02::set_gimmickdoor(
		const Vector3& start_pos, const Vector3& goal_pos,
		const Vector3& start_rot, const Vector3& goal_rot,
		float pos_speed, float rot_speed,
		const Vector3& scale,
		Stage_parts::Stageparts_tagbit tag,
		const Vector3& color,
		Gameobject* pearent
	) {

		auto coll = set_box(start_pos, scale, start_rot, color, pearent);
		auto go = coll->gameobject;
		auto door_comp = go->addComponent<Stage_parts::Door>();

		coll->physics_data.inertial_mass = 10000;
		coll->physics_data.is_moveable = true;
		coll->physics_data.is_fallable = false;
		coll->physics_data.is_kinmatic_linear = false;
		coll->physics_data.is_kinmatic_anglar = false;

		door_comp->tag = tag;
		door_comp->this_stage = this;
		door_comp->this_coll = coll;
		door_comp->start_pos = start_pos;
		door_comp->goal_pos = goal_pos;
		door_comp->start_rot = start_rot;
		door_comp->goal_rot = goal_rot;
		door_comp->pos_speed = pos_speed;
		door_comp->rot_speed = rot_speed;

		return go;
	}

	// inが二つとも立っているときoutのtagを立てる
	Gameobject* Stage_demo_02::set_Andcircuit(Stage_parts::Stageparts_tagbit in_tag0, Stage_parts::Stageparts_tagbit in_tag1, Stage_parts::Stageparts_tagbit out_tag, Gameobject* pearent) {
		Gameobject* go = Gameobject_manager::create("Andcircuit");
		auto comp = go->addComponent<Stage_parts::Tag_Andcicuit>();
		comp->in_tag0 = in_tag0;
		comp->in_tag1 = in_tag1;
		comp->out_tag = out_tag;

		comp->this_stage = this;

		if (pearent != nullptr)pearent->add_child(go);

		return go;
	}

	Gameobject* Stage_demo_02::set_player_statue(const Vector3& pos, const float& scale, const Vector3& rotate, Gameobject* pearent) {
		Gameobject* Human = Gameobject_manager::create("Human");
		Human->transform->local_pos = pos;
		Human->transform->local_orient = quaternion_from_euler(rotate);

		//::: Gameobjectの生成 :::
		Gameobject* Head = Gameobject_manager::createCube("Head");

		Gameobject* Lsholder = Gameobject_manager::createCube("Lsholder");
		Gameobject* Lelbow = Gameobject_manager::createCube("Lelbow");
		Gameobject* Rsholder = Gameobject_manager::createCube("Rsholder");
		Gameobject* Relbow = Gameobject_manager::createCube("Relbow");
		Gameobject* Body = Gameobject_manager::createCube("Body");
		Gameobject* Waist = Gameobject_manager::createCube("Waist");
		Gameobject* Rleg = Gameobject_manager::createCube("Rleg");
		Gameobject* Rfoot = Gameobject_manager::createCube("Rfoot");
		Gameobject* Lleg = Gameobject_manager::createCube("Lleg");
		Gameobject* Lfoot = Gameobject_manager::createCube("Lfoot");

		//Head->renderer->Load_PS("./DefaultShader/default_ps_noshadow.cso");
		//Lsholder->renderer->Load_PS("./DefaultShader/default_ps_noshadow.cso");
		//Lelbow->renderer->Load_PS("./DefaultShader/default_ps_noshadow.cso");
		//Rsholder->renderer->Load_PS("./DefaultShader/default_ps_noshadow.cso");
		//Relbow->renderer->Load_PS("./DefaultShader/default_ps_noshadow.cso");
		//Body->renderer->Load_PS("./DefaultShader/default_ps_noshadow.cso");
		//Waist->renderer->Load_PS("./DefaultShader/default_ps_noshadow.cso");
		//Rleg->renderer->Load_PS("./DefaultShader/default_ps_noshadow.cso");
		//Rfoot->renderer->Load_PS("./DefaultShader/default_ps_noshadow.cso");
		//Lleg->renderer->Load_PS("./DefaultShader/default_ps_noshadow.cso");
		//Lfoot->renderer->Load_PS("./DefaultShader/default_ps_noshadow.cso");

		//::: collider,shapeのアタッチ :::
		Collider* Head_collider = Head->addComponent<Collider>();
		Collider* Lsholder_collider = Lsholder->addComponent<Collider>();
		Collider* Lelbow_collider = Lelbow->addComponent<Collider>();
		Collider* Rsholder_collider = Rsholder->addComponent<Collider>();
		Collider* Relbow_collider = Relbow->addComponent<Collider>();
		Collider* Body_collider = Body->addComponent<Collider>();
		Collider* Waist_collider = Waist->addComponent<Collider>();
		Collider* Rleg_collider = Rleg->addComponent<Collider>();
		Collider* Rfoot_collider = Rfoot->addComponent<Collider>();
		Collider* Lleg_collider = Lleg->addComponent<Collider>();
		Collider* Lfoot_collider = Lfoot->addComponent<Collider>();


		Sphere* Head_shape = Head_collider->add_shape<Sphere>();
		Sphere* Lsholder_shape = Lsholder_collider->add_shape<Sphere>();
		Sphere* Lelbow_shape = Lelbow_collider->add_shape<Sphere>();
		Sphere* Rsholder_shape = Rsholder_collider->add_shape<Sphere>();
		Sphere* Relbow_shape = Relbow_collider->add_shape<Sphere>();
		Box* Body_shape = Body_collider->add_shape<Box>();
		Box* Waist_shape = Waist_collider->add_shape<Box>();
		Sphere* Rleg_shape = Rleg_collider->add_shape<Sphere>();
		Sphere* Rfoot_shape = Rfoot_collider->add_shape<Sphere>();
		Sphere* Lleg_shape = Lleg_collider->add_shape<Sphere>();
		Sphere* Lfoot_shape = Lfoot_collider->add_shape<Sphere>();

		{
			//::: 親子関係の設定 :::
			Human->add_child(Body);

			Human->add_child(Head);

			Human->add_child(Lsholder);
			Human->add_child(Lelbow);
			Human->add_child(Rsholder);
			Human->add_child(Relbow);

			Human->add_child(Waist);

			Human->add_child(Rleg);
			Human->add_child(Rfoot);
			Human->add_child(Lleg);
			Human->add_child(Lfoot);

			//::: gameobjectの座標設定 :::
			Vector3 head_size = Vector3(1, 1, 1);
			Vector3 body_size = Vector3(0.8f, 0.6f, 0.8f);
			Vector3 Waist_size = Vector3(0.81f, 0.7f, 0.81f);
			{
				{
					Head->transform->local_scale = head_size * scale;
					Head->transform->local_pos = Vector3(0, body_size.y + head_size.y + 0.2f, 0) * scale;
				}
			}

			{
				{
					auto& GO = Body;
					GO->transform->local_scale = body_size * scale;
					GO->transform->local_orient = quaternion_from_euler(0, 0, 0);
					GO->transform->local_pos = Vector3(0, 0, 0) * scale;
				}
				{
					auto& GO = Waist;
					GO->transform->local_scale = Waist_size * scale;
					GO->transform->local_pos = Vector3(0, -0.7f * 2, 0) * scale;
				}
			}

			Vector3 sholder_size = Vector3(0.4f, 0.5f, 0.4f);
			Vector3 arm_size = Vector3(0.4f, 0.5f, 0.4f);
			Vector3 hand_size = Vector3(0.45f, 0.45f, 0.45f);
			float arm_y_pos = 0.18f;
			{
				{
					auto& GO = Lsholder;
					GO->transform->local_scale = sholder_size * scale;
					GO->transform->local_orient = quaternion_from_euler(0, 0, +90);
					GO->transform->local_pos = Vector3(+(body_size.x + arm_size.x * 2), arm_y_pos, 0) * scale;
				}
				{
					auto& GO = Lelbow;
					GO->transform->local_scale = arm_size * scale;
					GO->transform->local_orient = quaternion_from_euler(0, 0, +90);
					GO->transform->local_pos = Vector3(+(body_size.x + arm_size.x * 5), arm_y_pos, 0) * scale;
				}
				{
					auto& GO = Rsholder;
					GO->transform->local_scale = sholder_size * scale;
					GO->transform->local_orient = quaternion_from_euler(0, 0, -90);
					GO->transform->local_pos = Vector3(-(body_size.x + arm_size.x * 2), arm_y_pos, 0) * scale;
				}
				{
					auto& GO = Relbow;
					GO->transform->local_scale = arm_size * scale;
					GO->transform->local_orient = quaternion_from_euler(0, 0, -90);
					GO->transform->local_pos = Vector3(-(body_size.x + arm_size.x * 5), arm_y_pos, 0) * scale;
				}
			}

			Vector3 Leg_size = Vector3(0.3f, 0.15, 0.3f);
			Vector3 Foot_size = Vector3(0.4f, 0.25, 0.4f);
			float leg_x_pos = 0.6f;
			{
				{
					auto& GO = Rleg;
					GO->transform->local_scale = Foot_size * scale;
					GO->transform->local_orient = quaternion_from_euler(0, 0, 0);
					GO->transform->local_pos = Vector3(-leg_x_pos, -(body_size.y + Waist_size.y * 2 + Foot_size.y * 2), 0) * scale;
				}
				{
					auto& GO = Rfoot;
					GO->transform->local_scale = Foot_size * scale;
					GO->transform->local_orient = quaternion_from_euler(0, 0, 0);
					GO->transform->local_pos = Vector3(-leg_x_pos, -(body_size.y + Waist_size.y * 2 + Foot_size.y * 5), 0) * scale;
				}
				{
					auto& GO = Lleg;
					GO->transform->local_scale = Foot_size * scale;
					GO->transform->local_orient = quaternion_from_euler(0, 0, 0);
					GO->transform->local_pos = Vector3(+leg_x_pos, -(body_size.y + Waist_size.y * 2 + Foot_size.y * 2), 0) * scale;
				}
				{
					auto& GO = Lfoot;
					GO->transform->local_scale = Foot_size * scale;
					GO->transform->local_orient = quaternion_from_euler(0, 0, 0);
					GO->transform->local_pos = Vector3(+leg_x_pos, -(body_size.y + Waist_size.y * 2 + Foot_size.y * 5), 0) * scale;
				}
			}

			//::: 慣性モーメントの調整 :::
			Head_collider->set_barycenter(Vector3(0, +0.5f, 0) * scale);
			Body_collider->set_barycenter(Vector3(0, +0.5f, 0) * scale);

			Lsholder_collider->set_barycenter(Vector3(0, 0.5f, 0) * scale);
			Rsholder_collider->set_barycenter(Vector3(0, 0.5f, 0) * scale);
			Lelbow_collider->set_barycenter(Vector3(0, 0.5f, 0) * scale);
			Relbow_collider->set_barycenter(Vector3(0, 0.5f, 0) * scale);

			Lleg_collider->set_barycenter(Vector3(0, 1, 0) * scale);
			Rleg_collider->set_barycenter(Vector3(0, 1, 0) * scale);
			Lfoot_collider->set_barycenter(Vector3(0, 1, 0) * scale);
			Rfoot_collider->set_barycenter(Vector3(0, 1, 0) * scale);


			//::: 質量の調整 :::
			Head_collider->physics_data.inertial_mass = 3;
			Lsholder_collider->physics_data.inertial_mass = 4;
			Rsholder_collider->physics_data.inertial_mass = 4;
			Lelbow_collider->physics_data.inertial_mass = 3;
			Relbow_collider->physics_data.inertial_mass = 3;
			Body_collider->physics_data.inertial_mass = 5;
			Waist_collider->physics_data.inertial_mass = 4;
			Lleg_collider->physics_data.inertial_mass = 2;
			Rleg_collider->physics_data.inertial_mass = 2;
			Lfoot_collider->physics_data.inertial_mass = 1;
			Rfoot_collider->physics_data.inertial_mass = 1;

			Lsholder_collider->physics_data.anglar_drag = 0.5f;
			Lelbow_collider->physics_data.anglar_drag = 0.5f;
			Rsholder_collider->physics_data.anglar_drag = 0.5f;
			Relbow_collider->physics_data.anglar_drag = 0.5f;
			Waist_collider->physics_data.anglar_drag = 0.8f;

			//::: 重力適用の調整 :::
			//

			//::: tagの設定 :::
			auto tag = Collider_tags::Stage | Collider_tags::Caera_not_sunk_Stage | Collider_tags::Kinematic_Stage;
			Head_collider->tag = tag;
			Lsholder_collider->tag = tag;
			Lelbow_collider->tag = tag;
			Rsholder_collider->tag = tag;
			Relbow_collider->tag = tag;
			Body_collider->tag = tag;
			Waist_collider->tag = tag;
			Rleg_collider->tag = tag;
			Rfoot_collider->tag = tag;
			Lleg_collider->tag = tag;
			Lfoot_collider->tag = tag;

			//::: sphereの調整 :::
			Lleg_shape->r = Lleg_collider->transform->local_scale.y / Lleg_collider->transform->local_scale.x;
			Lfoot_shape->r = Lfoot_collider->transform->local_scale.y / Lfoot_collider->transform->local_scale.x;
			Rleg_shape->r = Rleg_collider->transform->local_scale.y / Rleg_collider->transform->local_scale.x;
			Rfoot_shape->r = Rfoot_collider->transform->local_scale.y / Rfoot_collider->transform->local_scale.x;


			//::: Jointの設定
			float joint_bias = 0.1f;
			//頭
			{
				auto Ball = Joint::add_Conejoint(Head_collider, Body_collider, Vector3(0, -1, 0) * scale, Vector3(0, 0.8f, 0) * scale, Vector3(0, 1, 0), Vector3(0, 1, 0), joint_bias);
				Ball->limit = 30;

				auto Twist = Joint::add_Twistjoint(Body_collider, Head_collider, Vector3(0, 1, 0), Vector3(0, 1, 0), joint_bias);
				Twist->limit = Vector2(360 - 90, 90);
			}
			//腕
			{
				auto Cone = Joint::add_Conejoint(Body_collider, Lsholder_collider, Vector3(+1.1f, 0.2f, 0) * scale, Vector3(0, arm_size.y, 0) * scale, Vector3(-1, 0, -1).unit_vect(), Vector3(0, -1, 0).unit_vect(), joint_bias);
				Cone->limit = 85;

				auto Twist = Joint::add_Twistjoint(Body_collider, Lsholder_collider, Vector3(0, 1, 0), Vector3(0, 1, 0), joint_bias);
				Twist->limit = Vector2(360 - 90, 90);
			}
			{
				auto hinge = Joint::add_Hingejoint(Lsholder_collider, Lelbow_collider, Vector3(-1, -(arm_size.y + 0.1f), 0) * scale, Vector3(+1, -(arm_size.y + 0.1f), 0) * scale, Vector3(-1, (arm_size.y + 0.1f), 0) * scale, Vector3(+1, (arm_size.y + 0.1f), 0) * scale, joint_bias);
				hinge->shrink_bias = 0.5f;
				hinge->stretch_bias = 0.5f;
				hinge->limit = Vector2(230, 360);
			}
			{
				auto Cone = Joint::add_Conejoint(Body_collider, Rsholder_collider, Vector3(-1.1f, 0.2f, 0) * scale, Vector3(0, arm_size.y, 0) * scale, Vector3(+1, 0, -1).unit_vect(), Vector3(0, -1, 0).unit_vect(), joint_bias);
				Cone->limit = 85;

				auto Twist = Joint::add_Twistjoint(Body_collider, Rsholder_collider, Vector3(0, 1, 0), Vector3(0, 1, 0), joint_bias);
				Twist->limit = Vector2(360 - 90, 90);
			}
			{
				auto hinge = Joint::add_Hingejoint(Rsholder_collider, Relbow_collider, Vector3(-1, -(arm_size.y + 0.1f), 0) * scale, Vector3(+1, -(arm_size.y + 0.1f), 0) * scale, Vector3(-1, (arm_size.y + 0.1f), 0) * scale, Vector3(+1, (arm_size.y + 0.1f), 0) * scale, joint_bias);
				hinge->shrink_bias = 0.5f;
				hinge->stretch_bias = 0.5f;
				hinge->limit = Vector2(230, 360);
			}
			//腰
			{
				auto Cone = Joint::add_Conejoint(Body_collider, Waist_collider, Vector3(0, -1.0f, 0) * scale, Vector3(0, 0.5f, 0) * scale, Vector3(0, 1, 0).unit_vect(), Vector3(0, 1.5f, -1).unit_vect(), 0.1f);
				Cone->limit = 60;

				auto Twist = Joint::add_Twistjoint(Body_collider, Waist_collider, Vector3(0, 1, 0), Vector3(0, 1, 0), joint_bias);
				Twist->limit = Vector2(360 - 30, 30);
			}
			//足
			{
				auto Cone = Joint::add_Conejoint(Waist_collider, Rleg_collider, Vector3(-0.6f, -0.8f, 0) * scale, Vector3(0, 0.3, 0) * scale, Vector3(0, -1, -1.02f).unit_vect(), Vector3(0, -1, 0).unit_vect(), joint_bias);
				Cone->limit = 80;

				auto Twist = Joint::add_Twistjoint(Waist_collider, Rleg_collider, Vector3(0, 1, 0), Vector3(0, 1, 0), joint_bias);
				Twist->limit = Vector2(360 - 5, 15);
			}
			{
				auto hinge = Joint::add_Hingejoint(Rleg_collider, Rfoot_collider, Vector3(-1, -0.4f, 0) * scale, Vector3(+1, -0.3f, 0) * scale, Vector3(-1, +0.3f, 0) * scale, Vector3(+1, +0.3f, 0) * scale, joint_bias);
				hinge->limit = Vector2(0, 60);
			}
			{
				auto Cone = Joint::add_Conejoint(Waist_collider, Lleg_collider, Vector3(+0.6f, -0.8f, 0) * scale, Vector3(0, 0.3f, 0) * scale, Vector3(0, -1, -1.02f).unit_vect(), Vector3(0, -1, 0).unit_vect(), joint_bias);
				Cone->limit = 80;

				auto Twist = Joint::add_Twistjoint(Waist_collider, Lleg_collider, Vector3(0, 1, 0), Vector3(0, 1, 0), joint_bias);
				Twist->limit = Vector2(360 - 15, 5);
			}
			{
				auto hinge = Joint::add_Hingejoint(Lleg_collider, Lfoot_collider, Vector3(-1, -0.4f, 0) * scale, Vector3(+1, -0.3f, 0) * scale, Vector3(-1, +0.3f, 0) * scale, Vector3(+1, +0.3f, 0) * scale, joint_bias);
				hinge->limit = Vector2(0, 60);
			}
		}

		//顔とかベルトを着けてみる
		{
			//Vector4 face_color = Vector4(0.5f, 0.5f, 0.5f, 1);
			//{
			//	Gameobject* eye0 = Gameobject_manager::createSphere("eye0");
			//	Head->add_child(eye0);
			//	eye0->transform->local_pos = Vector3(+0.5f, 0.5f, -1);
			//	eye0->transform->local_scale = Vector3(0.25f, 0.25f, 0.25f);
			//	eye0->renderer->color = face_color;
			//	eye0->renderer->Load_PS("./DefaultShader/default_ps_noshadow.cso");
			//	//stage_parts.emplace_back(eye0);
			//}
			//{
			//	Gameobject* eye1 = Gameobject_manager::createSphere("eye1");
			//	Head->add_child(eye1);
			//	eye1->transform->local_pos = Vector3(-0.5f, 0.5f, -1);
			//	eye1->transform->local_scale = Vector3(0.25f, 0.25f, 0.25f);
			//	eye1->renderer->color = face_color;
			//	eye1->renderer->Load_PS("./DefaultShader/default_ps_noshadow.cso");
			//	//stage_parts.emplace_back(eye1);
			//}
			//{
			//	Gameobject* mouth = Gameobject_manager::createCube("mouth");
			//	Head->add_child(mouth);
			//	mouth->transform->local_pos = Vector3(0, -0.45f, -1);
			//	mouth->transform->local_scale = Vector3(0.7f, 0.25f, 0.3f);
			//	mouth->renderer->color = face_color;
			//	mouth->renderer->Load_PS("./DefaultShader/default_ps_noshadow.cso");
			//	//stage_parts.emplace_back(mouth);
			//}

			//{
			//	Gameobject* belt = Gameobject_manager::createCube("belt");
			//	Waist->add_child(belt);
			//	belt->transform->local_pos = Vector3(0, -0.45f, 0);
			//	belt->transform->local_scale = Vector3(1.1, 0.25f, 1.1);
			//	belt->renderer->color = face_color;
			//	belt->renderer->Load_PS("./DefaultShader/default_ps_noshadow.cso");
			//	//stage_parts.emplace_back(belt);
			//}
		}

		// playerが触れたときにmoveableをtrueにする
		{
			auto comp = Human->addComponent<Stage_parts::Touch_moveable>();
			comp->tag = Collider_tags::Human;
			comp->move_colliders.emplace_back(Head_collider);
			comp->move_colliders.emplace_back(Lsholder_collider);
			comp->move_colliders.emplace_back(Rsholder_collider);
			comp->move_colliders.emplace_back(Lelbow_collider);
			comp->move_colliders.emplace_back(Relbow_collider);
			comp->move_colliders.emplace_back(Body_collider);
			comp->move_colliders.emplace_back(Waist_collider);
			comp->move_colliders.emplace_back(Lleg_collider);
			comp->move_colliders.emplace_back(Rleg_collider);
			comp->move_colliders.emplace_back(Lfoot_collider);
			comp->move_colliders.emplace_back(Rfoot_collider);
			comp->judge_colliders.emplace_back(Head_collider);
			comp->judge_colliders.emplace_back(Lsholder_collider);
			comp->judge_colliders.emplace_back(Rsholder_collider);
			comp->judge_colliders.emplace_back(Lelbow_collider);
			comp->judge_colliders.emplace_back(Relbow_collider);
			comp->judge_colliders.emplace_back(Body_collider);
			comp->judge_colliders.emplace_back(Waist_collider);
			comp->judge_colliders.emplace_back(Lleg_collider);
			comp->judge_colliders.emplace_back(Rleg_collider);
			comp->judge_colliders.emplace_back(Lfoot_collider);
			comp->judge_colliders.emplace_back(Rfoot_collider);

			Head_collider->physics_data.is_moveable = false;
			Lsholder_collider->physics_data.is_moveable = false;
			Rsholder_collider->physics_data.is_moveable = false;
			Lelbow_collider->physics_data.is_moveable = false;
			Relbow_collider->physics_data.is_moveable = false;
			Body_collider->physics_data.is_moveable = false;
			Waist_collider->physics_data.is_moveable = false;
			Lleg_collider->physics_data.is_moveable = false;
			Rleg_collider->physics_data.is_moveable = false;
			Lfoot_collider->physics_data.is_moveable = false;
			Rfoot_collider->physics_data.is_moveable = false;
		}

		// ポーズをつける
		{
			Relbow->transform->local_pos = Vector3(-2.63f, 3.59f, -1.07f);
			Relbow->transform->local_orient = quaternion_from_euler(1.19f, -91.03f, -161.72f);

			Rsholder->transform->local_pos = Vector3(-2.39f, 1.33f, -0.31f);
			Rsholder->transform->local_orient = quaternion_from_euler(10.77f, -87.81f, -161.38f);

			Lelbow->transform->local_pos = Vector3(2.62f, 3.61f, -1.03f);
			Lelbow->transform->local_orient = quaternion_from_euler(3.46f, -101.18f, -164.07f);

			Lsholder->transform->local_pos = Vector3(2.39f, 1.34f, -0.31f);
			Lsholder->transform->local_orient = quaternion_from_euler(13.95f, 98.08f, 163.60f);
		}

		{
			auto comp = Waist->addComponent<Stage_parts::Kinematic_block>();
			comp->this_stage = this;
			comp->this_coll = Waist_collider;
			comp->pearent = Human;
			comp->respown_pos = Vector3(0);
			comp->respown_rotate = quaternion_identity();
		}

		if (pearent != nullptr)pearent->add_child(Human);

		return Human;
	}




	Gameobject* Stage_demo_02::set_sphere_rope(const Vector3& pos, const float& scale, const Vector3& rotate, int quantity, float mass, const Vector3& color) {

		Gameobject* rope_pearent = Gameobject_manager::create("sphere_rope");
		rope_pearent->transform->local_orient = quaternion_from_euler(rotate + Vector3(0.1f, 0, 0.1f));

		Collider* old_coll = nullptr;
		Gameobject* old_go = nullptr;
		{
			auto coll = set_sphere(pos, scale, color);
			coll->tag &= ~Collider_tags::Caera_not_sunk_Stage;

			rope_pearent->add_child(coll->gameobject);

			old_coll = coll;
			old_go = coll->gameobject;
		}

		for (int i = 0; i < quantity; i++) {

			Gameobject* go = nullptr;
			auto coll = set_sphere(pos - Vector3(0, scale * (i + 1) * 2, 0), scale, color);
			coll->tag &= ~Collider_tags::Caera_not_sunk_Stage;
			coll->physics_data.inertial_mass = mass * powf(1.1f, quantity - i);

			rope_pearent->add_child(coll->gameobject);

			if (old_coll != nullptr) Joint::add_balljoint(old_coll, coll, Vector3(0, -scale * 1.05f, 0), Vector3(0, scale * 1.05f, 0), 0.1f);

			old_coll = coll;
			old_go = go;
		}

		return rope_pearent;

	}

	Gameobject* Stage_demo_02::set_desk(const Vector3& pos, const Vector3& scale, const Vector3& rotate, float mass) {

		// deskの生成とcolliderのアタッチ
		Gameobject* desk = nullptr;
		desk = Gameobject_manager::create("Desk", GO_tag::Sphere);
		desk->transform->local_pos = pos;
		desk->transform->local_scale = scale;
		desk->transform->local_orient = quaternion_from_euler(rotate);

		Collider* coll = desk->addComponent<Collider>();
		coll->tag = Collider_tags::Stage | Collider_tags::Kinematic_Stage;
		if (mass == 0)coll->physics_data.is_moveable = false;
		else coll->physics_data.inertial_mass = mass;

		{
			Box* box[5] = { nullptr };
			box[0] = coll->add_shape<Box>();
			box[1] = coll->add_shape<Box>();
			box[2] = coll->add_shape<Box>();
			box[3] = coll->add_shape<Box>();
			box[4] = coll->add_shape<Box>();

			box[0]->center = Vector3(0, 0.75f, 0);
			box[0]->size = Vector3(4, 0.5f, 3);

			box[1]->center = Vector3(+2.8f, -0.75f, +1.8f);
			box[1]->size = Vector3(0.5f, 1, 0.5f);
			box[2]->center = Vector3(+2.8f, -0.75f, -1.8f);
			box[2]->size = Vector3(0.5f, 1, 0.5f);
			box[3]->center = Vector3(-2.8f, -0.75f, +1.8f);
			box[3]->size = Vector3(0.5f, 1, 0.5f);
			box[4]->center = Vector3(-2.8f, -0.75f, -1.8f);
			box[4]->size = Vector3(0.5f, 1, 0.5f);
		}


		// darumaが描画されるようにGOをdarumaにつける
		Gameobject* parts[5] = { nullptr };
		parts[0] = Gameobject_manager::createCube();
		parts[1] = Gameobject_manager::createCube();
		parts[2] = Gameobject_manager::createCube();
		parts[3] = Gameobject_manager::createCube();
		parts[4] = Gameobject_manager::createCube();
		Vector4 C = Vector4(207, 171, 142, 255) / 255;
		//parts[0]->renderer->color = Vector4(238, 229, 224, 255) / 255;
		////parts[1]->renderer->color = Vector4(207, 171, 142, 255) / 255;
		//parts[1]->renderer->color = Vector4(255, 77, 17, 255) / 255;

		parts[0]->renderer->color = C;
		parts[1]->renderer->color = C;
		parts[2]->renderer->color = C;
		parts[3]->renderer->color = C;
		parts[4]->renderer->color = C;

		parts[0]->transform->local_pos = Vector3(0, 0.75f, 0);
		parts[0]->transform->local_scale = Vector3(4, 0.5f, 3);

		parts[1]->transform->local_pos = Vector3(+2.8f, -0.75f, +1.8f);
		parts[1]->transform->local_scale = Vector3(0.5f, 1, 0.5f);
		parts[2]->transform->local_pos = Vector3(+2.8f, -0.75f, -1.8f);
		parts[2]->transform->local_scale = Vector3(0.5f, 1, 0.5f);
		parts[3]->transform->local_pos = Vector3(-2.8f, -0.75f, +1.8f);
		parts[3]->transform->local_scale = Vector3(0.5f, 1, 0.5f);
		parts[4]->transform->local_pos = Vector3(-2.8f, -0.75f, -1.8f);
		parts[4]->transform->local_scale = Vector3(0.5f, 1, 0.5f);


		//coll->tag = Collider_tags::Sphere;

		desk->add_child(parts[0]);
		desk->add_child(parts[1]);
		desk->add_child(parts[2]);
		desk->add_child(parts[3]);
		desk->add_child(parts[4]);


		//stage_parts.emplace_back(desk);
		//stage_parts.emplace_back(parts[0]);
		//stage_parts.emplace_back(parts[1]);
		//stage_parts.emplace_back(parts[2]);
		//stage_parts.emplace_back(parts[3]);
		//stage_parts.emplace_back(parts[4]);

		return desk;
	}

	Gameobject* Stage_demo_02::set_gear(const Vector3& pos, const Vector3& size, const Vector3& rotate, float mass, int tooth_count) {

		Gameobject* GEAR = Gameobject_manager::create("GEAR");
		GEAR->transform->local_pos = pos;
		GEAR->transform->local_orient = quaternion_from_euler(rotate);

		Collider* coll = GEAR->addComponent<Collider>();
		//coll->tag |= Collider_tags::Stage | Collider_tags::Kinematic_Stage ;
		coll->tag |= Collider_tags::Stage | Collider_tags::Kinematic_Stage | Collider_tags::Caera_not_sunk_Stage;
		coll->physics_data.inertial_mass = mass;
		coll->physics_data.anglar_drag = 0;
		coll->ignore_tags = Collider_tags::Static_Stage;

		for (int gear_tooth_num = 0; gear_tooth_num < tooth_count; gear_tooth_num++) {
			Gameobject* GO = Gameobject_manager::createCube("tooth");
			GO->transform->local_scale = size;
			GO->transform->local_orient = quaternion_from_euler(0, 0, 360.0f / tooth_count * gear_tooth_num);
			GO->transform->local_pos = Vector3(0, size.y * 2, 0);
			GO->transform->local_pos = vector3_quatrotate(GO->transform->local_pos, quaternion_axis_angle(Vector3(0, 0, 1), 360.0f / tooth_count * gear_tooth_num));

			GO->renderer->color = Vector4(1, 1, 0, 1);
			GEAR->add_child(GO);
			//stage_parts.emplace_back(GO);
		}

		for (int gear_tooth_num = 0; gear_tooth_num < tooth_count; gear_tooth_num++) {
			Box* box = coll->add_shape<Box>();
			box->size = size;
			box->rotate = Vector3(0, 0, 360.0f / tooth_count * gear_tooth_num);
			box->center = Vector3(0, size.y * 2, 0);
			box->center = vector3_quatrotate(box->center, quaternion_axis_angle(Vector3(0, 0, 1), 360.0f / tooth_count * gear_tooth_num));

		}


		Gameobject* gear_joint = Gameobject_manager::createCube("gear_joint");
		gear_joint->transform->local_pos = pos;
		gear_joint->transform->local_orient = quaternion_from_euler(rotate);;
		Collider* gear_joint_collider = gear_joint->addComponent<Collider>();
		gear_joint_collider->physics_data.is_moveable = false;

		Joint::add_Hingejoint(gear_joint_collider, coll, Vector3(0, 0, +10), Vector3(0, 0, -10), Vector3(0, 0, +10), Vector3(0, 0, -10), 0.1f, 0.1f);

		//stage_parts.emplace_back(gear_joint);
		//stage_parts.emplace_back(GEAR);

		return GEAR;

	}
}