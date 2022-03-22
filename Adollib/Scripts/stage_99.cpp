
#include "stage_99.h"

#include "../Adollib/Scripts/Object/gameobject_manager.h"
#include "../Adollib/Scripts/Scene/scene_manager.h"

#include "../Adollib/Scripts/Math/closest_func.h"
#include "../Adollib/Scripts/Imgui/imgui_all.h"

#include "../Adollib/Scripts/Imgui/debug.h"
#include "../Adollib/Scripts/Physics/ray.h"
#include "../Adollib/Scripts/Physics/joint.h"
#include "../Adollib/Scripts/Physics/collider_croth.h"
#include "../Adollib/Scripts/Physics/ALP__physics_manager.h"

#include "../Adollib/Scripts/Renderer/mesh_renderer.h"
#include "../Adollib/Scripts/Renderer/material_manager.h"
#include "../Adollib/Scripts/Renderer/material.h"
#include "../Adollib/Scripts/Renderer/texture.h"
#include "../Adollib/Scripts/Renderer/sprite_renderer.h"
#include "../Adollib/Scripts/Object/component_camera.h"

#include "../Adollib/Scripts/Main/systems.h"
#include "../Adollib/Scripts/Renderer/croth_renderer.h"

#include "player_manager.h"

#include "../Adollib/Scripts/Physics/collider_rope.h"
#include "../Adollib/Scripts/Renderer/rope_renderer.h"

