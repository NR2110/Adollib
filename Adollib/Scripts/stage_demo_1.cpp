

#include "stage_demo_1.h"

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
	void Stage_demo_01::stage_awake()
	{
		player_respown_pos = Vector3(-2.7f, 50, 0);
		respown_num = -1;
		next_stage = Stage_types::none;

		//set_box(Vector3(0, -60, -98), Vector3(30, 30, 60), Vector3(0), Vector3(188, 214, 54) / 255.0f, true);

		auto floar_go = Gameobject_manager::create("floar_go");
		stage_parts.emplace_back(floar_go);

		//set_desk(Vector3(0, 0, 0), Vector3(30, 15, 25), Vector3(0, 0, 0), 0);
		set_box(Vector3(0, -60, 0), Vector3(80, 60, 40), Vector3(0), Vector3(188, 214, 54) / 255.0f, floar_go);

#if _DEBUG
		set_tree(Vector3(-22, 0, -25), Vector3(1, 1.5f, 1), Vector3(0));
		set_gear(Vector3(+8, 15, 0), Vector3(1, 5, 5), Vector3(0), 12, 8);

		set_box(Vector3(32, 4, 0), Vector3(14.5f, 2.21f, 4), Vector3(0, 0, -43.3f), Vector3(188, 214, 54) / 255.0f);
#else
		////gear
		set_gear(Vector3(+22, 1, 25), Vector3(0.8f, 2.2f, 2), Vector3(0), 3, 7);
		set_gear(Vector3(+11, 1, 25), Vector3(0.8f, 2.2f, 2), Vector3(0), 3, 7);
		set_gear(Vector3(+-0, 1, 25), Vector3(0.8f, 2.2f, 2), Vector3(0), 3, 7);
		set_gear(Vector3(-11, 1, 25), Vector3(0.8f, 2.2f, 2), Vector3(0), 3, 7);
		set_gear(Vector3(-22, 1, 25), Vector3(0.8f, 2.2f, 2), Vector3(0), 3, 7);

		set_gear(Vector3(+8, 15, 0), Vector3(1, 5, 5), Vector3(0), 12, 8);

		set_box(Vector3(32, 4, 0), Vector3(14.5f, 2.21f, 4), Vector3(0, 0, -43.3f), Vector3(188, 214, 54) / 255.0f, floar_go);

		if (0)
		set_sphere_rope(Vector3(-40, 15, 40), 1.5f, Vector3(-90, 0, 0), 28, 5);

		set_tree(Vector3(-22, 0, -25), Vector3(1, 1.2f, 1), Vector3(0));
		set_tree(Vector3(+22, 0, -25), Vector3(1, 1.2f, 1), Vector3(0));
		//{}

		set_box(Vector3(65, 5, 31), Vector3(4, 7, 4), Vector3(0), Vector3(0.9f, 0.9f, 1) * 0.97f,floar_go);
		set_box(Vector3(69, 5, 23), Vector3(4, 7, 4), Vector3(0), Vector3(0.9f, 0.9f, 1) * 0.97f,floar_go);
		set_box(Vector3(61, 5, 23), Vector3(4, 7, 4), Vector3(0), Vector3(0.9f, 0.9f, 1) * 0.97f,floar_go);
		set_box(Vector3(65, 15, 27), Vector3(4, 7, 4), Vector3(0), Vector3(0.9f, 0.9f, 1) * 0.97f, floar_go);


		set_box(Vector3(55, 4, -31), Vector3(4, 4, 4), Vector3(0), Vector3(188, 214, 54) / 255.0f * 0.9f,floar_go);
		set_box(Vector3(59, 4, -23), Vector3(4, 4, 4), Vector3(0), Vector3(188, 214, 54) / 255.0f * 0.8f,floar_go);
		set_box(Vector3(51, 4, -23), Vector3(4, 4, 4), Vector3(0), Vector3(188, 214, 54) / 255.0f * 0.7f,floar_go);
		set_box(Vector3(55, 12, -27), Vector3(4, 4, 4), Vector3(0), Vector3(188, 214, 54) / 255.0f * 0.6f, floar_go);
#endif

		//set_sphere_rope(Vector3(64, 60, 65), 1, Vector3(0, 0, 0), 10, 5);
	}

	// 毎フレーム呼ばれる更新処理
	void Stage_demo_01::update() {

	}

	void Stage_demo_01::Update_hierarchy() {

	}

	void Stage_demo_01::stage_destroy() {
		for (auto& object : stage_parts) {
			Gameobject_manager::deleteGameobject(object, true);
		}
		stage_parts.clear();
	}

}
