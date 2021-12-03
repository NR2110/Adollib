

#include "stage_demo_meishi.h"

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
	void Stage_demo_meishi::stage_awake()
	{
		player_respown_pos = Vector3(-2.7f, 80, -5.f);
		respown_num = -1;
		//player_respown_pos = Vector3(-2.7f, 10, -5);
		//player_respown_pos = Vector3(90, 16.0f, 66.0f);
		y_respown_pos = 30; // respownしたときのYの座標

		//set_box(Vector3(0, -60, -98), Vector3(30, 30, 60), Vector3(0), Vector3(188, 214, 54) / 255.0f, true);

		//set_desk(Vector3(0, 0, 0), Vector3(30, 15, 25), Vector3(0, 0, 0), 0);

#if _DEBUG
		//set_player_statue(Vector3(104, 17.6f, 132), 100, Vector3(0));
		set_pillar(Vector3(16, 4, 23), Vector3(1), Vector3(0));
#else
		Gameobject* demo = Gameobject_manager::create("demo");
		stage_parts.emplace_back(demo);
		auto b = set_box(Vector3(25, -2, 10), Vector3(65, 2, 30), Vector3(0), Vector3(188, 214, 54) / 255.0f * 0.8f * 0.9f, demo);
		b->tag &= ~Collider_tags::Caera_not_sunk_Stage;

		{
			auto* pearent = Gameobject_manager::create("pearent");

			int count = 30;
			Vector3 size = Vector3(1.3f,1,1);
			Vector3 position = Vector3(0);
			float aida = 0.5f;
			Vector3 color = Vector3(1, 248, 230) / 255.f;

			for (int y_num = 0; y_num < count; ++y_num) {
				for (int x_num = 0; x_num < count - y_num; ++x_num) {

					set_box(
						Vector3(
							(size.x * 2 + aida) * x_num + (size.x * 2 + aida) * 0.5f * y_num,
							size.y + y_num * size.y * 2,
							0),
						Vector3(size),
						Vector3(0,1.5f,0),
						color,
						pearent


					);
				}
			}
		}




#endif

		//set_sphere_rope(Vector3(64, 60, 65), 1, Vector3(0, 0, 0), 10, 5);
	}

	// 毎フレーム呼ばれる更新処理
	void Stage_demo_meishi::update() {

	}

	void Stage_demo_meishi::Update_hierarchy() {

	}

	void Stage_demo_meishi::stage_destroy() {
		for (auto& object : stage_parts) {
			Gameobject_manager::deleteGameobject(object, true);
		}
		stage_parts.clear();
	}

}