namespace Adollib
{
	void Stage_99::stage_awake()
	{
		player_respown_pos = Vector3(-2.7f, 50, -5.f);
		respown_num = -1;
		next_stage = Stage_types::none;
		y_respown_pos = 100; // respownしたときのYの座標
		y_player_respown_limit = -60; //playerのyがこれ以下だとrespownされる
		y_respown_limit = -150;  //blockのyがこれ以下だとrespownされる

		auto player_manager = Gameobject_manager::find("Player_manager", Scenelist::scene_player)->findComponent<Player_manager>();
		player_manager->set_is_shotable(true);

#if  0 && _DEBUG
		////auto cube = Gameobject_manager::createFromFBX("Shaclo", "../Data/FBX/Model_Shaclo_Winter_Edit.fbx",true);
		////auto cube = Gameobject_manager::createFromFBX("bunny", "../Data/FBX/bunny.obj");
		////auto cube = Gameobject_manager::createFromFBX("plane", "./DefaultModel/plane_64x64vertex.fbx");
		//auto cube = Gameobject_manager::createCube();
		//////auto cube = Gameobject_manager::createSphere();

		//auto coll = cube->addComponent<Collider>();
		//coll->add_mesh_shape("../Data/FBX/stage_col.fbx", true, false);

		////auto coll = cube->addComponent<Collider_Croth>();
		////coll->load_file("./DefaultModel/plane_64x64vertex.fbx", true, false);

		////auto renderer = cube->addComponent<Croth_renderer>();
		////renderer->set_meshoffset(coll->get_vertex_offset());

		////coll->add_shape("../Data/FBX/bunny.obj", true, false);
		////coll->add_shape("./DefaultModel/sphere.fbx", true, false);
		////coll->physics_data.is_hitable = false;
		//coll->physics_data.is_moveable = false;

#else

		// first_zone
		{
			Gameobject* first_zone = Gameobject_manager::create("first_zone");
			stage_parts.emplace_back(first_zone);

			// stage
			{
				Gameobject* pearent = Gameobject_manager::create("stage");
				first_zone->add_child(pearent);

				set_box(Vector3(-6, -2, 33), Vector3(100, 2, 50), Vector3(0, 0, 0), grass_color, pearent);
				set_box(Vector3(-6, 2, 59), Vector3(90, 2, 24), Vector3(0, 0, 0), base_color, pearent);

				set_box(Vector3(-6, 1.5f, 33), Vector3(90, 1.5f, 2), Vector3(0, 0, 0), base_color, pearent);
				set_box(Vector3(-6, 1.0f, 29), Vector3(90, 1.0f, 2), Vector3(0, 0, 0), base_color, pearent);
				set_box(Vector3(-6, 0.5f, 25), Vector3(90, 0.5f, 2), Vector3(0, 0, 0), base_color, pearent);

				set_box(Vector3(-96, 3, 45), Vector3(12, 10, 44), Vector3(0, 0, 0), base_color, pearent);
				set_box(Vector3(+91, 3, 45), Vector3(12, 10, 44), Vector3(0, 0, 0), base_color, pearent);

				set_box(Vector3(-95, 12, 92), Vector3(17, 25, 20), Vector3(0, 0, 0), base_color, pearent);
				set_box(Vector3(89, 12, 92), Vector3(17, 25, 20), Vector3(0, 0, 0), base_color, pearent);

			}
			// wall
			{
				Gameobject* pearent = Gameobject_manager::create("wall");
				first_zone->add_child(pearent);

				set_box(Vector3(-12, 8, 82), Vector3(8, 4, 1), Vector3(0, 0, 0), base_color, pearent); //左
				set_box(Vector3(+12, 8, 82), Vector3(8, 4, 1), Vector3(0, 0, 0), base_color, pearent); //右
				set_box(Vector3(0, 15, 82), Vector3(10, 3, 1), Vector3(0, 0, 0), base_color, pearent); //上
				set_box(Vector3(0, 3, 86), Vector3(8, 1, 3), Vector3(0, 0, 0), base_color, pearent); //飛び込み台

				set_door(Vector3(-2, 8, 82), Vector3(2, 4, 0.5f), Vector3(0), true, pearent);
				set_door(Vector3(+2, 8, 82), Vector3(2, 4, 0.5f), Vector3(0), false, pearent);

				set_box(Vector3(-52, 8, 84), Vector3(32, 20, 1), Vector3(0, 0, 0), base_color, pearent);
				set_box(Vector3(50, 8, 84), Vector3(30, 20, 1), Vector3(0, 0, 0), base_color, pearent);

				set_box(Vector3(0, 26, 84), Vector3(30, 8, 1), Vector3(0, 0, 0), base_color, pearent);
				set_box(Vector3(-15, 15, 84), Vector3(5, 3, 1), Vector3(0, 0, 0), base_color, pearent);
				set_box(Vector3(+15, 15, 84), Vector3(5, 3, 1), Vector3(0, 0, 0), base_color, pearent);
			}
			// Thank_you_for_playing
			{
				Gameobject* pearent = Gameobject_manager::create("Thank_you_for_playing");
				first_zone->add_child(pearent);
				pearent->transform->local_pos = Vector3(-75, 15, 50);

				float x_offet = 8;
				float space = 3;
				float timer_offset = 0.1f;

				int tango_count = 0;
				int space_count = 0;

				set_T(Vector3(space * space_count + x_offet * tango_count++, 0, 0), Vector3(1), Vector3(0), base_color, pearent, timer_offset * tango_count);
				set_H(Vector3(space * space_count + x_offet * tango_count++, 0, 0), Vector3(1), Vector3(0), base_color, pearent, timer_offset * tango_count);
				set_A(Vector3(space * space_count + x_offet * tango_count++, 0, 0), Vector3(1), Vector3(0), base_color, pearent, timer_offset * tango_count);
				set_N(Vector3(space * space_count + x_offet * tango_count++, 0, 0), Vector3(1), Vector3(0), base_color, pearent, timer_offset * tango_count);
				set_K(Vector3(space * space_count + x_offet * tango_count++, 0, 0), Vector3(1), Vector3(0), base_color, pearent, timer_offset * tango_count);
				space_count++;
				set_Y(Vector3(space * space_count + x_offet * tango_count++, 0, 0), Vector3(1), Vector3(0), base_color, pearent, timer_offset * tango_count);
				set_O(Vector3(space * space_count + x_offet * tango_count++, 0, 0), Vector3(1), Vector3(0), base_color, pearent, timer_offset * tango_count);
				set_U(Vector3(space * space_count + x_offet * tango_count++, 0, 0), Vector3(1), Vector3(0), base_color, pearent, timer_offset * tango_count);
				space_count++;
				set_F(Vector3(space * space_count + x_offet * tango_count++, 0, 0), Vector3(1), Vector3(0), base_color, pearent, timer_offset * tango_count);
				set_O(Vector3(space * space_count + x_offet * tango_count++, 0, 0), Vector3(1), Vector3(0), base_color, pearent, timer_offset * tango_count);
				set_R(Vector3(space * space_count + x_offet * tango_count++, 0, 0), Vector3(1), Vector3(0), base_color, pearent, timer_offset * tango_count);
				space_count++;
				set_P(Vector3(space * space_count + x_offet * tango_count++, 0, 0), Vector3(1), Vector3(0), base_color, pearent, timer_offset * tango_count);
				set_L(Vector3(space * space_count + x_offet * tango_count++, 0, 0), Vector3(1), Vector3(0), base_color, pearent, timer_offset * tango_count);
				set_A(Vector3(space * space_count + x_offet * tango_count++, 0, 0), Vector3(1), Vector3(0), base_color, pearent, timer_offset * tango_count);
				set_Y(Vector3(space * space_count + x_offet * tango_count++, 0, 0), Vector3(1), Vector3(0), base_color, pearent, timer_offset * tango_count);
				set_I(Vector3(space * space_count + x_offet * tango_count++, 0, 0), Vector3(1), Vector3(0), base_color, pearent, timer_offset * tango_count);
				set_N(Vector3(space * space_count + x_offet * tango_count++, 0, 0), Vector3(1), Vector3(0), base_color, pearent, timer_offset * tango_count);
				set_G(Vector3(space * space_count + x_offet * tango_count++, 0, 0), Vector3(1), Vector3(0), base_color, pearent, timer_offset * tango_count);
			}

			// button
			{
				Gameobject* pearent = Gameobject_manager::create("button");
				first_zone->add_child(pearent);

				set_buttan(Vector3(46, 0, 4), Vector3(4, 0.9f, 4), Vector3(0, 0, 0), Stage_parts::Stageparts_tags::Flag_6, pearent, 1);
				set_buttan(Vector3(-55, 0, 4), Vector3(4, 0.9f, 4), Vector3(0, 0, 0), Stage_parts::Stageparts_tags::Flag_5, pearent, 1);
			}

			// trees
			{
				Gameobject* pearent = Gameobject_manager::create("trees");
				first_zone->add_child(pearent);

				set_tree(Vector3(85, 0, -8), Vector3(1, 1.2f, 1), Vector3(0, 02, 0), 2.5f, 0.8f, pearent);
				set_tree(Vector3(74, 0, -8), Vector3(1, 1.2f, 1), Vector3(0, 02, 0), 2.5f, 0.8f, pearent);
				set_tree(Vector3(63, 0, -8), Vector3(1, 1.2f, 1), Vector3(0, 02, 0), 2.5f, 0.8f, pearent);

				set_tree(Vector3(-75, 0, -8), Vector3(1, 1.2f, 1), Vector3(0, 02, 0), 2.5f, 0.8f, pearent);
				set_tree(Vector3(-86, 0, -8), Vector3(1, 1.2f, 1), Vector3(0, 02, 0), 2.5f, 0.8f, pearent);
				set_tree(Vector3(-97, 0, -8), Vector3(1, 1.2f, 1), Vector3(0, 02, 0), 2.5f, 0.8f, pearent);


			}
			//fences
			{
				Gameobject* pearent = Gameobject_manager::create("trees");
				first_zone->add_child(pearent);

				set_fence(Vector3(60.1f, 0, -14.8f), Vector3(1), Vector3(0, 0, 0), pearent);
				set_fence(Vector3(79.4f, 0, -14.8f), Vector3(1), Vector3(0, 0, 0), pearent);
				set_fence(Vector3(92, 0, -6.6f), Vector3(1), Vector3(0, 90, 0), pearent);

				set_fence(Vector3(-69.2f, 0, -14.8f), Vector3(1), Vector3(0, 0, 0), pearent);
				set_fence(Vector3(-90.8f, 0, -14.8f), Vector3(1), Vector3(0, 0, 0), pearent);
				set_fence(Vector3(-103.8f, 0, -5.4f), Vector3(1), Vector3(0, 90, 0), pearent);
			}

			// exit
			{
				std::shared_ptr<Material> exit_mat = Material_manager::create_material("exit_material");
				exit_mat->get_texture()->Load(L"./DefaultTexture/stage/EXIT.png");
				exit_mat->BS_state = State_manager::BStypes::BS_ALPHA;
				exit_mat->RS_state = State_manager::RStypes::RS_CULL_NONE;
				exit_mat->Load_VS("./DefaultShader/sprite_vs.cso");
				exit_mat->Load_PS("./DefaultShader/sprite_ps.cso");
				exit_mat->is_render_shadow = false;
				exit_mat->color = Vector4(1, 1, 1, 1);
				auto plane = set_plane(Vector3(0, 14, 80), Vector3(2.5f, 1.5f, 1), Vector3(0, 180, 0), Vector3(1), first_zone);
				plane->name = std::string("exit_plane");
				plane->renderer->set_material(exit_mat);

			}
		}


		//set_lever(Vector3(0,0,0),Vector3(1),   Vector3(0), Stage_parts::Stageparts_tags::Flag_4, Stage_parts::Stageparts_tags::Flag_4);
		//set_lever(Vector3(8,18,104),Vector3(1),Vector3(0,90,0), Stage_parts::Stageparts_tags::Flag_4, Stage_parts::Stageparts_tags::Flag_4);

		//set_box(Vector3(0, 100, 0), Vector3(10, 10, 10), Vector3(0, 0, 0), grass_color);
		//set_box(Vector3(0, 100, 0), Vector3(10, 10, 10), Vector3(0, 0, 0), grass_color);
		//set_box(Vector3(0, 100, 0), Vector3(10, 10, 10), Vector3(0, 0, 0), grass_color);
		//set_box(Vector3(0, 100, 0), Vector3(10, 10, 10), Vector3(0, 0, 0), grass_color);
		//set_box(Vector3(0, 100, 0), Vector3(10, 10, 10), Vector3(0, 0, 0), grass_color);
		//set_box(Vector3(0, 100, 0), Vector3(10, 10, 10), Vector3(0, 0, 0), grass_color);
		//set_box(Vector3(0, 100, 0), Vector3(10, 10, 10), Vector3(0, 0, 0), grass_color);

		//set_tree(Vector3(0, 100, 0), Vector3(1, 1.2f, 1), Vector3(0, 02, 0), 2.5f, 0.8f);
		//set_tree(Vector3(0, 100, 0), Vector3(1, 1.2f, 1), Vector3(0, 02, 0), 2.5f, 0.8f);
		//set_tree(Vector3(0, 100, 0), Vector3(1, 1.2f, 1), Vector3(0, 02, 0), 2.5f, 0.8f);
		//set_tree(Vector3(0, 100, 0), Vector3(1, 1.2f, 1), Vector3(0, 76, 0), 1.2f, 0.8f);
		//set_tree(Vector3(0, 100, 0), Vector3(1, 1.2f, 1), Vector3(0, 76, 0), 1.2f, 0.8f);
		//set_tree(Vector3(0, 100, 0), Vector3(1, 1.2f, 1), Vector3(0, 76, 0), 1.2f, 0.8f);

		//set_fence(Vector3(0,100,0), Vector3(1), Vector3(0, 0, 0));
		//set_fence(Vector3(0,100,0), Vector3(1), Vector3(0, 0, 0));
		//set_fence(Vector3(0,100,0), Vector3(1), Vector3(0, 0, 0));
		//set_fence(Vector3(0,100,0), Vector3(1), Vector3(0, 0, 0));
		//set_fence(Vector3(0,100,0), Vector3(1), Vector3(0, 0, 0));
		//set_fence(Vector3(0,100,0), Vector3(1), Vector3(0, 0, 0));
		//set_fence(Vector3(0,100,0), Vector3(1), Vector3(0, 0, 0));

		// goal
		{
			Gameobject* goal_and_respown_areas = Gameobject_manager::create("goal&respown_areas");
			stage_parts.emplace_back(goal_and_respown_areas);

			set_goal_area(Vector3(0, -1, 100), Vector3(20, 2, 14), Vector3(0), Stage_types::Title , goal_and_respown_areas);
		}


#endif


	}

	// 毎フレーム呼ばれる更新処理
	void Stage_99::update() {

	}

	void Stage_99::Update_hierarchy() {

	}

	void Stage_99::stage_destroy() {
		for (auto& object : stage_parts) {
			Gameobject_manager::deleteGameobject(object, true);
		}
		stage_parts.clear();
	}

}
