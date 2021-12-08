#include "stage_demo.h"

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

namespace Adollib
{
	Gameobject* Stage_demo::set_tree(const Vector3& pos, const Vector3& scale, const Vector3& rotate, Gameobject* pearent) {
		Gameobject* tree = nullptr;
		tree = Gameobject_manager::create("tree",GO_tag::None);

		Vector4 stem_color = Vector4(90, 47, 27, 255) / 255.0f;
		Vector4 reaf_color = Vector4(48, 188, 0, 255) / 255.0f;
		Vector4 floar_color = Vector4(188, 214, 54, 255) / 255.0f;

		tree->transform->local_pos = pos;
		tree->transform->local_scale = scale;
		tree->transform->local_orient = quaternion_from_euler(rotate);

		const int Tree_size = 7;
		Gameobject* tree_parts[Tree_size] = { nullptr };
		for (int i = 0; i < Tree_size; i++) {
			tree_parts[i] = Gameobject_manager::createCube("tree_part",GO_tag::Box);
		}

		tree_parts[0]->transform->local_pos = Vector3(0, 7, 0);
		tree_parts[0]->transform->local_scale = Vector3(1, 6.9f, 1);
		tree_parts[0]->renderer->color = stem_color;

		tree_parts[1]->transform->local_pos = Vector3(0, 15, 0);
		tree_parts[1]->transform->local_scale = Vector3(1.5, 1, 1.5);
		tree_parts[1]->transform->local_orient = quaternion_from_euler(0, 0, 0);
		tree_parts[1]->renderer->color = reaf_color;

		tree_parts[2]->transform->local_pos = Vector3(0, 13, 0);
		tree_parts[2]->transform->local_scale = Vector3(2.0, 1, 2.0);
		tree_parts[2]->transform->local_orient = quaternion_from_euler(0, 1.7f, 0);
		tree_parts[2]->renderer->color = reaf_color;

		tree_parts[3]->transform->local_pos = Vector3(0, 11, 0);
		tree_parts[3]->transform->local_scale = Vector3(3.0, 1, 3.0);
		tree_parts[3]->transform->local_orient = quaternion_from_euler(0, 13, 0);
		tree_parts[3]->renderer->color = reaf_color;

		tree_parts[4]->transform->local_pos = Vector3(0, 9, 0);
		tree_parts[4]->transform->local_scale = Vector3(3.5, 1, 3.5);
		tree_parts[4]->transform->local_orient = quaternion_from_euler(0, 28, 0);
		tree_parts[4]->renderer->color = reaf_color;

		tree_parts[5]->transform->local_pos = Vector3(0, 7, 0);
		tree_parts[5]->transform->local_scale = Vector3(4.5, 1, 4.5);
		tree_parts[5]->transform->local_orient = quaternion_from_euler(0, 14, 0);
		tree_parts[5]->renderer->color = reaf_color;

		tree_parts[6]->transform->local_pos = Vector3(0, 0.2f, 0);
		tree_parts[6]->transform->local_scale = Vector3(2, 0.2f, 2);
		tree_parts[6]->transform->local_orient = quaternion_from_euler(0, 0, 0);
		tree_parts[6]->renderer->color = floar_color;

		for (int i = 0; i < Tree_size; i++) {
			tree->add_child(tree_parts[i]);
		}

		Box* boxes[Tree_size];
		Collider* coll = tree->addComponent<Collider>();
		coll->physics_data.inertial_mass = 35;
		//coll->tag = Collider_tags::Stage | Collider_tags::Kinematic_Stage | Collider_tags::Jumpable_Stage;
		coll->tag = Collider_tags::Stage | Collider_tags::Kinematic_Stage | Collider_tags::Caera_not_sunk_Stage;

		for (int i = 0; i < Tree_size; i++) {
			boxes[i] = coll->add_shape<Box>();
		}

		boxes[0]->center = Vector3(0, 7, 0);
		boxes[0]->size = Vector3(1, 7, 1);

		boxes[1]->center = Vector3(0, 15, 0);
		boxes[1]->size = Vector3(1.5f, 1, 1.5f);
		boxes[1]->rotate = Vector3(0, 0, 0);

		boxes[2]->center = Vector3(0, 13, 0);
		boxes[2]->size = Vector3(2.0f, 1, 2.0f);
		boxes[2]->rotate = Vector3(0, 1.7f, 0);

		boxes[3]->center = Vector3(0, 11, 0);
		boxes[3]->size = Vector3(3.0f, 1, 3.0f);
		boxes[3]->rotate = Vector3(0, 13, 0);

		boxes[4]->center = Vector3(0, 9, 0);
		boxes[4]->size = Vector3(3.5f, 1, 3.5f);
		boxes[4]->rotate = Vector3(0, 28, 0);

		boxes[5]->center = Vector3(0, 7, 0);
		boxes[5]->size = Vector3(4.5f, 1, 4.5f);
		boxes[5]->rotate = Vector3(0, 14, 0);

		boxes[6]->center = Vector3(0, 0.2f, 0);
		boxes[6]->size = Vector3(2, 0.2f, 2);
		boxes[6]->rotate = Vector3(0, 0, 0);



		for (int i = 0; i < Tree_size; i++) {
			boxes[i]->adapt_Colliderdata();
		}
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

		if (pearent != nullptr) pearent->add_child(tree);
		return tree;
	}

