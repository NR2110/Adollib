#include "stage_01.h"

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

namespace Adollib
{
	Gameobject* Stage_01::set_tree(const Vector3& pos, const Vector3& scale, const Vector3& rotate,const float y_scale,const float tan_scale, Gameobject* pearent) {
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
		tree_parts[0]->material->color = stem_color;

		tree_parts[1]->transform->local_pos = Vector3(0, 5 + y_scale * 2 * 5, 0);
		tree_parts[1]->transform->local_scale = Vector3(1.5 * tan_scale, y_scale, 1.5 * tan_scale);
		tree_parts[1]->transform->local_orient = quaternion_from_euler(0, 0, 0);
		tree_parts[1]->material->color = reaf_color;

		tree_parts[2]->transform->local_pos = Vector3(0, 5 + y_scale * 2 * 4, 0);
		tree_parts[2]->transform->local_scale = Vector3(2.0 * tan_scale, y_scale, 2.0 * tan_scale);
		tree_parts[2]->transform->local_orient = quaternion_from_euler(0, 1.7f, 0);
		tree_parts[2]->material->color = reaf_color;

		tree_parts[3]->transform->local_pos = Vector3(0, 5 + y_scale * 2 * 3, 0);
		tree_parts[3]->transform->local_scale = Vector3(3.0 * tan_scale, y_scale, 3.0 * tan_scale);
		tree_parts[3]->transform->local_orient = quaternion_from_euler(0, 13, 0);
		tree_parts[3]->material->color = reaf_color;

		tree_parts[4]->transform->local_pos = Vector3(0, 5 + y_scale * 2 * 2, 0);
		tree_parts[4]->transform->local_scale = Vector3(3.5 * tan_scale, y_scale, 3.5 * tan_scale);
		tree_parts[4]->transform->local_orient = quaternion_from_euler(0, 28, 0);
		tree_parts[4]->material->color = reaf_color;

		tree_parts[5]->transform->local_pos = Vector3(0, 5 + y_scale * 2 * 1, 0);
		tree_parts[5]->transform->local_scale = Vector3(4.5 * tan_scale, y_scale, 4.5 * tan_scale);
		tree_parts[5]->transform->local_orient = quaternion_from_euler(0, 14, 0);
		tree_parts[5]->material->color = reaf_color;

		tree_parts[6]->transform->local_pos = Vector3(0, 0.2f, 0);
		tree_parts[6]->transform->local_scale = Vector3(3, 0.2f, 3);
		tree_parts[6]->transform->local_orient = quaternion_from_euler(0, 0, 0);
		tree_parts[6]->material->color = floar_color;

		for (int i = 0; i < Tree_size; i++) {
			tree->add_child(tree_parts[i]);
		}

		Box* boxes[Tree_size];
		Collider* coll = tree->addComponent<Collider>();
		coll->physics_data.inertial_mass = 120 * y_scale * tan_scale;
		coll->tag = Collider_tags::Stage | Collider_tags::Kinematic_Stage | Collider_tags::Caera_not_sunk_Stage ;
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

		for (int i = 0; i < Tree_size; i++) stage_parts.emplace_back(tree_parts[i]);
		stage_parts.emplace_back(tree);

		return tree;
	}

	Gameobject* Stage_01::set_pillar(const Vector3& pos, const Vector3& scale, const Vector3& rotate, Gameobject* pearent, const float base_length , const float pillar_length) {

		Gameobject* pillar_pearent = Gameobject_manager::create("pillar_pearent");

		pillar_pearent->transform->local_pos = pos;
		pillar_pearent->transform->local_orient = quaternion_from_euler(rotate);
		pillar_pearent->transform->local_scale = scale;

		set_box(Vector3(0, base_length, 0),                             Vector3(2, base_length, 2),         Vector3(0), Vector3(255, 255, 255) / 255.0f * 0.9f, pillar_pearent);
		set_box(Vector3(0, base_length * 2 + pillar_length, 0),         Vector3(1.5f, pillar_length, 1.5f), Vector3(0), Vector3(255, 255, 255) / 255.0f * 0.9f, pillar_pearent);
		set_box(Vector3(0, base_length * 2 + pillar_length * 2 + 0.5f, 0), Vector3(2, 0.5f, 2),                Vector3(0), Vector3(255, 255, 255) / 255.0f * 0.9f, pillar_pearent);

		if (pearent != nullptr)pearent->add_child(pillar_pearent);
	}

