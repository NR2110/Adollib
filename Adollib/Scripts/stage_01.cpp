

#include "stage_01.h"

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

namespace Adollib
{
	void Stage_01::stage_awake()
	{
		player_respown_pos = Vector3(-2.7f, 50, -5.f);
		respown_num = -1;
		next_stage = Stage_types::none;
		//player_respown_pos = Vector3(-2.7f, 10, -5);
		//player_respown_pos = Vector3(90, 16.0f, 66.0f);
		y_respown_pos = 50; // respownしたときのYの座標

		//set_box(Vector3(0, -60, -98), Vector3(30, 30, 60), Vector3(0), Vector3(188, 214, 54) / 255.0f, true);

		//set_desk(Vector3(0, 0, 0), Vector3(30, 15, 25), Vector3(0, 0, 0), 0);

		//Gameobject_manager::createFromFBX("sponza", "../Data/FBX/Model_Shaclo_Winter_Edit.fbx");

#if  _DEBUG
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

					// 回転gimmick
		{
			// 階段
			{
				set_box(Vector3(25, -2, 10), Vector3(65, 2, 30), Vector3(0), Vector3(188, 214, 54) / 255.0f * 0.8f * 0.9f);
				set_box(Vector3(-15, 2, 23), Vector3(15, 2, 15), Vector3(0, 0, 0), base_color);
				set_box(Vector3(28, 2, 29.5f), Vector3(28, 2, 8.5f), Vector3(0, 0, 0), base_color);
				set_box(Vector3(28, 1, 24), Vector3(28, 2, 8), Vector3(0, 0, 0), base_color);
				set_box(Vector3(28, 0, 20), Vector3(28, 2, 8), Vector3(0, 0, 0), base_color);
				set_box(Vector3(28, -1, 16), Vector3(28, 2, 8), Vector3(0, 0, 0), base_color);

			}

			set_buttan(Vector3(74, 11.2f, 76), Vector3(4, 0.9f, 4), Vector3(0, 0, 0), Stage_parts::Stageparts_tags::Flag_0, nullptr, false);
			set_rotate_block(Vector3(102, 25.0f, 71), Vector3(5, 15, 15), Vector3(0, 90, 0), Stage_parts::Stageparts_tags::Flag_0, Vector3(0, 0, -1), 1, 1, base_color,nullptr);
		}

#else

		// first_zone
		{
			Gameobject* first_zone = Gameobject_manager::create("first_zone");
			stage_parts.emplace_back(first_zone);
			set_box(Vector3(25, -2, 10), Vector3(65, 2, 30), Vector3(0), Vector3(188, 214, 54) / 255.0f * 0.8f * 0.9f, first_zone);
			// 階段
			{

				Gameobject* stairs_pearent = Gameobject_manager::create("stairs_pearent");
				first_zone->add_child(stairs_pearent);
				set_box(Vector3(-15, 2, 23), Vector3(15, 2, 15), Vector3(0, 0, 0), base_color, stairs_pearent);
				set_box(Vector3(28, 2, 29.5f), Vector3(28, 2, 8.5f), Vector3(0, 0, 0), base_color, stairs_pearent);
				set_box(Vector3(28, 1, 24), Vector3(28, 2, 8), Vector3(0, 0, 0), base_color, stairs_pearent);
				set_box(Vector3(28, 0, 20), Vector3(28, 2, 8), Vector3(0, 0, 0), base_color, stairs_pearent);
				set_box(Vector3(28, -1, 16), Vector3(28, 2, 8), Vector3(0, 0, 0), base_color, stairs_pearent);

			}

			// 壁
			{
				Gameobject* wallAnddoor_pearent = Gameobject_manager::create("wallAnddoor_pearent");
				first_zone->add_child(wallAnddoor_pearent);

				set_box(Vector3(-9, 10, 39), Vector3(31, 10, 1), Vector3(0), base_color, wallAnddoor_pearent);
				set_box(Vector3(60, 10, 39), Vector3(30, 10, 1), Vector3(0), base_color, wallAnddoor_pearent);
				set_box(Vector3(26, 16, 39), Vector3(4, 4, 1), Vector3(0), base_color, wallAnddoor_pearent);
				set_box(Vector3(26, 3, 39), Vector3(4, 1, 1), Vector3(0), base_color, wallAnddoor_pearent);

				set_box(Vector3(18, 12, 37), Vector3(2, 8, 1), Vector3(0), base_color * 0.9f, wallAnddoor_pearent);
				set_box(Vector3(34, 12, 37), Vector3(2, 8, 1), Vector3(0), base_color * 0.9f, wallAnddoor_pearent);

				set_door(Vector3(24, 8, 39.25f), Vector3(2, 4, 0.5f), Vector3(0), true, wallAnddoor_pearent);
				set_door(Vector3(28, 8, 39.25f), Vector3(2, 4, 0.5f), Vector3(0), false, wallAnddoor_pearent);
			}


			// 柱
			{
				Gameobject* pillars_pearent = Gameobject_manager::create("pillars");
				first_zone->add_child(pillars_pearent);
				set_pillar(Vector3(-02, 4, 23), Vector3(1), Vector3(0), pillars_pearent, 2.5f, 5, false);
				set_pillar(Vector3(-02, 4, 10), Vector3(1), Vector3(0), pillars_pearent, 2.5f, 5, false);
				set_pillar(Vector3(-15, 4, 10), Vector3(1), Vector3(0), pillars_pearent, 2.5f, 5, false);
				set_pillar(Vector3(-28, 4, 10), Vector3(1), Vector3(0), pillars_pearent, 2.5f, 5, false);
				set_pillar(Vector3(-28, 4, 23), Vector3(1), Vector3(0), pillars_pearent, 2.5f, 5, false);
				set_pillar(Vector3(-28, 4, 36), Vector3(1), Vector3(0), pillars_pearent, 2.5f, 5, false);

				set_pillar(Vector3(16, 4, 23), Vector3(1), Vector3(0), pillars_pearent);
				set_pillar(Vector3(36, 4, 23), Vector3(1), Vector3(0), pillars_pearent);
				set_pillar(Vector3(54, 4, 23), Vector3(1), Vector3(0), pillars_pearent);
				set_pillar(Vector3(54, 4, 36), Vector3(1), Vector3(0), pillars_pearent);
			}

			// 柵
			{
				Gameobject* fences_pearent = Gameobject_manager::create("fences");
				first_zone->add_child(fences_pearent);
				set_fence(Vector3(-2, 4, 16.5f), Vector3(1), Vector3(0, 90, 0), fences_pearent);
				set_fence(Vector3(-21.5f, 4, 10), Vector3(1), Vector3(0, 0, 0), fences_pearent);
				set_fence(Vector3(-8.50f, 4, 10), Vector3(1), Vector3(0, 0, 0), fences_pearent);
				set_fence(Vector3(-28, 4, 28.5f), Vector3(1), Vector3(0, 90, 0), fences_pearent);
				set_fence(Vector3(-28, 4, 15.5f), Vector3(1), Vector3(0, 90, 0), fences_pearent);
			}

			// 木
			{
				Gameobject* trees_pearent = Gameobject_manager::create("trees");
				first_zone->add_child(trees_pearent);
				set_tree(Vector3(-35, 0, 32), Vector3(1, 1.2f, 1), Vector3(0, 02, 0), 1.2f, 0.8f, trees_pearent);
				set_tree(Vector3(-35, 0, 22), Vector3(1, 1.2f, 1), Vector3(0, 76, 0), 1.2f, 0.8f, trees_pearent);
				set_tree(Vector3(-35, 0, 12), Vector3(1, 1.2f, 1), Vector3(0, 12, 0), 1.2f, 0.8f, trees_pearent);
				set_tree(Vector3(-35, 0, +2), Vector3(1, 1.2f, 1), Vector3(0, 92, 0), 1.2f, 0.8f, trees_pearent);

				set_tree(Vector3(63, 0, 32), Vector3(1, 1.2f, 1), Vector3(0, 02, 0), 2.5f, 0.8f, trees_pearent);
				set_tree(Vector3(73, 0, 32), Vector3(1, 1.2f, 1), Vector3(0, 76, 0), 2.5f, 0.8f, trees_pearent);
				set_tree(Vector3(83, 0, 32), Vector3(1, 1.2f, 1), Vector3(0, 12, 0), 2.5f, 0.8f, trees_pearent);
			}

			// 適当なピラミッド
			{
				Gameobject* tutrial_block = Gameobject_manager::create("pyramid");
				first_zone->add_child(tutrial_block);

				int phyramid_size = 6;
				Vector3 base_size = Vector3(1, 1, 1.5f);
				Vector3 base_pos = Vector3(52, 0, -4);

				for (int y = 0; y < phyramid_size; ++y) {
					for (int x = 0; x < phyramid_size - y; ++x) {
						for (int z = 0; z < phyramid_size - y; ++z) {

							auto coll = set_box(base_pos + Vector3(
								(base_size.x * 2 + 0.2f) * x - ((base_size.x * 2 + 0.2f) * (phyramid_size - y) * 0.5f),
								(base_size.y * 2 + 0) * y + base_size.y,
								(base_size.z * 2 + 0.2f) * z - ((base_size.z * 2 + 0.2f) * (phyramid_size - y) * 0.5f)

							), base_size, Vector3(0, 0, 0), moveable_green, tutrial_block, false);
							coll->physics_data.inertial_mass = 5;
							coll->physics_data.angula_sleep_threrhold = 0;
						}
					}
				}
			}


		}

		{}

		// second_zone
		{
			Gameobject* second_zone = Gameobject_manager::create("second_zone");
			stage_parts.emplace_back(second_zone);
			// 床
			{
				Gameobject* stairs_pearent = Gameobject_manager::create("stairs_pearent");
				second_zone->add_child(stairs_pearent);
				set_box(Vector3(47.0f, -1.0f, 70.0f), Vector3(37, 05, 30), Vector3(0), base_color, stairs_pearent);
				set_box(Vector3(61.0f, +2.0f, 70.0f), Vector3(23, 10, 30), Vector3(0), base_color, stairs_pearent);
				set_box(Vector3(17.0f, 6.0f, 93.0f), Vector3(7.0f, 2.0f, 7.0f), Vector3(0), base_color, stairs_pearent);
				set_box(Vector3(26.4f, 6.5f, 93.0f), Vector3(2.4f, 2.5f, 7.0f), Vector3(0), base_color, stairs_pearent);
				set_box(Vector3(31.2f, 7.0f, 93.0f), Vector3(2.4f, 3.0f, 7.0f), Vector3(0), base_color, stairs_pearent);
				set_box(Vector3(36.0f, 7.5f, 93.0f), Vector3(2.4f, 3.5f, 7.0f), Vector3(0), base_color, stairs_pearent);
				set_box(Vector3(17.0f, 4.5f, 74.0f), Vector3(7.0f, 0.5f, 2.4f), Vector3(0), base_color, stairs_pearent);
				set_box(Vector3(17.0f, 5.0f, 78.8f), Vector3(7.0f, 1.0f, 2.4f), Vector3(0), base_color, stairs_pearent);
				set_box(Vector3(17.0f, 5.5f, 83.6f), Vector3(7.0f, 1.5f, 2.4f), Vector3(0), base_color, stairs_pearent);
			}

			// arch
			{
				Gameobject* arch_pearent = Gameobject_manager::create("arch_pearent");
				second_zone->add_child(arch_pearent);
				set_box(Vector3(11, 15.5f, 85.0f), Vector3(1, 8.5f, 1), Vector3(0), base_color, arch_pearent);
				set_box(Vector3(25, 14.0f, 85.0f), Vector3(1, 10.f, 1), Vector3(0), base_color, arch_pearent);
				set_box(Vector3(39, 18.0f, 85.0f), Vector3(1, 6.0f, 1), Vector3(0), base_color, arch_pearent);
				set_box(Vector3(53, 18.0f, 85.0f), Vector3(1, 6.0f, 1), Vector3(0), base_color, arch_pearent);
				set_box(Vector3(67, 18.0f, 85.0f), Vector3(1, 6.0f, 1), Vector3(0), base_color, arch_pearent);
				set_box(Vector3(81, 18.0f, 85.0f), Vector3(1, 6.0f, 1), Vector3(0), base_color, arch_pearent);

				set_box(Vector3(46, 26.0f, 85.0f), Vector3(36, 2.0f, 1), Vector3(0), base_color, arch_pearent);
			}

			// 壁
			if (0)
			{
				Gameobject* wall_pearent = Gameobject_manager::create("wall_pearent");
				second_zone->add_child(wall_pearent);
				set_box(Vector3(83, 20.0f, 84.0f), Vector3(1, 8.0f, 16), Vector3(0), base_color, wall_pearent);
				set_box(Vector3(83, 20.0f, 50.0f), Vector3(1, 8.0f, 10), Vector3(0), base_color, wall_pearent);
				set_box(Vector3(83, 24.0f, 64.0f), Vector3(1, 4.0f, 4), Vector3(0), base_color, wall_pearent);

				set_gimmickdoor(Vector3(211, 19, 64), Vector3(210, 19, 64),
					Vector3(0), Vector3(0),
					2, 0,
					Vector3(2, 4.0f, 0.5f),
					Stage_parts::Stageparts_tags::Flag_0,
					base_color * 0.8f,
					wall_pearent
				);

				set_gimmickdoor(Vector3(209, 19, 64), Vector3(210, 19, 64),
					Vector3(0), Vector3(0),
					2, 0,
					Vector3(2, 4.0f, 0.5f),
					Stage_parts::Stageparts_tags::Flag_0,
					base_color * 0.8f,
					wall_pearent
				);

			}

			// 適当なピラミッド
			{
				Gameobject* tutrial_block = Gameobject_manager::create("pyramid");
				second_zone->add_child(tutrial_block);

				int phyramid_size = 6;
				Vector3 base_size = Vector3(1.5f, 1, 1);
				Vector3 base_pos = Vector3(53, 12, 76);

				for (int y = 0; y < phyramid_size; ++y) {
					for (int x = 0; x < phyramid_size - y; ++x) {
						for (int z = 0; z < phyramid_size - y; ++z) {

							auto coll = set_box(base_pos + Vector3(
								(base_size.x * 2 + 0.2f) * x - ((base_size.x * 2 + 0.2f) * (phyramid_size - y) * 0.5f),
								(base_size.y * 2 + 0) * y + base_size.y,
								(base_size.z * 2 + 0.2f) * z - ((base_size.z * 2 + 0.2f) * (phyramid_size - y) * 0.5f)

							), base_size, Vector3(0, 0, 0), moveable_red, tutrial_block, false);
							coll->physics_data.inertial_mass = 5;
							coll->physics_data.angula_sleep_threrhold = 0;
						}
					}
				}
			}


			// 回転gimmick
			{
				Gameobject* rotate_gimmick = Gameobject_manager::create("rotate_gimmick");
				second_zone->add_child(rotate_gimmick);

				set_buttan(Vector3(74, 11.2f, 76), Vector3(4, 0.9f, 4), Vector3(0, 0, 0), Stage_parts::Stageparts_tags::Flag_0, rotate_gimmick, false);
				set_rotate_block(Vector3(102, 25.0f, 71), Vector3(5, 15, 15), Vector3(0, 90, 0), Stage_parts::Stageparts_tags::Flag_0, Vector3(0, 0, -1), 1, 1, base_color, rotate_gimmick);
			}

			// 体を持ち上げるtutrial
			{
				Gameobject* tute_up = Gameobject_manager::create("tute_up");
				second_zone->add_child(tute_up);

				set_box(Vector3(133, 6.5f, 137), Vector3(11, 4.5f, 6), Vector3(0), wood_color, tute_up);
				set_box(Vector3(150, 9.4f, 103), Vector3(6, 7.4f, 40), Vector3(0), base_color, tute_up);
				set_box(Vector3(139, 7, 97), Vector3(17, 5, 34), Vector3(0), base_color, tute_up);
				set_box(Vector3(168, 9.4f, 123), Vector3(12, 7.4f, 20), Vector3(0), base_color, tute_up);


				set_box(Vector3(172, 17, 122), Vector3(16, 6, 5), Vector3(0), grass_color, tute_up);
				set_box(Vector3(205, 17, 122), Vector3(17, 6, 5), Vector3(0), grass_color, tute_up);
				set_box(Vector3(172, 17, 85), Vector3(16, 6, 20), Vector3(0), grass_color, tute_up);
				set_box(Vector3(167, 17, 111), Vector3(11, 6, 6), Vector3(0), grass_color, tute_up);
				set_box(Vector3(214, 20.4f, 135), Vector3(8, 3.6f, 8), Vector3(0), grass_color, tute_up);
				set_box(Vector3(201, 9.4f, 135), Vector3(21, 7.4f, 8), Vector3(0), base_color, tute_up);
				set_box(Vector3(205, 13.5f, 84), Vector3(17, 1.5f, 21), Vector3(0), wood_color, tute_up);

				// 階段
				set_box(Vector3(182, 16.5f, 111), Vector3(4, 5.5f, 6), Vector3(0), base_color, tute_up);
				set_box(Vector3(190, 16.0f, 111), Vector3(4, 5.0f, 6), Vector3(0), base_color, tute_up);
				set_box(Vector3(198, 15.5f, 111), Vector3(4, 4.5f, 6), Vector3(0), base_color, tute_up);
				set_box(Vector3(206, 15.0f, 111), Vector3(4, 4.0f, 6), Vector3(0), base_color, tute_up);
				set_box(Vector3(216, 14.5f, 111), Vector3(6, 3.5f, 6), Vector3(0), base_color, tute_up);
				set_box(Vector3(216, 16.0f, 101), Vector3(6, 1.0f, 4), Vector3(0), base_color, tute_up);
				set_box(Vector3(216, 15.5f, 93), Vector3(6, 0.5f, 4), Vector3(0), base_color, tute_up);
				//set_box(Vector3(216,13.0f,70), Vector3(6, 1.0f, 4), Vector3(0), moveable_red);
				//set_box(Vector3(216,12.5f,62), Vector3(6, 0.5f, 4), Vector3(0), moveable_red);

				set_box(Vector3(227, 11, 64), Vector3(13, 12, 1), Vector3(0), base_color, tute_up);
				set_box(Vector3(181, 17, 64), Vector3(25, 6, 1), Vector3(0), base_color, tute_up);
				set_box(Vector3(198, 27, 64), Vector3(42, 4, 1), Vector3(0), base_color, tute_up);

				set_buttan(Vector3(199, 19, 64.4f), Vector3(2, 0.9f, 2), Vector3(90, 0, 0), Stage_parts::Stageparts_tags::Flag_2, tute_up, true);
				set_gimmickdoor(Vector3(212, 19, 64), Vector3(214, 19, 64),
					Vector3(0), Vector3(0),
					2, 0,
					Vector3(2, 4.0f, 0.5f),
					Stage_parts::Stageparts_tags::Flag_2,
					base_color * 0.8f,
					tute_up
				);

				set_gimmickdoor(Vector3(208, 19, 64), Vector3(206, 19, 64),
					Vector3(0), Vector3(0),
					2, 0,
					Vector3(2, 4.0f, 0.5f),
					Stage_parts::Stageparts_tags::Flag_2,
					base_color * 0.8f,
					tute_up
				);

				set_tree(Vector3(139, 11, 137), Vector3(1, 1.2f, 1), Vector3(0, 12, 0), 1.2f, 0.8f, tute_up);
				set_tree(Vector3(128, 11, 137), Vector3(1, 1.2f, 1), Vector3(0, 12, 0), 1.2f, 0.8f, tute_up);
				set_tree(Vector3(214, 24, 135), Vector3(1, 1.2f, 1), Vector3(0, 12, 0), 1.2f, 0.8f, tute_up);
				set_tree(Vector3(193, 15, 78), Vector3(1, 1.2f, 1), Vector3(0, 12, 0), 1.2f, 0.8f, tute_up);
				set_tree(Vector3(193, 15, 88), Vector3(1, 1.2f, 1), Vector3(0, 12, 0), 1.2f, 0.8f, tute_up);
				set_tree(Vector3(193, 15, 98), Vector3(1, 1.2f, 1), Vector3(0, 12, 0), 1.2f, 0.8f, tute_up);

				set_fence(Vector3(186, 23, 71), Vector3(1), Vector3(0, 90, 0), tute_up);
				set_fence(Vector3(186, 23, 83), Vector3(1), Vector3(0, 90, 0), tute_up);
				set_fence(Vector3(186, 23, 95), Vector3(1), Vector3(0, 90, 0), tute_up);
				set_pillar(Vector3(186, 23, 67), Vector3(1), Vector3(0), tute_up, 2.5f, 5, false);
				set_pillar(Vector3(186, 23, 103), Vector3(1), Vector3(0), tute_up, 2.5f, 5, false);

				set_fence(Vector3(166, 23, 125), Vector3(1), Vector3(0, 0, 0), tute_up);
				set_fence(Vector3(178, 23, 125), Vector3(1), Vector3(0, 0, 0), tute_up);
				set_fence(Vector3(190, 23, 125), Vector3(1), Vector3(0, 0, 0), tute_up);
				set_fence(Vector3(202, 23, 125), Vector3(1), Vector3(0, 0, 0), tute_up);
				set_pillar(Vector3(158, 23, 125), Vector3(1), Vector3(0), tute_up, 2.5f, 5, false);
				set_pillar(Vector3(208, 23, 125), Vector3(1), Vector3(0), tute_up, 2.5f, 5, false);


				set_fence(Vector3(146, 16.8, 133), Vector3(1), Vector3(0, 90, 0), tute_up);
				set_pillar(Vector3(146, 16.8f, 125), Vector3(1), Vector3(0), tute_up, 2.5f, 5, false);
				set_pillar(Vector3(146, 16.8f, 141), Vector3(1), Vector3(0), tute_up, 2.5f, 5, false);

				// 適当なピラミッド
				{
					Gameobject* tutrial_block = Gameobject_manager::create("pyramid");
					second_zone->add_child(tutrial_block);
					tutrial_block->transform->local_pos = Vector3(138, 12, 116);

					int phyramid_size = 4;
					Vector3 base_size = Vector3(2, 1.5f, 2);
					Vector3 base_pos = Vector3(0);

					for (int y = 0; y < phyramid_size; ++y) {
						for (int x = 0; x < phyramid_size - y; ++x) {
							for (int z = 0; z < phyramid_size - y; ++z) {

								auto coll = set_box(base_pos + Vector3(
									(base_size.x * 2 + 0.2f) * x - ((base_size.x * 2 + 0.2f) * (phyramid_size - y) * 0.5f),
									(base_size.y * 2 + 0) * y + base_size.y,
									(base_size.z * 2 + 0.2f) * z - ((base_size.z * 2 + 0.2f) * (phyramid_size - y) * 0.5f)

								), base_size, Vector3(0, 0, 0), moveable_blue, tutrial_block, false);
								coll->physics_data.inertial_mass = 8;
								coll->physics_data.angula_sleep_threrhold = 0;
							}
						}
					}
				}
			}


		}

		// third_zone
		{
			Gameobject* third_zone = Gameobject_manager::create("third_zone");
			stage_parts.emplace_back(third_zone);
			third_zone->transform->local_pos = Vector3(140, 3, 147);
			third_zone->transform->local_orient = quaternion_from_euler(0, 90, 0);

			// 階段
			{
				Gameobject* stairs_pearent = Gameobject_manager::create("stairs_pearent");
				third_zone->add_child(stairs_pearent);

				set_box(Vector3(92.0f, 10.f, 69.0f), Vector3(8, 2, 11), Vector3(0), base_color, stairs_pearent);
				set_box(Vector3(89.0f, 4.0f, 49.0f), Vector3(7, 8, 9), Vector3(0), base_color, stairs_pearent);
				set_box(Vector3(98.00f, 3.5f, 49.0f), Vector3(2, 7.5f, 9), Vector3(0), base_color, stairs_pearent);
				set_box(Vector3(102.0f, 3.0f, 49.0f), Vector3(2, 7.0f, 9), Vector3(0), base_color, stairs_pearent);
				set_box(Vector3(106.0f, 2.5f, 49.0f), Vector3(2, 6.5f, 9), Vector3(0), base_color, stairs_pearent);

				set_box(Vector3(92.0f, 12.f, 81.0f), Vector3(8, 4.5f, 1), Vector3(0), base_color, stairs_pearent);
				set_box(Vector3(101.f, 12.f, 70.0f), Vector3(1, 4.5f, 12), Vector3(0), base_color, stairs_pearent);
			}

			// 床
			{
				Gameobject* floor_pearent = Gameobject_manager::create("floor_pearent");
				third_zone->add_child(floor_pearent);
				set_box(Vector3(134, 0, 13), Vector3(28, 4, 11), Vector3(0), base_color, floor_pearent);
				set_box(Vector3(128, 2, 32), Vector3(22, 6, 8), Vector3(0), grass_color, floor_pearent);
				set_box(Vector3(128, 2, 49), Vector3(22, 6, 9), Vector3(0), base_color, floor_pearent);
				set_box(Vector3(117, 2, 78), Vector3(33, 6, 20), Vector3(0), grass_color, floor_pearent);
				set_box(Vector3(83.5f, 10, 90.0f), Vector3(1, 2, 8), Vector3(0), base_color, floor_pearent);

				set_box(Vector3(156, 2.0f, 69), Vector3(6, 6, 29), Vector3(0), base_color, floor_pearent);

				set_box(Vector3(156, 2.0f, 38), Vector3(6, 6.0f, 2), Vector3(0), base_color, floor_pearent);
				set_box(Vector3(156, 1.5f, 34), Vector3(6, 5.5f, 2), Vector3(0), base_color, floor_pearent);
				set_box(Vector3(156, 1.0f, 30), Vector3(6, 5.0f, 2), Vector3(0), base_color, floor_pearent);
				set_box(Vector3(156, 0.5f, 26), Vector3(6, 4.5f, 2), Vector3(0), base_color, floor_pearent);
				set_box(Vector3(145, 10, 90), Vector3(17, 2, 8), Vector3(0), base_color, floor_pearent);

				set_box(Vector3(156, 9.5f, 80), Vector3(6, 1.5f, 2), Vector3(0), base_color, floor_pearent);
				set_box(Vector3(156, 9.0f, 76), Vector3(6, 1.0f, 2), Vector3(0), base_color, floor_pearent);
				set_box(Vector3(156, 8.5f, 72), Vector3(6, 0.5f, 2), Vector3(0), base_color, floor_pearent);
			}

			// 柱 と上の棒
			{
				Gameobject* pillar_pearent = Gameobject_manager::create("pillar_pearent");
				third_zone->add_child(pillar_pearent);

				set_pillar(Vector3(110.0f, 8, 30), Vector3(1), Vector3(0), pillar_pearent);
				set_pillar(Vector3(128.0f, 8, 30), Vector3(1), Vector3(0), pillar_pearent);
				set_pillar(Vector3(146.0f, 8, 30), Vector3(1), Vector3(0), pillar_pearent);

				set_pillar(Vector3(110.0f, 8, 62), Vector3(1), Vector3(0), pillar_pearent);
				set_pillar(Vector3(128.0f, 8, 62), Vector3(1), Vector3(0), pillar_pearent);
				set_pillar(Vector3(146.0f, 8, 62), Vector3(1), Vector3(0), pillar_pearent);

				set_pillar(Vector3(110.0f, 8, 76), Vector3(1), Vector3(0), pillar_pearent);
				set_pillar(Vector3(128.0f, 8, 76), Vector3(1), Vector3(0), pillar_pearent);
				set_pillar(Vector3(146.0f, 8, 76), Vector3(1), Vector3(0), pillar_pearent);

				set_box(Vector3(110.0f, 24.5f, 53), Vector3(0.5f, 0.5f, 26), Vector3(0), wood_color, pillar_pearent, false);
				set_box(Vector3(128.0f, 24.5f, 53), Vector3(0.5f, 0.5f, 26), Vector3(0), wood_color, pillar_pearent, false);
				set_box(Vector3(146.0f, 24.5f, 53), Vector3(0.5f, 0.5f, 26), Vector3(0), wood_color, pillar_pearent, false);

				set_box(Vector3(128.0f, 25.5f, 30), Vector3(21, 0.5f, 0.5f), Vector3(0), wood_color, pillar_pearent, false);
				set_box(Vector3(128.0f, 25.5f, 62), Vector3(21, 0.5f, 0.5f), Vector3(0), wood_color, pillar_pearent, false);
				set_box(Vector3(128.0f, 25.5f, 76), Vector3(21, 0.5f, 0.5f), Vector3(0), wood_color, pillar_pearent, false);
			}

			// 木
			{
				Gameobject* tree_pearent = Gameobject_manager::create("tree_pearent");
				third_zone->add_child(tree_pearent);

				set_tree(Vector3(92, 8, 90), Vector3(1, 1.2f, 1), Vector3(0, 02, 0), 2.5f, 0.8f, tree_pearent);
				set_tree(Vector3(157, 4, 8), Vector3(1, 1.2f, 1), Vector3(0, 76, 0), 1.2f, 0.8f, tree_pearent);
				set_tree(Vector3(148, 4, 8), Vector3(1, 1.2f, 1), Vector3(0, 12, 0), 1.2f, 0.8f, tree_pearent);
			}

			// 壁
			{
				Gameobject* wall_pearent = Gameobject_manager::create("wall_pearent");
				third_zone->add_child(wall_pearent);

				set_box(Vector3(144, 10, 99), Vector3(4, 2, 1), Vector3(0), base_color, wall_pearent);
				set_box(Vector3(144, 24, 99), Vector3(4, 4, 1), Vector3(0), base_color, wall_pearent);
				set_box(Vector3(155, 18, 99), Vector3(7, 10, 1), Vector3(0), base_color, wall_pearent);
				set_box(Vector3(112, 18, 99), Vector3(28, 10, 1), Vector3(0), base_color, wall_pearent);
				set_box(Vector3(123, 2, 99), Vector3(39, 6, 1), Vector3(0), base_color, wall_pearent);

				set_gimmickdoor(Vector3(146, 16, 99), Vector3(149.5f, 16, 99),
					Vector3(0), Vector3(0),
					2, 0,
					Vector3(2, 4.0f, 0.5f),
					Stage_parts::Stageparts_tags::Flag_3,
					base_color * 0.8f,
					wall_pearent
				);

				set_gimmickdoor(Vector3(142, 16, 99), Vector3(138.5f, 16, 99),
					Vector3(0), Vector3(0),
					2, 0,
					Vector3(2, 4.0f, 0.5f),
					Stage_parts::Stageparts_tags::Flag_3,
					base_color * 0.8f,
					wall_pearent
				);


				set_buttan(Vector3(135, 16, 98.8f), Vector3(2, 0.9f, 2), Vector3(-90, 0, 0), Stage_parts::Stageparts_tags::Flag_1, wall_pearent, true);
				set_buttan(Vector3(153, 16, 98.8f), Vector3(2, 0.9f, 2), Vector3(-90, 0, 0), Stage_parts::Stageparts_tags::Flag_2, wall_pearent, true);

				set_Andcircuit(Stage_parts::Stageparts_tags::Flag_1, Stage_parts::Stageparts_tags::Flag_2, Stage_parts::Stageparts_tags::Flag_3, wall_pearent);

			}


		}

		{}

		// fourth_zone
		{
			Gameobject* fourth_zone = Gameobject_manager::create("fourth_zone");
			stage_parts.emplace_back(fourth_zone);
			fourth_zone->transform->local_pos = Vector3(140, 3, 147);
			fourth_zone->transform->local_orient = quaternion_from_euler(0, 90, 0);

			// 橋
			{
				Gameobject* bridge_pearent = Gameobject_manager::create("bridge_pearent");
				fourth_zone->add_child(bridge_pearent);

				set_box(Vector3(144, 10.81f, 101.89f), Vector3(5, 1, 2), Vector3(5.5f, 0, 0), base_color, bridge_pearent);
				set_box(Vector3(144, 10.23f, 105.74f), Vector3(5, 1, 2), Vector3(11.5f, 0, 0), base_color, bridge_pearent);
				set_box(Vector3(144, 9.19f, 109.44f), Vector3(5, 1, 2), Vector3(20.5f, 0, 0), base_color, bridge_pearent);
				set_box(Vector3(144, 7.58f, 112.89f), Vector3(5, 1, 2), Vector3(30, 0, 0), base_color, bridge_pearent);
				set_box(Vector3(144, 5.71f, 115.63f), Vector3(5, 1, 1.5f), Vector3(40, 0, 0), base_color, bridge_pearent);

			}

			// 床
			{
				Gameobject* floor_pearent = Gameobject_manager::create("floor_pearent");
				fourth_zone->add_child(floor_pearent);

				set_box(Vector3(110, 5, 119), Vector3(40, 1, 7), Vector3(0), roof_color, floor_pearent);
				set_box(Vector3(104, 5, 136), Vector3(20, 1, 10), Vector3(0), wood_color, floor_pearent);

				//上
				set_box(Vector3(104, 8, 154), Vector3(34, 4, 8), Vector3(0), stair_color, floor_pearent);
				set_box(Vector3(104, 12.5f, 154), Vector3(18, 0.5f, 8), Vector3(0), stair_color, floor_pearent);
				set_box(Vector3(104, 13.0f, 172), Vector3(34, 1, 10), Vector3(0), roof_color, floor_pearent);
				set_box(Vector3(104, 13.5f, 154), Vector3(14, 0.5f, 8), Vector3(0), roof_color, floor_pearent);
				set_box(Vector3(104, 13.5f, 199), Vector3(18, 0.5f, 17), Vector3(0), roof_color, floor_pearent);
				set_box(Vector3(104, 8.0f, 172), Vector3(34, 4.0f, 10), Vector3(0), wood_color, floor_pearent);
				set_box(Vector3(104, 8.5f, 186), Vector3(34, 4.5f, 4), Vector3(0), wood_color, floor_pearent);
				set_box(Vector3(104, 8.5f, 203), Vector3(26, 4.5f, 13), Vector3(0), wood_color, floor_pearent);

				set_box(Vector3(82, 13.5f, 213), Vector3(4, 0.5f, 3), Vector3(0), roof_color, floor_pearent);
				set_box(Vector3(126, 13.5f, 213), Vector3(4, 0.5f, 3), Vector3(0), roof_color, floor_pearent);
			}

			// 像
			{
				Gameobject* statue_pearent = Gameobject_manager::create("statue_pearent");
				fourth_zone->add_child(statue_pearent);

				set_box(Vector3(104, 8.5f, 132), Vector3(4, 0.5f, 3), Vector3(0), base_color, statue_pearent);
				set_box(Vector3(104, 7, 132), Vector3(5, 1, 5), Vector3(0), base_color, statue_pearent);

				auto go = set_player_statue(Vector3(104, 17.6f, 132), 2, Vector3(0));
				statue_pearent->add_child(go);
			}

			// 階段
			{
				Gameobject* stair_pearent = Gameobject_manager::create("stair_pearent");
				fourth_zone->add_child(stair_pearent);
				const Vector3 color = Vector3(255, 255, 255) / 255.0f;
				const Vector3 roof_color = Vector3(233, 225, 217) / 255.0f;
				const Vector3 stair_color = Vector3(215, 205, 197) / 255.0f;

				// 階段
				set_box(Vector3(130, 5.5f, 128), Vector3(8, 1.5f, 2), Vector3(0), stair_color, stair_pearent);
				set_box(Vector3(130, 6.0f, 132), Vector3(8, 2.0f, 2), Vector3(0), stair_color, stair_pearent);
				set_box(Vector3(130, 6.5f, 136), Vector3(8, 2.5f, 2), Vector3(0), stair_color, stair_pearent);
				set_box(Vector3(130, 7.0f, 140), Vector3(8, 3.0f, 2), Vector3(0), stair_color, stair_pearent);
				set_box(Vector3(130, 7.5f, 144), Vector3(8, 3.5f, 2), Vector3(0), stair_color, stair_pearent);
				// 階段
				set_box(Vector3(78, 5.5f, 128), Vector3(8, 1.5f, 2), Vector3(0), stair_color, stair_pearent);
				set_box(Vector3(78, 6.0f, 132), Vector3(8, 2.0f, 2), Vector3(0), stair_color, stair_pearent);
				set_box(Vector3(78, 6.5f, 136), Vector3(8, 2.5f, 2), Vector3(0), stair_color, stair_pearent);
				set_box(Vector3(78, 7.0f, 140), Vector3(8, 3.0f, 2), Vector3(0), stair_color, stair_pearent);
				set_box(Vector3(78, 7.5f, 144), Vector3(8, 3.5f, 2), Vector3(0), stair_color, stair_pearent);
			}

			// 柱
			{
				Gameobject* pillar_pearent = Gameobject_manager::create("pillar_pearent");
				fourth_zone->add_child(pillar_pearent);

				set_pillar(Vector3(120, 13, 148), Vector3(1), Vector3(0), pillar_pearent, 0.5f, 7.0f, false);
				set_pillar(Vector3(136, 12, 148), Vector3(1), Vector3(0), pillar_pearent);
				set_pillar(Vector3(136, 14, 164), Vector3(1), Vector3(0), pillar_pearent);
				set_pillar(Vector3(136, 14, 180), Vector3(1), Vector3(0), pillar_pearent);
				set_pillar(Vector3(120, 14, 180), Vector3(1), Vector3(0), pillar_pearent);
				set_pillar(Vector3(120, 14, 196), Vector3(1), Vector3(0), pillar_pearent);
				set_pillar(Vector3(120, 14, 212), Vector3(1), Vector3(0), pillar_pearent);

				set_pillar(Vector3(88, 13, 148), Vector3(1), Vector3(0), pillar_pearent, 0.5f, 7.0f, false);
				set_pillar(Vector3(72, 12, 148), Vector3(1), Vector3(0), pillar_pearent);
				set_pillar(Vector3(72, 14, 164), Vector3(1), Vector3(0), pillar_pearent);
				set_pillar(Vector3(72, 14, 180), Vector3(1), Vector3(0), pillar_pearent);
				set_pillar(Vector3(88, 14, 180), Vector3(1), Vector3(0), pillar_pearent);
				set_pillar(Vector3(88, 14, 196), Vector3(1), Vector3(0), pillar_pearent);
				set_pillar(Vector3(88, 14, 212), Vector3(1), Vector3(0), pillar_pearent);
			}

			// 壁
			{
				Gameobject* wall_pearent = Gameobject_manager::create("wall_pearent");
				fourth_zone->add_child(wall_pearent);
				const Vector3 color = Vector3(255, 255, 255) / 255.0f;

				set_box(Vector3(93, 18, 215), Vector3(7, 4, 1), Vector3(0), base_color, wall_pearent);
				set_box(Vector3(115, 18, 215), Vector3(7, 4, 1), Vector3(0), base_color, wall_pearent);
				set_box(Vector3(104, 26, 215), Vector3(18, 4, 1), Vector3(0), base_color, wall_pearent);

				set_box(Vector3(104, 9, 216.05f), Vector3(26, 5, 0.05f), Vector3(0), color, wall_pearent);
				set_box(Vector3(93, 18, 216.05f), Vector3(7, 4, 0.05f), Vector3(0), color, wall_pearent);
				set_box(Vector3(115, 18, 216.05f), Vector3(7, 4, 0.05f), Vector3(0), color, wall_pearent);
				set_box(Vector3(104, 26, 216.05f), Vector3(18, 4, 0.05f), Vector3(0), color, wall_pearent);
				set_box(Vector3(104, 13.5f, 217), Vector3(4, 0.5f, 1), Vector3(0), color, wall_pearent);

				set_door(Vector3(102, 18, 215), Vector3(2, 4, 0.5f), Vector3(0), true, wall_pearent);
				set_door(Vector3(106, 18, 215), Vector3(2, 4, 0.5f), Vector3(0), false, wall_pearent);
			}

			// 柵
			{
				Gameobject* fence_pearent = Gameobject_manager::create("fence_pearent");
				fourth_zone->add_child(fence_pearent);

				set_fence(Vector3(95, 14, 148.05f), Vector3(1), Vector3(0, 0, 0), fence_pearent);
				set_fence(Vector3(104, 14, 148.05f), Vector3(1), Vector3(0, 0, 0), fence_pearent);
				set_fence(Vector3(113, 14, 148.05f), Vector3(1), Vector3(0, 0, 0), fence_pearent);
			}

			// 木
			{
				Gameobject* tree_pearent = Gameobject_manager::create("tree_pearent");
				fourth_zone->add_child(tree_pearent);

				set_tree(Vector3(82, 14, 213), Vector3(1, 1.2f, 1), Vector3(0), 2.5f, 0.5f, tree_pearent);
				set_tree(Vector3(82, 13, 199), Vector3(1, 1.2f, 1), Vector3(0), 2.5f, 0.5f, tree_pearent);
				set_tree(Vector3(82, 13, 186), Vector3(1, 1.2f, 1), Vector3(0), 2.5f, 0.5f, tree_pearent);
				set_tree(Vector3(74, 13, 186), Vector3(1, 1.2f, 1), Vector3(0), 2.5f, 0.5f, tree_pearent);

				set_tree(Vector3(126, 14, 213), Vector3(1, 1.2f, 1), Vector3(0), 2.5f, 0.5f, tree_pearent);
				set_tree(Vector3(126, 13, 199), Vector3(1, 1.2f, 1), Vector3(0), 2.5f, 0.5f, tree_pearent);
				set_tree(Vector3(126, 13, 186), Vector3(1, 1.2f, 1), Vector3(0), 2.5f, 0.5f, tree_pearent);
				set_tree(Vector3(134, 13, 186), Vector3(1, 1.2f, 1), Vector3(0), 2.5f, 0.5f, tree_pearent);

				set_tree(Vector3(90.0f, 6, 140), Vector3(1, 1.2f, 1), Vector3(0), 1.2f, 0.8f, tree_pearent);
				set_tree(Vector3(99.5f, 6, 140), Vector3(1, 1.2f, 1), Vector3(0), 1.2f, 0.8f, tree_pearent);
				set_tree(Vector3(108.5f, 6, 140), Vector3(1, 1.2f, 1), Vector3(0), 1.2f, 0.8f, tree_pearent);
				set_tree(Vector3(118.0f, 6, 140), Vector3(1, 1.2f, 1), Vector3(0), 1.2f, 0.8f, tree_pearent);

			}


		}

		// goal&respown_areas
		{
			Gameobject* goal_and_respown_areas = Gameobject_manager::create("goal&respown_areas");
			stage_parts.emplace_back(goal_and_respown_areas);

			set_respown_area(Vector3(24, 9, 57), Vector3(14, 5, 15), Vector3(0), 2, Vector3(24, 50, 57), goal_and_respown_areas);
			set_respown_area(Vector3(151, 18, 98), Vector3(28, 10, 20), Vector3(0), 3, Vector3(132, 50, 83), goal_and_respown_areas);
			set_respown_area(Vector3(210, 18, 43), Vector3(28, 10, 20), Vector3(0), 4, Vector3(189, 50, 49), goal_and_respown_areas);
			set_respown_area(Vector3(272, 18, 98), Vector3(28, 10, 33), Vector3(0), 5, Vector3(260, 50, 14), goal_and_respown_areas);

			set_goal_area(Vector3(370, 13, 43), Vector3(14, 2, 14), Vector3(0), Stage_types::stage_2, goal_and_respown_areas);
		}
#endif

		//set_sphere_rope(Vector3(64, 60, 65), 1, Vector3(0, 0, 0), 10, 5);

		//Gameobject_manager::find("Player_manager",Scenelist::scene_player)->findComponent<Player_manager>()->tutrial_move_and_camera();

		//summon_block_01();
	}