	Gameobject* Stage_demo::set_buttan(const Vector3& pos, const Vector3& scale, const Vector3& rotate, Stage_parts::Stageparts_tagbit tag, const Vector3& color, Gameobject* pearent) {
		auto coll = set_box(pos, scale, rotate, color);
		auto go = coll->gameobject;
		auto buttan_comp = go->addComponent<Stage_parts::Buttan>();

		coll->tag &= ~Collider_tags::Caera_not_sunk_Stage;
		coll->physics_data.is_hitable = false;

		go->transform->local_scale.y += 0.2f;

		buttan_comp->tag = tag;
		buttan_comp->this_stage = this;
		buttan_comp->this_coll = coll;
		buttan_comp->base_scale_y = scale.y;
		buttan_comp->base_material_color = color;

		if (pearent != nullptr) pearent->add_child(go);
		return go;
	}

	Gameobject* Stage_demo::set_door(const Vector3& pos, const Vector3& scale, const Vector3& rotate, const int is_left, Gameobject* pearent) {

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

	Gameobject* Stage_demo::set_gimmickdoor(
		const Vector3& start_pos, const Vector3& goal_pos,
		const Vector3& start_rot, const Vector3& goal_rot,
		float pos_speed, float rot_speed,
		const Vector3& scale,
		Stage_parts::Stageparts_tagbit tag,
		const Vector3& color,
		Gameobject* pearent
	) {

		auto coll = set_box(start_pos, scale, start_rot, color);
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

		if (pearent != nullptr) pearent->add_child(go);
		return go;
	}

	Gameobject* Stage_demo::set_sphere_rope(const Vector3& pos, const float& scale, const Vector3& rotate, int quantity, float mass, const Vector3& color) {

		Gameobject* rope_pearent = Gameobject_manager::create("sphere_rope");
		rope_pearent->transform->local_orient = quaternion_from_euler(rotate + Vector3(0.1f, 0, 0.1f));

		stage_parts.emplace_back(rope_pearent);

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
			auto coll = set_sphere(pos - Vector3(0, scale * (i + 1) * 2, 0), scale, color, nullptr, false);
			coll->tag &= ~Collider_tags::Caera_not_sunk_Stage;
			coll->physics_data.inertial_mass = mass * powf(1.1f, quantity - i);

			rope_pearent->add_child(coll->gameobject);

			if (old_coll != nullptr) Joint::add_balljoint(old_coll, coll, Vector3(0, -1, 0), Vector3(0, 1, 0), 0.1f);

			old_coll = coll;
			old_go = go;
		}

		return rope_pearent;

	}




}