	Gameobject* Stage_01::set_fence(const Vector3& pos, const Vector3& scale, const Vector3& rotate, Gameobject* pearent, const int fence_count) {

		Gameobject* fence_pearent = Gameobject_manager::create("fence_pearent");

		fence_pearent->transform->local_pos = pos;
		fence_pearent->transform->local_orient = quaternion_from_euler(rotate);
		fence_pearent->transform->local_scale = scale;

		set_box(Vector3(0, 4, 0),     Vector3(6.5f, 0.5f, 1), Vector3(0), Vector3(255, 255, 255) / 255.0f * 0.9f, fence_pearent);

		set_box(Vector3(-4.5f, 2, 0), Vector3(0.5f, 2, 0.5f), Vector3(0), Vector3(255, 255, 255) / 255.0f * 0.9f, fence_pearent);
		set_box(Vector3(-1.5f, 2, 0), Vector3(0.5f, 2, 0.5f), Vector3(0), Vector3(255, 255, 255) / 255.0f * 0.9f, fence_pearent);
		set_box(Vector3(+1.5f, 2, 0), Vector3(0.5f, 2, 0.5f), Vector3(0), Vector3(255, 255, 255) / 255.0f * 0.9f, fence_pearent);
		set_box(Vector3(+4.5f, 2, 0), Vector3(0.5f, 2, 0.5f), Vector3(0), Vector3(255, 255, 255) / 255.0f * 0.9f, fence_pearent);

		if (pearent != nullptr)pearent->add_child(fence_pearent);
	}

	Gameobject* Stage_01::set_door(const Vector3& pos, const Vector3& scale, const Vector3& rotate, const int is_left, Gameobject* pearent) {

		// joint用に動かないcolliderを作成
		auto door_joint = Gameobject_manager::create("door_joint");
		if (pearent != nullptr)pearent->add_child(door_joint);

		auto coll = door_joint->addComponent<Collider>();
		coll->physics_data.is_moveable = false;

		Collider* door = nullptr;

		//boxを生成
		{
			Gameobject* object = nullptr;
			object = Gameobject_manager::createCube("Door", GO_tag::Box);
			object->material->color = Vector4(Vector3(90, 47, 27) / 255.0f, 1);

			object->transform->local_pos = pos;
			object->transform->local_orient = quaternion_from_euler(rotate);
			object->transform->local_scale = scale;

			door = object->addComponent<Collider>();
			auto box = door->add_shape<Box>();
			box->size = Vector3(1) * 0.9f; //突っかからないようにColliderのサイズを小さめにする

			door->tag = Collider_tags::Box | Collider_tags::Stage | Collider_tags::Caera_not_sunk_Stage | Collider_tags::Kinematic_Stage;

			stage_parts.emplace_back(object);
			if (pearent != nullptr)pearent->add_child(object);
		}

		door->physics_data.inertial_mass = 20;
		door->physics_data.is_moveable = true;

		Vector3 joint_pos[2] = { scale, scale };
		if (is_left) { joint_pos[0].x *= -1; joint_pos[1].x *= -1;}

		// 変な方向に動きにくくするために大きめにとる
		joint_pos[0].y *= +5;
		joint_pos[1].y *= -5;

		Joint::add_Hingejoint(coll, door,
			vector3_quatrotate(joint_pos[0], door->transform->local_orient) + door->transform->local_pos,
			vector3_quatrotate(joint_pos[1], door->transform->local_orient) + door->transform->local_pos,
			joint_pos[0], joint_pos[1],
			0.05f
		);
	}

	Gameobject* Stage_01::set_buttan(const Vector3& pos, const Vector3& scale, const Vector3& rotate, Stage_parts::Stageparts_tagbit tag, Gameobject* pearent, bool is_use_trigger) {
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
		buttan_comp->base_material_color = Vector3(1,0,0);
		buttan_comp->is_use_trigger = is_use_trigger;

		return go;
	}