	void Stage_01::summon_block_01() {

		//int phyramid_size = 4;
		//Vector3 base_size = Vector3(1, 1, 1);
		//Vector3 base_pos = Vector3(33, 30, 12);

		//Gameobject* tutrial_block = Gameobject_manager::create("tutrial_block");
		//stage_parts.emplace_back(tutrial_block);

		//for (int y = 0; y < phyramid_size; ++y) {
		//	for (int x = 0; x < phyramid_size - y; ++x) {
		//		for (int z = 0; z < phyramid_size - y; ++z) {

		//			auto coll = set_box(base_pos + Vector3(
		//				(base_size.x * 2 + 0.2f) * x - ((base_size.x * 2 + 0.2f) * (phyramid_size - y) * 0.5f),
		//				(base_size.y * 2 + 1.f) * y + base_size.y,
		//				(base_size.z * 2 + 0.2f) * z - ((base_size.z * 2 + 0.2f) * (phyramid_size - y) * 0.5f)

		//			), base_size, Vector3(0, 0, 0), moveable_green, tutrial_block, false);
		//			coll->physics_data.inertial_mass = 5;
		//		}
		//	}
		//}


	}

	// 毎フレーム呼ばれる更新処理
	void Stage_01::update() {

	}

	void Stage_01::Update_hierarchy() {

	}

	void Stage_01::stage_destroy() {
		for (auto& object : stage_parts) {
			Gameobject_manager::deleteGameobject(object, true);
		}
		stage_parts.clear();
	}

}
