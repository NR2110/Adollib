

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
		player_respown_pos = Vector3(-2.7f, 80, -5.f);
		respown_num = -1;
		//player_respown_pos = Vector3(-2.7f, 10, -5);
		//player_respown_pos = Vector3(90, 16.0f, 66.0f);
		y_respown_pos = 30; // respownÇµÇΩÇ∆Ç´ÇÃYÇÃç¿ïW

		//set_box(Vector3(0, -60, -98), Vector3(30, 30, 60), Vector3(0), Vector3(188, 214, 54) / 255.0f, true);

		//set_desk(Vector3(0, 0, 0), Vector3(30, 15, 25), Vector3(0, 0, 0), 0);

#if _DEBUG
		//set_player_statue(Vector3(104, 17.6f, 132), 100, Vector3(0));
		set_pillar(Vector3(16, 4, 23), Vector3(1), Vector3(0));
#else

		// first_zone
		{
			Gameobject* first_zone = Gameobject_manager::create("first_zone");
			stage_parts.emplace_back(first_zone);
			set_box(Vector3(25, -2, 10), Vector3(65, 2, 30), Vector3(0), Vector3(188, 214, 54) / 255.0f * 0.8f * 0.9f, first_zone);
			// äKíi
			{
				Gameobject* stairs_pearent = Gameobject_manager::create("stairs_pearent");
				first_zone->add_child(stairs_pearent);
				set_box(Vector3(-15, 2, 23), Vector3(15, 2, 15), Vector3(0, 0, 0), base_color, stairs_pearent);
				set_box(Vector3(28, 2, 29.5f), Vector3(28, 2, 8.5f), Vector3(0, 0, 0), base_color, stairs_pearent);
				set_box(Vector3(28, 1, 24), Vector3(28, 2, 8), Vector3(0, 0, 0), base_color, stairs_pearent);
				set_box(Vector3(28, 0, 20), Vector3(28, 2, 8), Vector3(0, 0, 0), base_color, stairs_pearent);
				set_box(Vector3(28, -1, 16), Vector3(28, 2, 8), Vector3(0, 0, 0), base_color, stairs_pearent);
			}

			// ï«
			{
				Gameobject* wallAnddoor_pearent = Gameobject_manager::create("wallAnddoor_pearent");
				first_zone->add_child(wallAnddoor_pearent);

				set_box(Vector3(-9, 10, 39), Vector3(31, 10, 1), Vector3(0), base_color, wallAnddoor_pearent);
				set_box(Vector3(60, 10, 39), Vector3(30, 10, 1), Vector3(0), base_color, wallAnddoor_pearent);
				set_box(Vector3(26, 16, 39), Vector3(4, 4, 1), Vector3(0), base_color, wallAnddoor_pearent);
				set_box(Vector3(26, 3, 39), Vector3(4, 1, 1), Vector3(0), base_color, wallAnddoor_pearent);

				set_box(Vector3(18, 12, 37), Vector3(2, 8, 1), Vector3(0), base_color, wallAnddoor_pearent);
				set_box(Vector3(34, 12, 37), Vector3(2, 8, 1), Vector3(0), base_color, wallAnddoor_pearent);

				set_door(Vector3(24, 8, 39.25f), Vector3(2, 4, 0.5f), Vector3(0), true, wallAnddoor_pearent);
				set_door(Vector3(28, 8, 39.25f), Vector3(2, 4, 0.5f), Vector3(0), false, wallAnddoor_pearent);
			}


			// íå
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

			// çÚ
			{
				Gameobject* fences_pearent = Gameobject_manager::create("fences");
				first_zone->add_child(fences_pearent);
				set_fence(Vector3(-2, 4, 16.5f), Vector3(1), Vector3(0, 90, 0), fences_pearent);
				set_fence(Vector3(-21.5f, 4, 10), Vector3(1), Vector3(0, 0, 0), fences_pearent);
				set_fence(Vector3(-8.50f, 4, 10), Vector3(1), Vector3(0, 0, 0), fences_pearent);
				set_fence(Vector3(-28, 4, 28.5f), Vector3(1), Vector3(0, 90, 0), fences_pearent);
				set_fence(Vector3(-28, 4, 15.5f), Vector3(1), Vector3(0, 90, 0), fences_pearent);
			}

			//ñÿ
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
		}

		// second_zone
		{
			Gameobject* second_zone = Gameobject_manager::create("second_zone");
			stage_parts.emplace_back(second_zone);
			// è∞
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

			// ï«
			{
				Gameobject* wall_pearent = Gameobject_manager::create("wall_pearent");
				second_zone->add_child(wall_pearent);
				set_box(Vector3(83, 20.0f, 84.0f), Vector3(1, 8.0f, 16), Vector3(0), base_color, wall_pearent);
				set_box(Vector3(83, 20.0f, 50.0f), Vector3(1, 8.0f, 10), Vector3(0), base_color, wall_pearent);
				set_box(Vector3(83, 24.0f, 64.0f), Vector3(1, 4.0f, 4), Vector3(0), base_color, wall_pearent);

				set_gimmickdoor(Vector3(83, 16.0f, 62.0f), Vector3(83, 16.0f, 58.5f),
					Vector3(0), Vector3(0),
					2, 0,
					Vector3(0.5f, 4.0f, 2),
					Stage_parts::Stageparts_tags::Flag_0,
					base_color * 0.8f,
					wall_pearent
				);

				set_gimmickdoor(Vector3(83, 16.0f, 66.0f), Vector3(83, 16.0f, 69.5f),
					Vector3(0), Vector3(0),
					2, 0,
					Vector3(0.5f, 4.0f, 2),
					Stage_parts::Stageparts_tags::Flag_0,
					base_color * 0.8f,
					wall_pearent
				);


				set_buttan(Vector3(82.80, 16, 54), Vector3(2, 0.9f, 2), Vector3(0, 0, -90), Stage_parts::Stageparts_tags::Flag_0, wall_pearent, true);
			}
		}

		// third_zone
		{
			Gameobject* third_zone = Gameobject_manager::create("third_zone");
			stage_parts.emplace_back(third_zone);

			// äKíi
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

			// è∞
			{
				Gameobject* floor_pearent = Gameobject_manager::create("floor_pearent");
				third_zone->add_child(floor_pearent);
				set_box(Vector3(134, 0, 13), Vector3(28, 4, 11), Vector3(0), base_color, floor_pearent);
				set_box(Vector3(128, 2, 32), Vector3(22, 6, 8), Vector3(0), grass_color, floor_pearent);
				set_box(Vector3(128, 2, 49), Vector3(22, 6, 9), Vector3(0), base_color, floor_pearent);
				set_box(Vector3(116, 2, 78), Vector3(34, 6, 20), Vector3(0), grass_color, floor_pearent);
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

			// íå Ç∆è„ÇÃñ_
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

			// ñÿ
			{
				Gameobject* tree_pearent = Gameobject_manager::create("tree_pearent");
				third_zone->add_child(tree_pearent);

				set_tree(Vector3(92, 8, 90), Vector3(1, 1.2f, 1), Vector3(0, 02, 0), 2.5f, 0.8f, tree_pearent);
				set_tree(Vector3(157, 4, 8), Vector3(1, 1.2f, 1), Vector3(0, 76, 0), 1.2f, 0.8f, tree_pearent);
				set_tree(Vector3(148, 4, 8), Vector3(1, 1.2f, 1), Vector3(0, 12, 0), 1.2f, 0.8f, tree_pearent);
			}

			// ï«
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

			// ã¥
			{
				Gameobject* bridge_pearent = Gameobject_manager::create("bridge_pearent");
				fourth_zone->add_child(bridge_pearent);

				set_box(Vector3(144, 10.81f, 101.89f), Vector3(5, 1, 2), Vector3(5.5f, 0, 0), base_color, bridge_pearent);
				set_box(Vector3(144, 10.23f, 105.74f), Vector3(5, 1, 2), Vector3(11.5f, 0, 0), base_color, bridge_pearent);
				set_box(Vector3(144, 9.19f, 109.44f), Vector3(5, 1, 2), Vector3(20.5f, 0, 0), base_color, bridge_pearent);
				set_box(Vector3(144, 7.58f, 112.89f), Vector3(5, 1, 2), Vector3(30, 0, 0), base_color, bridge_pearent);
				set_box(Vector3(144, 5.71f, 115.63f), Vector3(5, 1, 1.5f), Vector3(40, 0, 0), base_color, bridge_pearent);

			}

			// è∞
			{
				Gameobject* floor_pearent = Gameobject_manager::create("floor_pearent");
				fourth_zone->add_child(floor_pearent);

				set_box(Vector3(110, 5, 119), Vector3(40, 1, 7), Vector3(0), roof_color, floor_pearent);
				set_box(Vector3(104, 5, 136), Vector3(20, 1, 10), Vector3(0), wood_color, floor_pearent);

				//è„
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

			// ëú
			{
				Gameobject* statue_pearent = Gameobject_manager::create("statue_pearent");
				fourth_zone->add_child(statue_pearent);

				set_box(Vector3(104, 8.5f, 132), Vector3(4, 0.5f, 3), Vector3(0), base_color, statue_pearent);
				set_box(Vector3(104, 7, 132), Vector3(5, 1, 5), Vector3(0), base_color, statue_pearent);

				auto go = set_player_statue(Vector3(104, 17.6f, 132), 2, Vector3(0));
				statue_pearent->add_child(go);
			}

			// äKíi
			{
				Gameobject* stair_pearent = Gameobject_manager::create("stair_pearent");
				fourth_zone->add_child(stair_pearent);
				const Vector3 color = Vector3(255, 255, 255) / 255.0f;
				const Vector3 roof_color = Vector3(233, 225, 217) / 255.0f;
				const Vector3 stair_color = Vector3(215, 205, 197) / 255.0f;

				// äKíi
				set_box(Vector3(130, 5.5f, 128), Vector3(8, 1.5f, 2), Vector3(0), stair_color, stair_pearent);
				set_box(Vector3(130, 6.0f, 132), Vector3(8, 2.0f, 2), Vector3(0), stair_color, stair_pearent);
				set_box(Vector3(130, 6.5f, 136), Vector3(8, 2.5f, 2), Vector3(0), stair_color, stair_pearent);
				set_box(Vector3(130, 7.0f, 140), Vector3(8, 3.0f, 2), Vector3(0), stair_color, stair_pearent);
				set_box(Vector3(130, 7.5f, 144), Vector3(8, 3.5f, 2), Vector3(0), stair_color, stair_pearent);
				// äKíi
				set_box(Vector3(78, 5.5f, 128), Vector3(8, 1.5f, 2), Vector3(0), stair_color, stair_pearent);
				set_box(Vector3(78, 6.0f, 132), Vector3(8, 2.0f, 2), Vector3(0), stair_color, stair_pearent);
				set_box(Vector3(78, 6.5f, 136), Vector3(8, 2.5f, 2), Vector3(0), stair_color, stair_pearent);
				set_box(Vector3(78, 7.0f, 140), Vector3(8, 3.0f, 2), Vector3(0), stair_color, stair_pearent);
				set_box(Vector3(78, 7.5f, 144), Vector3(8, 3.5f, 2), Vector3(0), stair_color, stair_pearent);
			}

			// íå
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

			// ï«
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

			// çÚ
			{
				Gameobject* fence_pearent = Gameobject_manager::create("fence_pearent");
				fourth_zone->add_child(fence_pearent);

				set_fence(Vector3(95, 14, 148.05f), Vector3(1), Vector3(0, 0, 0), fence_pearent);
				set_fence(Vector3(104, 14, 148.05f), Vector3(1), Vector3(0, 0, 0), fence_pearent);
				set_fence(Vector3(113, 14, 148.05f), Vector3(1), Vector3(0, 0, 0), fence_pearent);
			}

			// ñÿ
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

			set_respown_area(Vector3(24, 9, 57),    Vector3(14, 5, 15),  Vector3(0), 2, Vector3(24,  80, 57), goal_and_respown_areas);
			set_respown_area(Vector3(100, 18, 61),  Vector3(16, 10, 19), Vector3(0), 3, Vector3(102, 80, 50), goal_and_respown_areas);
			set_respown_area(Vector3(126, 16, 116), Vector3(24, 10, 10), Vector3(0), 4, Vector3(104, 80,120), goal_and_respown_areas);

			set_goal_area(Vector3(104, 6, 224), Vector3(10, 2, 8), Vector3(0), Stage_types::demo, goal_and_respown_areas);
		}


#endif

		//set_sphere_rope(Vector3(64, 60, 65), 1, Vector3(0, 0, 0), 10, 5);
	}

	// ñàÉtÉåÅ[ÉÄåƒÇŒÇÍÇÈçXêVèàóù
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