	Gameobject* Stage_01::set_gimmickdoor(
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
	Gameobject* Stage_01::set_Andcircuit(Stage_parts::Stageparts_tagbit in_tag0, Stage_parts::Stageparts_tagbit in_tag1, Stage_parts::Stageparts_tagbit out_tag, Gameobject* pearent) {
		Gameobject* go = Gameobject_manager::create("Andcircuit");
		auto comp = go->addComponent<Stage_parts::Tag_Andcicuit>();
		comp->in_tag0 = in_tag0;
		comp->in_tag1 = in_tag1;
		comp->out_tag = out_tag;

		comp->this_stage = this;

		if (pearent != nullptr)pearent->add_child(go);
	}

	Gameobject* Stage_01::set_sphere_rope(const Vector3& pos, const float& scale, const Vector3& rotate, int quantity, float mass, const Vector3& color) {

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





	Gameobject* Stage_01::set_desk(const Vector3& pos, const Vector3& scale, const Vector3& rotate, float mass) {

		// deskの生成とcolliderのアタッチ
		Gameobject* desk = nullptr;
		desk = Gameobject_manager::create("Desk", GO_tag::Sphere);
		desk->transform->local_pos = pos;
		desk->transform->local_scale = scale;
		desk->transform->local_orient = quaternion_from_euler(rotate);

		Collider* coll = desk->addComponent<Collider>();
		coll->tag = Collider_tags::Stage | Collider_tags::Kinematic_Stage ;
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
		//parts[0]->material->color = Vector4(238, 229, 224, 255) / 255;
		////parts[1]->material->color = Vector4(207, 171, 142, 255) / 255;
		//parts[1]->material->color = Vector4(255, 77, 17, 255) / 255;

		parts[0]->material->color = C;
		parts[1]->material->color = C;
		parts[2]->material->color = C;
		parts[3]->material->color = C;
		parts[4]->material->color = C;

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


		stage_parts.emplace_back(desk);
		stage_parts.emplace_back(parts[0]);
		stage_parts.emplace_back(parts[1]);
		stage_parts.emplace_back(parts[2]);
		stage_parts.emplace_back(parts[3]);
		stage_parts.emplace_back(parts[4]);

		return desk;
	}

	Gameobject* Stage_01::set_gear(const Vector3& pos, const Vector3& size, const Vector3& rotate, float mass, int tooth_count) {

		Gameobject* GEAR = Gameobject_manager::create("GEAR");
		GEAR->transform->local_pos = pos;
		GEAR->transform->local_orient = quaternion_from_euler(rotate);

		Collider* coll = GEAR->addComponent<Collider>();
		//coll->tag |= Collider_tags::Stage | Collider_tags::Kinematic_Stage ;
		coll->tag |= Collider_tags::Stage | Collider_tags::Kinematic_Stage | Collider_tags::Caera_not_sunk_Stage ;
		coll->physics_data.inertial_mass = mass;
		coll->physics_data.anglar_drag = 0;
		coll->ignore_tags = Collider_tags::Static_Stage;

		for (int gear_tooth_num = 0; gear_tooth_num < tooth_count; gear_tooth_num++) {
			Gameobject* GO = Gameobject_manager::createCube("tooth");
			GO->transform->local_scale = size;
			GO->transform->local_orient = quaternion_from_euler(0, 0, 360.0f / tooth_count * gear_tooth_num);
			GO->transform->local_pos = Vector3(0, size.y * 2, 0);
			GO->transform->local_pos = vector3_quatrotate(GO->transform->local_pos, quaternion_axis_angle(Vector3(0, 0, 1), 360.0f / tooth_count * gear_tooth_num));

			GO->material->color = Vector4(1, 1, 0, 1);
			GEAR->add_child(GO);
			stage_parts.emplace_back(GO);
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

		stage_parts.emplace_back(gear_joint);
		stage_parts.emplace_back(GEAR);

		return GEAR;

	}
}