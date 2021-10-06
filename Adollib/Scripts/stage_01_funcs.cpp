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

namespace Adollib
{
	Gameobject* Stage_01::set_tree(const Vector3& pos, const Vector3& scale, const Vector3& rotate) {
		Gameobject* tree = nullptr;
		tree = Gameobject_manager::create("tree", GO_tag::None);

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

		tree_parts[1]->transform->local_pos = Vector3(0, 15, 0);
		tree_parts[1]->transform->local_scale = Vector3(1.5, 1, 1.5);
		tree_parts[1]->transform->local_orient = quaternion_from_euler(0, 0, 0);
		tree_parts[1]->material->color = reaf_color;

		tree_parts[2]->transform->local_pos = Vector3(0, 13, 0);
		tree_parts[2]->transform->local_scale = Vector3(2.0, 1, 2.0);
		tree_parts[2]->transform->local_orient = quaternion_from_euler(0, 1.7f, 0);
		tree_parts[2]->material->color = reaf_color;

		tree_parts[3]->transform->local_pos = Vector3(0, 11, 0);
		tree_parts[3]->transform->local_scale = Vector3(3.0, 1, 3.0);
		tree_parts[3]->transform->local_orient = quaternion_from_euler(0, 13, 0);
		tree_parts[3]->material->color = reaf_color;

		tree_parts[4]->transform->local_pos = Vector3(0, 9, 0);
		tree_parts[4]->transform->local_scale = Vector3(3.5, 1, 3.5);
		tree_parts[4]->transform->local_orient = quaternion_from_euler(0, 28, 0);
		tree_parts[4]->material->color = reaf_color;

		tree_parts[5]->transform->local_pos = Vector3(0, 7, 0);
		tree_parts[5]->transform->local_scale = Vector3(4.5, 1, 4.5);
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
		coll->physics_data.inertial_mass = 35;
		coll->tag = Collider_tags::Stage | Collider_tags::Kinematic_Stage | Collider_tags::Jumpable_Stage;

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
		boxes[6]->size = Vector3(3, 0.2f, 3);
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

		for (int i = 0; i < Tree_size; i++) stage_parts.emplace_back(tree_parts[i]);
		stage_parts.emplace_back(tree);

		return tree;
	}

	Gameobject* Stage_01::set_desk(const Vector3& pos, const Vector3& scale, const Vector3& rotate, float mass) {

		// deskの生成とcolliderのアタッチ
		Gameobject* desk = nullptr;
		desk = Gameobject_manager::create("Desk", GO_tag::Sphere);
		desk->transform->local_pos = pos;
		desk->transform->local_scale = scale;
		desk->transform->local_orient = quaternion_from_euler(rotate);

		Collider* coll = desk->addComponent<Collider>();
		coll->tag = Collider_tags::Stage | Collider_tags::Kinematic_Stage | Collider_tags::Jumpable_Stage;
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
		coll->tag |= Collider_tags::Stage | Collider_tags::Kinematic_Stage | Collider_tags::Jumpable_Stage;
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


	Gameobject* Stage_01::set_buttan(const Vector3& pos, const Vector3& scale, const Vector3& rotate, Stage_parts::Stageparts_tagbit tag, const Vector3& color) {
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

		return go;
	}

	Gameobject* Stage_01::set_door(
		const Vector3& start_pos, const Vector3& goal_pos,
		const Vector3& start_rot, const Vector3& goal_rot,
		float pos_speed, float rot_speed,
		const Vector3& scale,
		Stage_parts::Stageparts_tagbit tag,
		const Vector3& color
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

		return go;
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
			auto coll = set_sphere(pos - Vector3(0, scale * (i + 1) * 2, 0), scale, color, false);
			coll->tag &= ~Collider_tags::Caera_not_sunk_Stage;
			coll->physics_data.inertial_mass = mass * powf(1.1f, quantity - i);

			rope_pearent->add_child(coll->gameobject);

			if (old_coll != nullptr) Joint::add_balljoint(old_coll, coll, Vector3(0, -scale * 1.05f, 0), Vector3(0, scale * 1.05f, 0), 0.1f);

			old_coll = coll;
			old_go = go;
		}

		return rope_pearent;

	}




}