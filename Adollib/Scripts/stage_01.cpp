

#include "stage_01.h"

#include "../Adollib/Scripts/Object/gameobject_manager.h"
#include "../Adollib/Scripts/Scene/scene_manager.h"

#include "../Adollib/Scripts/Math/closest_func.h"
#include "../Adollib/Scripts/Imgui/imgui_all.h"
#include "../Adollib/Scripts/Physics/ALP__physics_manager.h"

#include "../Adollib/Scripts/Physics/ray.h"
#include "../Adollib/Scripts/Imgui/debug.h"
#include "../Adollib/Scripts/Physics/joint.h"

#include "../Adollib/Scripts/Renderer/mesh_renderer.h"
#include "../Adollib/Scripts/Renderer/sprite_renderer.h"
#include "../Adollib/Scripts/Object/component_camera.h"

#include "../Adollib/Scripts/Main/systems.h"

namespace Adollib
{
	void Stage_01::stage_awake()
	{
		player_respown_pos = Vector3(-2.7f, 50, 0);


		//set_box(Vector3(0, -60, -98), Vector3(30, 30, 60), Vector3(0), Vector3(188, 214, 54) / 255.0f, true);

		//set_desk(Vector3(0, 0, 0), Vector3(30, 15, 25), Vector3(0, 0, 0), 0);
		set_box(Vector3(0, -60, 0), Vector3(80, 60, 40), Vector3(0), Vector3(188, 214, 54) / 255.0f,  true);

#if _DEBUG
		set_tree(Vector3(-22, 0, -25), Vector3(1, 1.5f, 1), Vector3(0));
#else
		////gear
		set_gear(Vector3(+22, 1, 25), Vector3(0.8f, 2.2f, 2), Vector3(0), 5, 7);
		set_gear(Vector3(+11, 1, 25), Vector3(0.8f, 2.2f, 2), Vector3(0), 5, 7);
		set_gear(Vector3(+-0, 1, 25), Vector3(0.8f, 2.2f, 2), Vector3(0), 5, 7);
		set_gear(Vector3(-11, 1, 25), Vector3(0.8f, 2.2f, 2), Vector3(0), 5, 7);
		set_gear(Vector3(-22, 1, 25), Vector3(0.8f, 2.2f, 2), Vector3(0), 5, 7);

		set_gear(Vector3(+8, 15, 0), Vector3(1, 5, 5), Vector3(0), 12, 8);

		set_box(Vector3(32, 4, 0), Vector3(14.5f,2.21f, 4), Vector3(0,0,-43.3f), Vector3(188, 214, 54) / 255.0f);

		set_sphere_rope(Vector3(-40, 15, 40), 1.5f, Vector3(-90, 0, 0), 28, 5);

		set_tree(Vector3(-22, 0, -25), Vector3(1, 1.2f, 1), Vector3(0));
		set_tree(Vector3(+22, 0, -25), Vector3(1, 1.2f, 1), Vector3(0));
		//{}

		//croth
		{
			Gameobject* pearent = Gameobject_manager::create("BallJoint_Shperenet");
			pearent->transform->local_pos = Vector3(62, 30, 22);
			pearent->transform->local_orient = quaternion_from_euler(0, 45, 0);
			std::vector<Collider*>colls;
			stage_parts.emplace_back(pearent);

			int sphere_count = 20;
			float sphere_size = 1.5f;
			const int colls_size = sphere_count * sphere_count;
			colls.resize(colls_size);

			for (int xaxis = 0; xaxis < sphere_count; xaxis++) {
				for (int zaxis = 0; zaxis < sphere_count; zaxis++) {
					int index = xaxis * sphere_count + zaxis;

					Collider* coll = nullptr;
					Gameobject* go = nullptr;

					coll = set_sphere(Vector3(
						(xaxis - sphere_count * 0.5f) * sphere_size * 2,
						2,
						(zaxis - sphere_count * 0.5f) * sphere_size * 2
					),
						sphere_size,
						Vector3(1, 0, (1.0f / (sphere_count * sphere_count)) * index),
						false
					);
					coll->physics_data.is_moveable = true;
					coll->tag = Collider_tags::Sphere | Collider_tags::Stage | Collider_tags::Caera_not_sunk_Stage | Collider_tags::Kinematic_Stage | Collider_tags::Jumpable_Stage;

					go = coll->gameobject;

					pearent->add_child(go);
					colls.at(index) = (coll);

					coll->tag &= ~Collider_tags::Caera_not_sunk_Stage;
					coll->physics_data.inertial_mass = 0.1f;
				}
			}

			for (int xaxis = 0; xaxis < sphere_count; xaxis++) {
				for (int zaxis = 0; zaxis < sphere_count; zaxis++) {
					int index = xaxis * sphere_count + zaxis;

					if (xaxis > 0) Joint::add_balljoint(colls[index], colls[index - sphere_count], Vector3(-sphere_size * 1.1f, 0, 0), Vector3(sphere_size * 1.1f, 0, 0), 1);
					if (zaxis > 0) Joint::add_balljoint(colls[index], colls[index - 1], Vector3(0, 0, -sphere_size * 1.1f), Vector3(0, 0, sphere_size * 1.1f), 1);
				}
			}
		}

		set_box(Vector3(65, 5, 31), Vector3(4, 7, 4), Vector3(0), Vector3(0.9f, 0.9f, 1) * 0.97f, true);
		set_box(Vector3(69, 5, 23), Vector3(4, 7, 4), Vector3(0), Vector3(0.9f, 0.9f, 1) * 0.97f, true);
		set_box(Vector3(61, 5, 23), Vector3(4, 7, 4), Vector3(0), Vector3(0.9f, 0.9f, 1) * 0.97f, true);
		set_box(Vector3(65, 15, 27), Vector3(4, 7, 4), Vector3(0), Vector3(0.9f, 0.9f, 1) * 0.97f, true);


		set_box(Vector3(55, 4,  -31),  Vector3(4, 4, 4), Vector3(0), Vector3(188, 214, 54) / 255.0f * 0.9f, true);
		set_box(Vector3(59, 4,  -23),  Vector3(4, 4, 4), Vector3(0), Vector3(188, 214, 54) / 255.0f * 0.8f, true);
		set_box(Vector3(51, 4,  -23),  Vector3(4, 4, 4), Vector3(0), Vector3(188, 214, 54) / 255.0f * 0.7f, true);
		set_box(Vector3(55, 12, -27), Vector3(4, 4, 4), Vector3(0),  Vector3(188, 214, 54) / 255.0f * 0.6f, true);
#endif

		//set_sphere_rope(Vector3(64, 60, 65), 1, Vector3(0, 0, 0), 10, 5);
	}

	// 毎フレーム呼ばれる更新処理
	void Stage_01::update() {

	}

	void Stage_01::Update_hierarchy() {

	}

	void Stage_01::stage_destroy() {
		for (auto& object : stage_parts) {
			Gameobject_manager::deleteGameobject(object);
		}
		stage_parts.clear();
	}

}
