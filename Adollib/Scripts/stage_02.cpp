
#include "stage_02.h"

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
	void Stage_02::stage_awake()
	{
		player_respown_pos = Vector3(-2.7f, 50, -5.f);
		respown_num = -1;
		next_stage = Stage_types::none;
		y_respown_pos = 100; // respownしたときのYの座標
		y_player_respown_limit = -40; //playerのyがこれ以下だとrespownされる
		y_respown_limit = -150;  //blockのyがこれ以下だとrespownされる

		auto player_manager = Gameobject_manager::find("Player_manager", Scenelist::scene_player)->findComponent<Player_manager>();
		player_manager->set_is_shotable(false);

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

			{
				Gameobject* pearent = Gameobject_manager::create("stairs_pearent");
				first_zone->add_child(pearent);
				set_box(Vector3(0, -2, 13), Vector3(50, 2, 32.0f), Vector3(0, 0, 0), grass_color, pearent);
				set_box(Vector3(0, 15, 30), Vector3(50, 15, 15), Vector3(0, 0, 0), base_color, pearent);
				set_box(Vector3(50, 4, 0), Vector3(20, 4, 20), Vector3(0, 0, 0), base_color, pearent);
				set_box(Vector3(60, 2, 15), Vector3(20, 10, 25), Vector3(0, 0, 0), base_color, pearent);
				set_box(Vector3(80, 15, 18), Vector3(8, 15, 40), Vector3(0, 0, 0), base_color, pearent);
				set_box(Vector3(20, 1.5f, 0), Vector3(2, 1.5f, 12), Vector3(0, 0, 0), grass_color, pearent);
				set_box(Vector3(28, 3, 0), Vector3(2, 3, 15), Vector3(0, 0, 0), grass_color, pearent);
				set_box(Vector3(50.5f, 9, 1), Vector3(18, 1, 16), Vector3(0, 0, 0), grass_color, pearent);

				set_fence(Vector3(-4, 0, -15), Vector3(1), Vector3(0, 0, 0), pearent);
				set_fence(Vector3(-31, 0, -15), Vector3(1), Vector3(0, 180, 0), pearent);
				set_fence(Vector3(-45, 0, -6), Vector3(1), Vector3(0, +90, 0), pearent);
				set_fence(Vector3(-45, 0, 16), Vector3(1), Vector3(0, -90, 0), pearent);


				set_box(Vector3(7, 32, 30), Vector3(23, 2, 13), Vector3(0, 0, 0), base_color, pearent);
				set_box(Vector3(12, 34, 33), Vector3(10, 2, 8), Vector3(0, 0, 0), base_color, pearent);


			}

			// 壁
			{
				Gameobject* wallAnddoor_pearent = Gameobject_manager::create("wallAnddoor_pearent");
				first_zone->add_child(wallAnddoor_pearent);
				wallAnddoor_pearent->transform->local_pos = Vector3(35, 8, 0);

				set_box(Vector3(7, 10, 39), Vector3(15, 10, 1), Vector3(0), base_color, wallAnddoor_pearent);
				set_box(Vector3(36, 10, 39), Vector3(6, 10, 1), Vector3(0), base_color, wallAnddoor_pearent);
				set_box(Vector3(26, 16, 39), Vector3(4, 4, 1), Vector3(0), base_color, wallAnddoor_pearent);

				set_box(Vector3(18, 12, 37), Vector3(2, 8, 1), Vector3(0), base_color * 0.9f, wallAnddoor_pearent);
				set_box(Vector3(34, 12, 37), Vector3(2, 8, 1), Vector3(0), base_color * 0.9f, wallAnddoor_pearent);

				set_door(Vector3(24, 8, 39.25f), Vector3(2, 4, 0.5f), Vector3(0), true, wallAnddoor_pearent);
				set_door(Vector3(28, 8, 39.25f), Vector3(2, 4, 0.5f), Vector3(0), false, wallAnddoor_pearent);
			}

			{
				Gameobject* tree = Gameobject_manager::create("tree");
				first_zone->add_child(tree);

				set_tree(Vector3(-12, 0, 10), Vector3(1, 1.2f, 1), Vector3(0, 76, 0), 1.2f, 0.8f, tree);
				set_tree(Vector3(-23, 0, 10), Vector3(1, 1.2f, 1), Vector3(0, 14, 0), 1.2f, 0.8f, tree);
				set_tree(Vector3(-34, 0, 10), Vector3(1, 1.2f, 1), Vector3(0, 95, 0), 1.2f, 0.8f, tree);
			}

		}

		// second_zone Tag 0,1,2,3使用
		{
			Gameobject* second_zone = Gameobject_manager::create("second_zone");
			stage_parts.emplace_back(second_zone);

			// floor
			{
				Gameobject* first_floor = Gameobject_manager::create("first_floor");
				second_zone->add_child(first_floor);

				// 最初の床
				set_box(Vector3(52, 2, 50), Vector3(20, 10, 10), Vector3(0, 0, 0), base_color, first_floor);
			}
			// stairs
			{
				Gameobject* staire_pearent = Gameobject_manager::create("staire_pearent");
				second_zone->add_child(staire_pearent);

				// 階段
				set_box(Vector3(38, 5.5f, 50), Vector3(2, 7.5f, 5), Vector3(0, 0, 0), base_color, staire_pearent);
				set_box(Vector3(34, 6.0f, 50), Vector3(2, 8.0f, 5), Vector3(0, 0, 0), base_color, staire_pearent);
				set_box(Vector3(30, 6.5f, 50), Vector3(2, 8.5f, 5), Vector3(0, 0, 0), base_color, staire_pearent);
				set_box(Vector3(26, 7.0f, 50), Vector3(2, 9.0f, 5), Vector3(0, 0, 0), base_color, staire_pearent);
				set_box(Vector3(22, 7.5f, 50), Vector3(2, 9.5f, 5), Vector3(0, 0, 0), base_color, staire_pearent);
				set_box(Vector3(12, 8, 50), Vector3(8, 10, 5), Vector3(0, 0, 0), base_color, staire_pearent);

				set_box(Vector3(26, 8, 65), Vector3(22, 10, 10), Vector3(0, 0, 0), base_color, staire_pearent); //上
				set_box(Vector3(60, 8, 60), Vector3(12, 10, 5), Vector3(0, 0, 0), base_color, staire_pearent); //上
				set_box(Vector3(60, 8.5f, 70), Vector3(12, 10, 5), Vector3(0, 0, 0), grass_color, staire_pearent); //上

				set_box(Vector3(14, 8, 109), Vector3(10, 10, 34), Vector3(0, 0, 0), base_color, staire_pearent); //上
			}
			// gimmick
			{
				Gameobject* gimmick = Gameobject_manager::create("gimmick");
				gimmick->transform->local_pos = Vector3(80, 0, 65);
				gimmick->transform->local_orient = quaternion_from_euler(0, -90, 0);

				second_zone->add_child(gimmick);

				set_box(Vector3(60, 8, 125), Vector3(20, 10, 25), Vector3(0, 0, 0), base_color, gimmick);
				set_box(Vector3(10, 8, 115), Vector3(30, 10, 15), Vector3(0, 0, 0), base_color, gimmick);
				set_box(Vector3(41, 24, 125), Vector3(1, 6, 25), Vector3(0, 0, 0), base_color, gimmick);
				set_box(Vector3(4, 28, 122), Vector3(1, 2, 5), Vector3(0, 0, 0), base_color, gimmick); //上
				set_box(Vector3(4, 23, 128), Vector3(1, 5, 2), Vector3(0, 0, 0), base_color, gimmick); //右
				set_box(Vector3(4, 23, 109), Vector3(1, 5, 9), Vector3(0, 0, 0), base_color, gimmick); //左

				set_box(Vector3(10.5f, 23, 129), Vector3(5.5f, 5, 1), Vector3(0, 0, 0), stair_color, gimmick);
				set_box(Vector3(-7.5f, 20, 99), Vector3(12.5f, 8, 1), Vector3(0, 0, 0), base_color, gimmick);

				std::shared_ptr<Material> yajisushi_mat = Material_manager::create_material("yajisushi_material");
				yajisushi_mat->get_texture()->Load(L"./DefaultTexture/stage/yajiushi.png");
				yajisushi_mat->BS_state = State_manager::BStypes::BS_ALPHA;
				yajisushi_mat->Load_VS("./DefaultShader/sprite_vs.cso");
				yajisushi_mat->Load_PS("./DefaultShader/sprite_ps.cso");
				yajisushi_mat->is_render_shadow = false;
				yajisushi_mat->color = Vector4(1, 1, 1, 1);
				auto plane = set_plane(Vector3(11, 23, 127), Vector3(5.5f, 5, 0), Vector3(0, 180, 0), Vector3(base_color), gimmick);
				plane->renderer->set_material(yajisushi_mat);

				set_gimmickdoor(Vector3(4, 22, 124), Vector3(4, 22, 127), Vector3(0), Vector3(0),
					2, 0, Vector3(0.5f, 4, 2), Stage_parts::Stageparts_tags::Flag_2, stair_color, gimmick);
				set_gimmickdoor(Vector3(4, 22, 120), Vector3(4, 22, 117), Vector3(0), Vector3(0),
					2, 0, Vector3(0.5f, 4, 2), Stage_parts::Stageparts_tags::Flag_2, stair_color, gimmick);

				set_buttan(Vector3(20, 17.2f, 110), Vector3(4, 1, 4), Vector3(0), Stage_parts::Stageparts_tags::Flag_2, gimmick);

				// ピラミッド
				{
					Gameobject* tutrial_block = Gameobject_manager::create("pyramid");
					gimmick->add_child(tutrial_block);
					tutrial_block->transform->local_pos = Vector3(63, 18, 130);

					int phyramid_size = 3;
					Vector3 base_size = Vector3(4, 2, 4);
					Vector3 base_pos = Vector3(0);

					for (int y = 0; y < phyramid_size; ++y) {
						for (int x = 0; x < phyramid_size - y; ++x) {
							for (int z = 0; z < phyramid_size - y; ++z) {

								auto coll = set_box(base_pos + Vector3(
									(base_size.x * 2 + 0.2f) * x - ((base_size.x * 2 + 0.2f) * (phyramid_size - y) * 0.5f),
									(base_size.y * 2 + 0) * y + base_size.y,
									(base_size.z * 2 + 0.2f) * z - ((base_size.z * 2 + 0.2f) * (phyramid_size - y) * 0.5f)

								), base_size, Vector3(0, 0, 0), moveable_red, tutrial_block, false);
								coll->physics_data.inertial_mass = 20;
								coll->physics_data.angula_sleep_threrhold = 0;
							}
						}
					}
				}

				// エレベーター
				{
					auto doorgo = set_gimmickdoor(Vector3(-15, 9, 115), Vector3(-15, 20, 115), Vector3(0), Vector3(0),
						3, 0, Vector3(5, 10, 5), Stage_parts::Stageparts_tags::Flag_3, stair_color, gimmick);

					auto switchgo = set_buttan(Vector3(0, 1.01f, 0), Vector3(0.8f, 0.08f, 0.8f), Vector3(0), Stage_parts::Stageparts_tags::Flag_3, gimmick);

					doorgo->add_child(switchgo);
					//set_box(Vector3(-15, 9, 115), Vector3(5, 10, 5), Vector3(0, 0, 0), base_color, gimmick);
					//set_box(Vector3(-15, 19, 115), Vector3(4, 1, 4), Vector3(0, 0, 0), base_color, gimmick);
				}

			}
			// lever&move
			{
				Gameobject* lever_and_move = Gameobject_manager::create("lever_and_move");
				second_zone->add_child(lever_and_move);
				set_lever(Vector3(8, 19, 104), Vector3(1), Vector3(0, 90, 0), Stage_parts::Stageparts_tags::Flag_0, Stage_parts::Stageparts_tags::Flag_1, lever_and_move);
				set_box(Vector3(8, 17.5, 104), Vector3(4, 1, 2), Vector3(0, 90, 0), stair_color, lever_and_move);
				set_move_block_2flags(Vector3(-10, 17, 95), Vector3(-10, 17, 117),
					Vector3(10, 1, 5), Vector3(0),
					0.1f, 0.1f,
					Stage_parts::Stageparts_tags::Flag_0, Stage_parts::Stageparts_tags::Flag_1,
					grass_color, lever_and_move);
			}
			// trees
			{
				Gameobject* tree = Gameobject_manager::create("tree");
				second_zone->add_child(tree);

				set_tree(Vector3(66, 18.5f, 70), Vector3(1, 1.2f, 1), Vector3(0, 02, 0), 2.5f, 0.8f, tree);
				set_tree(Vector3(55, 18.5f, 70), Vector3(1, 1.2f, 1), Vector3(0, 02, 0), 2.5f, 0.8f, tree);
			}
			// 装飾
			{
				Gameobject* sousyoku = Gameobject_manager::create("sousyoku");
				second_zone->add_child(sousyoku);

				set_box(Vector3(23, 32, 93), Vector3(1, 2, 50), Vector3(0, 0, 0), stair_color, sousyoku);
				set_box(Vector3(23, 24, 46), Vector3(1, 7, 1), Vector3(0, 0, 0), stair_color, sousyoku);
				set_box(Vector3(23, 24, 74), Vector3(1, 6, 1), Vector3(0, 0, 0), stair_color, sousyoku);
				set_box(Vector3(23, 24, 96), Vector3(1, 6, 1), Vector3(0, 0, 0), stair_color, sousyoku);
				set_box(Vector3(23, 24, 120), Vector3(1, 6, 1), Vector3(0, 0, 0), stair_color, sousyoku);
				set_box(Vector3(23, 24, 142), Vector3(1, 6, 1), Vector3(0, 0, 0), stair_color, sousyoku);

				set_box(Vector3(13, 32, 142), Vector3(9, 2, 1), Vector3(0, 0, 0), stair_color, sousyoku);
				set_box(Vector3(-31, 32, 142), Vector3(36, 2, 1), Vector3(0, 0, 0), stair_color, sousyoku);
				set_box(Vector3(-21, 24, 142), Vector3(1, 6, 1), Vector3(0, 0, 0), stair_color, sousyoku);
				set_box(Vector3(-43, 24, 142), Vector3(1, 6, 1), Vector3(0, 0, 0), stair_color, sousyoku);
				set_box(Vector3(-66, 24, 142), Vector3(1, 6, 1), Vector3(0, 0, 0), stair_color, sousyoku);
				set_box(Vector3(-66, 24, 108), Vector3(1, 6, 1), Vector3(0, 0, 0), stair_color, sousyoku);

				set_box(Vector3(-66, 32, 123), Vector3(1, 2, 18), Vector3(0, 0, 0), stair_color, sousyoku);

				set_box(Vector3(-49, 24, 104), Vector3(1, 6, 1), Vector3(0, 0, 0), stair_color, sousyoku);
				set_box(Vector3(-49, 24, 82), Vector3(1, 6, 1), Vector3(0, 0, 0), stair_color, sousyoku);
				set_box(Vector3(-49, 19, 92), Vector3(0.5f, 1, 11), Vector3(0, 0, 0), stair_color, sousyoku);

			}

		}

		{}

		// third_zone Tag 4,5使用
		{
			Gameobject* third_zone = Gameobject_manager::create("third_zone");
			stage_parts.emplace_back(third_zone);

			// bridge
			{
				Gameobject* bridge = Gameobject_manager::create("bridge");
				third_zone->add_child(bridge);

				// 橋
				//set_box(Vector3(-9, 29, 74), Vector3(5, 1, 29), Vector3(0, 0, 0), base_color, bridge);
				set_box(Vector3(-40, 29, -17), Vector3(5, 1, 32), Vector3(0, 0, 0), wood_color, bridge);

				set_box(Vector3(-46, 33, 16), Vector3(0.5f, 3, 0.5f), Vector3(0, 0, 0), wood_color, bridge);
				set_box(Vector3(-45.69f, 31.48f, 1), Vector3(0.3f, 0.3f, 15), Vector3(-10, -1.4f, 0), fence_color, bridge);
				set_box(Vector3(-34, 33, 16), Vector3(0.5f, 3, 0.5f), Vector3(0, 0, 0), wood_color, bridge);
				set_box(Vector3(-34.30f, 31.48f, 1), Vector3(0.3f, 0.3f, 15), Vector3(-10, +1.4f, 0), fence_color, bridge);

				set_box(Vector3(-46, 33, -50), Vector3(0.5f, 3, 0.5f), Vector3(0, 0, 0), wood_color, bridge);
				set_box(Vector3(-45.69f, 31.48f, -35.0f), Vector3(0.3f, 0.3f, 15), Vector3(+10, +1.4f, 0), fence_color, bridge);
				set_box(Vector3(-34, 33, -50), Vector3(0.5f, 3, 0.5f), Vector3(0, 0, 0), wood_color, bridge);
				set_box(Vector3(-34.30f, 31.48f, -35.0f), Vector3(0.3f, 0.3f, 15), Vector3(+10, -1.4f, 0), fence_color, bridge);

			}
			// stage
			{
				Gameobject* stage = Gameobject_manager::create("stage");
				third_zone->add_child(stage);

				// 入口
				set_box(Vector3(-27, 0, -75), Vector3(26, 30, 26), Vector3(0, 0, 0), base_color, stage);

				// 床
				set_box(Vector3(33, 1, -131), Vector3(43, 10, 17), Vector3(0, 0, 0), grass_color, stage);
				set_box(Vector3(-34, 1, -131), Vector3(16, 10, 17), Vector3(0, 0, 0), grass_color, stage);
				set_box(Vector3(-39, 1, -108.5f), Vector3(11, 10, 5.5f), Vector3(0, 0, 0), grass_color, stage);
				set_box(Vector3(13, 1, -90), Vector3(63, 10, 13), Vector3(0, 0, 0), grass_color, stage);
				set_box(Vector3(24, 1, -108.5f), Vector3(52, 10, 5.5f), Vector3(0, 0, 0), base_color, stage);
				set_box(Vector3(-14, 1, -131), Vector3(4, 10, 17), Vector3(0, 0, 0), base_color, stage);

			}
			// ピラミッド
			{
				Gameobject* tutrial_block = Gameobject_manager::create("pyramid");
				third_zone->add_child(tutrial_block);
				tutrial_block->transform->local_pos = Vector3(48, 11, -129);

				int phyramid_size = 4;
				Vector3 base_size = Vector3(2.5f, 1.5f, 2);
				Vector3 base_pos = Vector3(0);

				for (int y = 0; y < phyramid_size; ++y) {
					for (int x = 0; x < phyramid_size - y; ++x) {
						for (int z = 0; z < phyramid_size - y; ++z) {

							auto coll = set_box(base_pos + Vector3(
								(base_size.x * 2 + 0.2f) * x - ((base_size.x * 2 + 0.2f) * (phyramid_size - y) * 0.5f),
								(base_size.y * 2 + 0) * y + base_size.y,
								(base_size.z * 2 + 0.2f) * z - ((base_size.z * 2 + 0.2f) * (phyramid_size - y) * 0.5f)

							), base_size, Vector3(0, 0, 0), moveable_red, tutrial_block, false);
							coll->physics_data.inertial_mass = 10;
							coll->physics_data.angula_sleep_threrhold = 0;
							coll->physics_data.is_moveable = false;
						}
					}
				}
			}
			// stairss
			{
				Gameobject* stairs = Gameobject_manager::create("stairs");
				third_zone->add_child(stairs);

				// 階段
				set_box(Vector3(-35, 20.5f, -104), Vector3(5, 9.5f, 3), Vector3(0, 0, 0), base_color, stairs);
				set_box(Vector3(-35, 20.0f, -109), Vector3(5, 9.0f, 2), Vector3(0, 0, 0), base_color, stairs);
				set_box(Vector3(-35, 23.0f, -113), Vector3(5, 5.0f, 2), Vector3(0, 0, 0), base_color, stairs); //アーチ
				set_box(Vector3(-35, 23.5f, -117), Vector3(5, 3.5f, 2), Vector3(0, 0, 0), base_color, stairs); //アーチ
				set_box(Vector3(-35, 23.5f, -121), Vector3(5, 2.5f, 2), Vector3(0, 0, 0), base_color, stairs); //アーチ
				set_box(Vector3(-35, 22.5f, -125), Vector3(5, 2.5f, 2), Vector3(0, 0, 0), base_color, stairs); //アーチ
				set_box(Vector3(-35, 21.0f, -129), Vector3(5, 3.0f, 2), Vector3(0, 0, 0), base_color, stairs); //アーチ
				set_box(Vector3(-35, 17.0f, -133), Vector3(5, 6.0f, 2), Vector3(0, 0, 0), base_color, stairs); //アーチ
				set_box(Vector3(-35, 16.5f, -137), Vector3(5, 5.5f, 2), Vector3(0, 0, 0), base_color, stairs);
				set_box(Vector3(-35, 16.0f, -141), Vector3(5, 5.0f, 2), Vector3(0, 0, 0), base_color, stairs);
				set_box(Vector3(-35, 15.5f, -145), Vector3(5, 4.5f, 2), Vector3(0, 0, 0), base_color, stairs);
				set_box(Vector3(-35, 15.0f, -149), Vector3(5, 4.0f, 2), Vector3(0, 0, 0), base_color, stairs);
				set_box(Vector3(-35, 14.5f, -153), Vector3(5, 3.5f, 2), Vector3(0, 0, 0), base_color, stairs);

				// 遊び場
				set_box(Vector3(-33, -2.5f, -157), Vector3(15, 20, 10), Vector3(0, 0, 0), base_color, stairs);
				set_box(Vector3(-14, -2.5f, -161), Vector3(4, 20, 6), Vector3(0, 0, 0), base_color, stairs);
				set_box(Vector3(6, -2.5f, -157), Vector3(16, 20, 10), Vector3(0, 0, 0), base_color, stairs);
				// 遊び場奥
				set_box(Vector3(31, -2.0f, -155.50f), Vector3(9, 20, 9), Vector3(0, 0, 0), grass_color, stairs);

				// 階段
				set_box(Vector3(-14, 14.0f, -153), Vector3(4, 3.0f, 2), Vector3(0, 0, 0), base_color, stairs);
				set_box(Vector3(-14, 13.5f, -149), Vector3(4, 2.5f, 2), Vector3(0, 0, 0), base_color, stairs);
				set_box(Vector3(-14, 13.0f, -145), Vector3(4, 2.0f, 2), Vector3(0, 0, 0), base_color, stairs);
				set_box(Vector3(-14, 12.5f, -141), Vector3(4, 1.5f, 2), Vector3(0, 0, 0), base_color, stairs);
				set_box(Vector3(-14, 12.0f, -137), Vector3(4, 1.0f, 2), Vector3(0, 0, 0), base_color, stairs);
				set_box(Vector3(-14, 11.5f, -133), Vector3(4, 0.5f, 2), Vector3(0, 0, 0), base_color, stairs);
			}
			// switch_base
			{
				Gameobject* switch_base = Gameobject_manager::create("switch_base");
				third_zone->add_child(switch_base);

				// switch base
				set_box(Vector3(33, 9, -82), Vector3(10, 10, 10), Vector3(0, 0, 0), base_color, switch_base);

				set_box(Vector3(21, 15.0f, -76), Vector3(2, 4.0f, 3), Vector3(0, 0, 0), base_color, switch_base);
				set_box(Vector3(21, 14.5f, -80), Vector3(2, 3.5f, 1), Vector3(0, 0, 0), base_color, switch_base);
				set_box(Vector3(21, 14.0f, -82), Vector3(2, 3.0f, 1), Vector3(0, 0, 0), base_color, switch_base);
				set_box(Vector3(21, 13.5f, -84), Vector3(2, 2.5f, 1), Vector3(0, 0, 0), base_color, switch_base);
				set_box(Vector3(21, 13.0f, -86), Vector3(2, 2.0f, 1), Vector3(0, 0, 0), base_color, switch_base);
				set_box(Vector3(21, 12.5f, -88), Vector3(2, 1.5f, 1), Vector3(0, 0, 0), base_color, switch_base);
				set_box(Vector3(21, 12.0f, -90), Vector3(2, 1.5f, 1), Vector3(0, 0, 0), base_color, switch_base);
				set_box(Vector3(21, 11.5f, -92), Vector3(2, 1.0f, 1), Vector3(0, 0, 0), base_color, switch_base);
				set_box(Vector3(21, 11.0f, -94), Vector3(2, 0.5f, 1), Vector3(0, 0, 0), base_color, switch_base);

				set_box(Vector3(45, 15.0f, -76), Vector3(2, 4.0f, 3), Vector3(0, 0, 0), base_color, switch_base);
				set_box(Vector3(45, 14.5f, -80), Vector3(2, 3.5f, 1), Vector3(0, 0, 0), base_color, switch_base);
				set_box(Vector3(45, 14.0f, -82), Vector3(2, 3.0f, 1), Vector3(0, 0, 0), base_color, switch_base);
				set_box(Vector3(45, 13.5f, -84), Vector3(2, 2.5f, 1), Vector3(0, 0, 0), base_color, switch_base);
				set_box(Vector3(45, 13.0f, -86), Vector3(2, 2.0f, 1), Vector3(0, 0, 0), base_color, switch_base);
				set_box(Vector3(45, 12.5f, -88), Vector3(2, 1.5f, 1), Vector3(0, 0, 0), base_color, switch_base);
				set_box(Vector3(45, 12.0f, -90), Vector3(2, 1.5f, 1), Vector3(0, 0, 0), base_color, switch_base);
				set_box(Vector3(45, 11.5f, -92), Vector3(2, 1.0f, 1), Vector3(0, 0, 0), base_color, switch_base);
				set_box(Vector3(45, 11.0f, -94), Vector3(2, 0.5f, 1), Vector3(0, 0, 0), base_color, switch_base);

			}
			// wall
			{
				Gameobject* wall = Gameobject_manager::create("wall");
				third_zone->add_child(wall);

				// 壁
				set_box(Vector3(55, 25, -163), Vector3(15, 30, 17), Vector3(0, 0, 0), base_color, wall);

				set_box(Vector3(75, 21, -83.65f), Vector3(1, 10, 6.65f), Vector3(0, 0, 0), base_color, wall); //左
				set_box(Vector3(75, 21, -133.35f), Vector3(1, 10, 6.65f), Vector3(0, 0, 0), base_color, wall); //右

				set_box(Vector3(75, 14, -91.3f), Vector3(1, 1, 1), Vector3(0, 0, 0), base_color, wall); //左 すきま
				set_box(Vector3(75, 18, -91.3f), Vector3(1, 1, 1), Vector3(0, 0, 0), base_color, wall); //左 すきま
				set_box(Vector3(75, 22, -91.3f), Vector3(1, 1, 1), Vector3(0, 0, 0), base_color, wall); //左 すきま
				set_box(Vector3(75, 26, -91.3f), Vector3(1, 1, 1), Vector3(0, 0, 0), base_color, wall); //左 すきま
				set_box(Vector3(75, 30, -91.3f), Vector3(1, 1, 1), Vector3(0, 0, 0), base_color, wall); //左 すきま

				set_box(Vector3(75, 14, -125.7f), Vector3(1, 1, 1), Vector3(0, 0, 0), base_color, wall); //右 すきま
				set_box(Vector3(75, 18, -125.7f), Vector3(1, 1, 1), Vector3(0, 0, 0), base_color, wall); //右 すきま
				set_box(Vector3(75, 22, -125.7f), Vector3(1, 1, 1), Vector3(0, 0, 0), base_color, wall); //右 すきま
				set_box(Vector3(75, 26, -125.7f), Vector3(1, 1, 1), Vector3(0, 0, 0), base_color, wall); //右 すきま
				set_box(Vector3(75, 30, -125.7f), Vector3(1, 1, 1), Vector3(0, 0, 0), base_color, wall); //右 すきま

			}
			// crane
			{
				Gameobject* crane = Gameobject_manager::create("crane");
				third_zone->add_child(crane);

				// クレーン
				set_box(Vector3(+30, 46, -108.5f), Vector3(60.f, 1, 1.0f), Vector3(0, 0, 0), wood_color, crane); //上
				set_box(Vector3(-24, 45, -108.5f), Vector3(1, 1, 16), Vector3(0, 0, 0), stair_color, crane);       //手前 上
				set_box(Vector3(-24, 37, -94.5f), Vector3(0.5f, 7, 0.5f), Vector3(0, 0, 0), stair_color, crane);   //手前 柱
				set_box(Vector3(-24, 27, -122.5f), Vector3(0.5f, 17, 0.5f), Vector3(0, 0, 0), stair_color, crane); //手前 柱
				set_box(Vector3(+83, 45, -108.5f), Vector3(1, 1, 16), Vector3(0, 0, 0), stair_color, crane);       //奥 上
				set_box(Vector3(+83, 27, -94.5f), Vector3(0.5f, 17, 0.5f), Vector3(0, 0, 0), stair_color, crane);  //奥 柱
				set_box(Vector3(+83, 27, -122.5f), Vector3(0.5f, 17, 0.5f), Vector3(0, 0, 0), stair_color, crane); //奥 柱

				set_box(Vector3(-22.5f, 45.5f, -108.5f), Vector3(0.5f, 2, 1.5f), Vector3(0, 0, 0), grass_color, crane); //手前固定
				set_box(Vector3(-25.5f, 45.5f, -108.5f), Vector3(0.5f, 2, 1.5f), Vector3(0, 0, 0), grass_color, crane); //手前固定
				set_box(Vector3(+81.5f, 45.5f, -108.5f), Vector3(0.5f, 2, 1.5f), Vector3(0, 0, 0), grass_color, crane); //奥固定
				set_box(Vector3(+84.5f, 45.5f, -108.5f), Vector3(0.5f, 2, 1.5f), Vector3(0, 0, 0), grass_color, crane); //奥固定


				// 壊す壁
				set_box(Vector3(75, 11, -66.5f), Vector3(1, 20, 10.5f), Vector3(0, 0, 0), base_color, crane);
			}
			// brake_wall
			{
				Gameobject* brake_wall = Gameobject_manager::create("brake_wall");
				third_zone->add_child(brake_wall);
				brake_wall->transform->local_pos = Vector3(75, 11, -110.5f);
				brake_wall->transform->local_orient = quaternion_from_euler(Vector3(0, 90, 0));

				float mass = 10;
				Vector3 color = Vector3(187, 185, 181) / 255;
				Vector3 size = Vector3(2, 1, 1);
				Vector2 count = Vector2(8, 10);



				for (int i = 0; i < count.y; i++) {

					int x_num = count.x;
					if (i % 2 == 0) x_num += 1;

					for (int o = 0; o < x_num; o++) {
						auto coll = set_box(Vector3(
							size.x * 2 * o - size.x * x_num,
							size.y * 2 * i + size.y,
							0
						),
							size,
							Vector3(0, 0, 0),
							color + Vector3(rand() % 5 / 255.0f),
							nullptr,
							false
						);
						//coll->tag &= ~Collider_tags::Caera_not_sunk_Stage;
						coll->physics_data.inertial_mass = mass;
						coll->physics_data.linear_sleep_threrhold = 0.2f;
						coll->physics_data.angula_sleep_threrhold = 0.2f;
						//coll->physics_data.is_moveable = false;
						//coll->physics_data.dynamic_friction = 0.8f;
						brake_wall->add_child(coll->gameobject);
					}


				}

			}
			// lever&move
			{
				Gameobject* lever_and_move = Gameobject_manager::create("lever_and_move");
				third_zone->add_child(lever_and_move);
				set_lever(Vector3(32.5f, 20, -88), Vector3(1), Vector3(0), Stage_parts::Stageparts_tags::Flag_4, Stage_parts::Stageparts_tags::Flag_5, lever_and_move);
				set_box(Vector3(32.5f, 18.5f, -88), Vector3(4, 1, 2), Vector3(0, 0, 0), stair_color, lever_and_move);

				{
					float rope_sphere_r = 0.5f;
					float rope_sphree_offset_size = rope_sphere_r * 3;
					int rope_sphere_count = 16;
					float rope_mass = 5;
					Vector3 rope_joint_start_offset = Vector3(0, -0.8f, 0);

					// 動かすもの
					auto base_coll = set_move_block_2flags(Vector3(80.5f, 46, -108.5f), Vector3(-21.5f, 46, -108.5f),
						Vector3(0.5f, 1.2f, 1.2f), Vector3(0),
						0.4f, 0.8f,
						Stage_parts::Stageparts_tags::Flag_4, Stage_parts::Stageparts_tags::Flag_5,
						Yellow_color, lever_and_move);

					Vector3 rope_0_pos = base_coll->gameobject->transform->local_pos + rope_joint_start_offset;

					// 下の球
					float sphere_r = 5.2f;
					auto sphere_coll = set_sphere(
						rope_0_pos
						+ Vector3(0, -1, 0) * rope_sphree_offset_size * (rope_sphere_count - 1)
						+ Vector3(0, -1, 0) * (sphere_r - 0.5f)
						, sphere_r, Vector3(1, 0, 0), lever_and_move);
					sphere_coll->tag = Collider_tags::Sphere | Collider_tags::Stage | Collider_tags::Caera_not_sunk_Stage | Collider_tags::Kinematic_Stage;
					sphere_coll->physics_data.inertial_mass = 500;
					sphere_coll->physics_data.angula_sleep_threrhold = 0;
					sphere_coll->physics_data.is_moveable = true;
					sphere_coll->physics_data.is_static = false;


					Collider_Rope* rope_coll = nullptr;
					// ropeの作成
					{

						// colliderのアタッチ&初期設定
						auto rope_go = Gameobject_manager::create("rope_go");
						lever_and_move->add_child(rope_go);
						rope_go->transform->local_pos = rope_0_pos;

						rope_coll = rope_go->addComponent<Collider_Rope>();

						rope_coll->sphere_size_r = rope_sphere_r;
						rope_coll->sphree_offset_size = rope_sphree_offset_size;
						rope_coll->sphere_num_size = rope_sphere_count;
						rope_coll->start_rope_dir = Vector3(0, -1, 0);
						rope_coll->tag |= Collider_tags::Human_rope;
						rope_coll->ignore_tags |= Collider_tags::Human;
						rope_coll->default_physics_data.inertial_mass = rope_mass;
						rope_coll->create_rope();

						// 場所によって質量を調整する
						for (int i = 0; i < rope_coll->get_collider_size(); ++i) {
							auto physics_data = rope_coll->get_vertex_data(i);
							physics_data.inertial_mass = rope_mass * (1 + ((float)rope_coll->get_collider_size() - i) / rope_coll->get_collider_size());
							rope_coll->set_vertex_data(i, physics_data);
						}

						// blockにつなぐ0と最後のcolliderを衝突しないように(暴れやすいため)
						{
							auto vertex_data_0 = rope_coll->get_vertex_data(0);
							vertex_data_0.is_hitable = false;
							rope_coll->set_vertex_data(0, vertex_data_0);

							auto vertex_data_1 = rope_coll->get_vertex_data(rope_coll->get_collider_size() - 1);
							vertex_data_1.is_hitable = false;
							rope_coll->set_vertex_data(rope_coll->get_collider_size() - 1, vertex_data_1);
						}
						// rendererのアタッチ&セット
						auto renderer = rope_go->addComponent<Rope_renderer>();
						rope_go->renderer = renderer;
						renderer->radius = rope_sphere_r * 0.6f;
						renderer->split_count = 20;
						renderer->color = Vector4(stair_color, 1);
						renderer->set_meshoffset(rope_coll->get_vertex_offset());
					}

					// 腕とropeをつなぐjoint
					Joint::add_balljoint(
						base_coll, rope_coll->get_collider(0),
						rope_joint_start_offset, Vector3(0)
					);

					// つなげるものとropeをつなぐjoint
					Joint::add_balljoint(
						sphere_coll, rope_coll->get_collider(rope_coll->get_collider_size() - 1),
						//vector3_quatrotate(contact_point - data.coll->transform->position, data.coll->transform->orientation.inverse()),
						Vector3(0, sphere_r - 0.5f, 0),
						Vector3(0),
						0.1f
					);

				}
			}
			// house
			{
				Gameobject* house = Gameobject_manager::create("house");
				third_zone->add_child(house);

				// 外床
				set_box(Vector3(85, 1, -98), Vector3(9, 10, 42), Vector3(0, 0, 0), base_color, house);
				set_box(Vector3(113, 1, -64.5f), Vector3(19, 10, 8.5f), Vector3(0, 0, 0), base_color, house);
				set_box(Vector3(154, 1, -64.5f), Vector3(22, 10, 8.5f), Vector3(0, 0, 0), grass_color, house);

				// 中壁
				set_door(Vector3(95, 15, -105.5f), Vector3(2, 4, 0.5f), Vector3(0, 90, 0), true, house); //door
				set_door(Vector3(95, 15, -109.5f), Vector3(2, 4, 0.5f), Vector3(0, 90, 0), false, house); //door
				set_box(Vector3(95, 21, -101), Vector3(2, 10, 1), Vector3(0), base_color, house); //door横柱
				set_box(Vector3(95, 21, -114), Vector3(2, 10, 1), Vector3(0), base_color, house); //door横柱
				set_box(Vector3(95, 11, -89.25f), Vector3(1, 20, 14.25f), Vector3(0, 0, 0), base_color, house); //左
				set_box(Vector3(95, 25, -107.5f), Vector3(1, 6, 4), Vector3(0, 0, 0), base_color, house); //上
				set_box(Vector3(95, 21, -125.75f), Vector3(1, 10, 14.25f), Vector3(0, 0, 0), base_color, house); //右
				set_box(Vector3(135, 11, -74), Vector3(41, 20, 1), Vector3(0, 0, 0), base_color, house); //右
				set_box(Vector3(175, 21, -64.5f), Vector3(1, 10, 8.5f), Vector3(0, 0, 0), base_color, house); //右

				// 中床
				set_box(Vector3(114.0f, 1, -114.0f), Vector3(20, 10, 26), Vector3(0, 0, 0), base_color, house);
				set_box(Vector3(121.5f, 1, -81.5f), Vector3(12.5f, 10, 7.5f), Vector3(0, 0, 0), base_color, house);
				set_box(Vector3(101.5f, 0.5f, -80.5f), Vector3(7.5f, 10, 7.5f), Vector3(0, 0, 0), wood_color, house);


				set_box(Vector3(160, 1, -79), Vector3(26, 10, 6), Vector3(0, 0, 0), base_color, house);

				// 中床 奥の遊び
				set_box(Vector3(180, 1.5f, -71), Vector3(4, 10.5f, 2), Vector3(0, 0, 0), base_color, house); //階段
				set_box(Vector3(180, 2.0f, -67), Vector3(4, 11.0f, 2), Vector3(0, 0, 0), base_color, house); //階段
				set_box(Vector3(180, 2.5f, -63), Vector3(4, 11.5f, 2), Vector3(0, 0, 0), base_color, house); //階段
				set_box(Vector3(180, 3.0f, -54), Vector3(4, 12.0f, 7), Vector3(0, 0, 0), grass_color, house); //上床
				set_box(Vector3(192, 3.0f, -62), Vector3(8, 12.0f, 15), Vector3(0, 0, 0), grass_color, house);//上床
				set_box(Vector3(175, 3.0f, -51.5f), Vector3(1, 12.0f, 4.5f), Vector3(0, 0, 0), grass_color, house);//上床
			}
			// rope_tute
			{
				Gameobject* rope_tute = Gameobject_manager::create("rope_tute");
				third_zone->add_child(rope_tute);

				std::shared_ptr<Material> mato_mat = Material_manager::create_material("mato_material");
				mato_mat->get_texture()->Load(L"./DefaultTexture/stage/mato.png");
				mato_mat->BS_state = State_manager::BStypes::BS_ALPHA;
				mato_mat->Load_VS("./DefaultShader/sprite_vs.cso");
				mato_mat->Load_PS("./DefaultShader/sprite_ps.cso");
				mato_mat->is_render_shadow = false;
				mato_mat->color = Vector4(0.8f, 0.8f, 0.8f, 1);

				// 台座
				set_box(Vector3(126, 1.5f, -122), Vector3(11, 10.5f, 12), Vector3(0, 0, 0), base_color, rope_tute);
				set_box(Vector3(127, 12.5f, -122), Vector3(10, 0.5f, 10), Vector3(0, 0, 0), base_color, rope_tute);
				set_box(Vector3(129, 13.5f, -122), Vector3(8, 0.5f, 8), Vector3(0, 0, 0), base_color, rope_tute);

				// ropeが使えるようになる
				set_event(Vector3(129, 16, -122), rope_tute);

				// 足場01
				set_box(Vector3(183, 2, -122), Vector3(10, 10, 10), Vector3(0, 0, 0), base_color, rope_tute);
				set_box(Vector3(194, 12, -122), Vector3(1, 20, 10), Vector3(0, 0, 0), base_color, rope_tute); //壁
				auto plane_0 = set_plane(Vector3(192, 22, -122), Vector3(10, 10, 1), Vector3(0, -90, 0), Vector3(1), rope_tute);
				plane_0->renderer->set_material(mato_mat);

				// 足場02
				set_box(Vector3(184, 20, -183), Vector3(10, 10, 10), Vector3(0, 0, 0), base_color, rope_tute);
				set_box(Vector3(184, 30, -194), Vector3(10, 20, 1), Vector3(0, 0, 0), base_color, rope_tute); //壁
				auto plane_1 = set_plane(Vector3(184, 40, -192), Vector3(10, 10, 1), Vector3(0, 0, 0), Vector3(1), rope_tute);
				plane_1->renderer->set_material(mato_mat);

				// 足場03
				set_box(Vector3(120, 25, -190), Vector3(10, 10, 10), Vector3(0, 0, 0), base_color, rope_tute);
				//set_box(Vector3(109, 35, -190), Vector3(1, 20, 10), Vector3(0, 0, 0), base_color, rope_tute); //壁  forth_zoneの壁で代用
				auto plane_2 = set_plane(Vector3(111, 45, -190), Vector3(10, 10, 1), Vector3(0, 90, 0), Vector3(1), rope_tute);
				plane_2->renderer->set_material(mato_mat);

				// 階段
				set_box(Vector3(114, 23.0f, -177), Vector3(4, 12.0f, 3), Vector3(0, 0, 0), base_color, rope_tute);
				set_box(Vector3(114, 23.5f, -172), Vector3(4, 12.5f, 2), Vector3(0, 0, 0), base_color, rope_tute);
				set_box(Vector3(114, 24.0f, -168), Vector3(4, 13.0f, 2), Vector3(0, 0, 0), base_color, rope_tute);
				set_box(Vector3(114, 24.5f, -164), Vector3(4, 13.5f, 2), Vector3(0, 0, 0), base_color, rope_tute);
				set_box(Vector3(114, 25.0f, -160), Vector3(4, 14.0f, 2), Vector3(0, 0, 0), base_color, rope_tute);
				set_box(Vector3(114, 25.5f, -156), Vector3(4, 14.5f, 2), Vector3(0, 0, 0), base_color, rope_tute);
				set_box(Vector3(114, 26.0f, -152), Vector3(4, 15.0f, 2), Vector3(0, 0, 0), base_color, rope_tute);
				set_box(Vector3(114, 26.5f, -148), Vector3(4, 15.5f, 2), Vector3(0, 0, 0), base_color, rope_tute);

				set_box(Vector3(114, 27.0f, -142), Vector3(4, 16.0f, 4), Vector3(0, 0, 0), base_color, rope_tute); //遊び場

				set_box(Vector3(108, 27.0f, -142), Vector3(2, 16.0f, 4), Vector3(0, 0, 0), base_color, rope_tute);
				set_box(Vector3(104, 27.5f, -142), Vector3(2, 16.5f, 4), Vector3(0, 0, 0), base_color, rope_tute);
				set_box(Vector3(100, 28.0f, -142), Vector3(2, 17.0f, 4), Vector3(0, 0, 0), base_color, rope_tute);
				set_box(Vector3(96, 28.5f, -142), Vector3(2, 17.5f, 4), Vector3(0, 0, 0), base_color, rope_tute);
				set_box(Vector3(92, 29.0f, -142), Vector3(2, 18.0f, 4), Vector3(0, 0, 0), base_color, rope_tute);
				set_box(Vector3(88, 29.5f, -142), Vector3(2, 18.5f, 4), Vector3(0, 0, 0), base_color, rope_tute);
				set_box(Vector3(84, 30.0f, -142), Vector3(2, 19.0f, 4), Vector3(0, 0, 0), base_color, rope_tute);
				set_box(Vector3(80, 30.5f, -142), Vector3(2, 19.5f, 4), Vector3(0, 0, 0), base_color, rope_tute);

				set_box(Vector3(74, 30.5f, -142), Vector3(4, 19.5f, 4), Vector3(0, 0, 0), base_color, rope_tute); //遊び場

				set_box(Vector3(74, 31.0f, -148), Vector3(4, 20.0f, 2), Vector3(0, 0, 0), base_color, rope_tute);
				set_box(Vector3(74, 31.5f, -152), Vector3(4, 20.5f, 2), Vector3(0, 0, 0), base_color, rope_tute);
				set_box(Vector3(74, 32.0f, -156), Vector3(4, 21.0f, 2), Vector3(0, 0, 0), base_color, rope_tute);
				set_box(Vector3(74, 32.5f, -160), Vector3(4, 21.5f, 2), Vector3(0, 0, 0), base_color, rope_tute);
				set_box(Vector3(74, 33.0f, -164), Vector3(4, 22.0f, 2), Vector3(0, 0, 0), base_color, rope_tute);

			}
			// trees
			{
				Gameobject* tree = Gameobject_manager::create("tree");
				third_zone->add_child(tree);

				set_tree(Vector3(31, 18.05f, -155.5f), Vector3(1, 1.2f, 1), Vector3(0, 55, 0), 2.5f, 0.8f, tree);

				set_tree(Vector3(168, 11.5f, -65), Vector3(1, 1.2f, 1), Vector3(0, 57, 0), 2.5f, 0.8f, tree);
				set_tree(Vector3(157, 11.5f, -65), Vector3(1, 1.2f, 1), Vector3(0, 57, 0), 2.5f, 0.8f, tree);
				set_tree(Vector3(146, 11.5f, -65), Vector3(1, 1.2f, 1), Vector3(0, 57, 0), 2.5f, 0.8f, tree);
				set_tree(Vector3(102.7f, 10.5f, -81.7f), Vector3(1, 1.2f, 1), Vector3(0, 12, 0), 2.5f, 0.8f, tree);

				set_tree(Vector3(193, 15, -70), Vector3(1, 1.2f, 1), Vector3(0, 79, 0), 1.2f, 0.8f, tree);

			}
			// fences
			{
				Gameobject* fences = Gameobject_manager::create("fences");
				third_zone->add_child(fences);

				set_fence(Vector3(-26, 30, 19), Vector3(1), Vector3(0, 00, 0), fences);
				set_fence(Vector3(-19, 30, -54), Vector3(1), Vector3(0, 00, 0), fences);
				set_fence(Vector3(-4, 30, -82), Vector3(1), Vector3(0, 90, 0), fences);
				set_fence(Vector3(-44, 11, -121), Vector3(1), Vector3(0, 90, 0), fences);
				set_fence(Vector3(189, 15, -51), Vector3(1), Vector3(0, 0, 0), fences);
			}
		}

		{}

		// forth_zone
		{
			Gameobject* forth_zone = Gameobject_manager::create("forth_zone");
			stage_parts.emplace_back(forth_zone);

			// floor
			{
				Gameobject* floor = Gameobject_manager::create("floor");
				forth_zone->add_child(floor);

				set_box(Vector3(30, 55, -172), Vector3(10, 1, 8), Vector3(0, 0, 0), grass_color, floor);
				set_box(Vector3(30, 24, -172), Vector3(10, 30, 8), Vector3(0, 0, 0), base_color, floor);
				set_box(Vector3(94, 25, -163), Vector3(16, 30, 17), Vector3(0, 0, 0), base_color, floor);
				set_box(Vector3(74, 33, -173), Vector3(4, 22, 7), Vector3(0, 0, 0), base_color, floor);
				set_box(Vector3(60, 25, -189), Vector3(50, 30, 9), Vector3(0, 0, 0), base_color, floor);
				set_box(Vector3(101, 25, -218), Vector3(9, 30, 20), Vector3(0, 0, 0), base_color, floor);

				set_box(Vector3(50, 54, -202), Vector3(6, 1, 2), Vector3(0, 0, 0), base_color, floor); //goal扉奥の床
			}
			// wall
			{
				Gameobject* wall = Gameobject_manager::create("wall");
				forth_zone->add_child(wall);

				set_box(Vector3(91, 35, -218), Vector3(1, 40, 20), Vector3(0, 0, 0), base_color, wall);
				set_box(Vector3(101, 75, -237), Vector3(9, 20, 1), Vector3(0, 0, 0), base_color, wall);

				set_door(Vector3(52, 59, -199), Vector3(2, 4, 0.5f), Vector3(0, 180, 0), true, wall); //door
				set_door(Vector3(48, 59, -199), Vector3(2, 4, 0.5f), Vector3(0, 180, 0), false, wall); //door
				set_box(Vector3(72, 35, -199), Vector3(18, 40, 1), Vector3(0, 0, 0), base_color, wall); //左
				set_box(Vector3(50, 69, -199), Vector3(4, 6, 1), Vector3(0, 0, 0), base_color, wall); //真ん中
				set_box(Vector3(28, 35, -199), Vector3(18, 40, 1), Vector3(0, 0, 0), base_color, wall); //右
				set_box(Vector3(50, 25, -199), Vector3(4, 30, 1), Vector3(0, 0, 0), base_color, wall); //下

				set_box(Vector3(57, 65, -197.5F), Vector3(1, 10, 0.5F), Vector3(0, 0, 0), base_color, wall); //goal横でっぱり左
				set_box(Vector3(43, 65, -197.5F), Vector3(1, 10, 0.5F), Vector3(0, 0, 0), base_color, wall); //goal横でっぱり右
			}

			// trees
			{
				Gameobject* tree = Gameobject_manager::create("tree");
				forth_zone->add_child(tree);

				set_tree(Vector3(31, 56, -172), Vector3(1, 1.2f, 1), Vector3(0, 76, 0), 1.2f, 0.8f, tree);

			}

			// fences
			{
				Gameobject* fences = Gameobject_manager::create("fences");
				forth_zone->add_child(fences);

				set_fence(Vector3(16, 55, -189), Vector3(1), Vector3(0, 90, 0), fences);

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
				auto plane = set_plane(Vector3(49.8f, 65, -197), Vector3(2.5f, 1.5f, 1), Vector3(0, 0, 0), Vector3(base_color), forth_zone);
				plane->name = std::string("exit_plane");
				plane->renderer->set_material(exit_mat);

			}
		}

		//set_box(Vector3(0, 100, 0), Vector3(10, 10, 10), Vector3(0, 0, 0), grass_color);
		//set_box(Vector3(0, 100, 0), Vector3(10, 10, 10), Vector3(0, 0, 0), grass_color);

		//set_lever(Vector3(0,0,0),Vector3(1),   Vector3(0), Stage_parts::Stageparts_tags::Flag_4, Stage_parts::Stageparts_tags::Flag_4);
		//set_lever(Vector3(8,18,104),Vector3(1),Vector3(0,90,0), Stage_parts::Stageparts_tags::Flag_4, Stage_parts::Stageparts_tags::Flag_4);


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

		{}

		// goal&respown_areas
		{
			Gameobject* goal_and_respown_areas = Gameobject_manager::create("goal&respown_areas");
			stage_parts.emplace_back(goal_and_respown_areas);

			set_respown_area(Vector3(36, 18, 70), Vector3(34, 5, 25), Vector3(0), 1, Vector3(33, 100, 62), goal_and_respown_areas);
			set_respown_area(Vector3(-37, 27, 35), Vector3(15, 10, 12), Vector3(0), 2, Vector3(-30, 100, 35), goal_and_respown_areas);
			set_respown_area(Vector3(-36, 20, -113), Vector3(34, 20, 61), Vector3(0), 3, Vector3(-18, 100, -72), goal_and_respown_areas);
			set_respown_area(Vector3(117, 20, -113), Vector3(34, 20, 55), Vector3(0), 4, Vector3(105, 100, -120), goal_and_respown_areas);
			set_respown_area(Vector3(107, 52.5f, -189), Vector3(10, 20, 50), Vector3(0), 5, Vector3(100, 100, -158), goal_and_respown_areas);

			set_goal_area(Vector3(51, 51, -221), Vector3(20, 2, 20), Vector3(0), Stage_types::stage_3, goal_and_respown_areas);
		}
#endif

	}

	void Stage_02::summon_block_01() {

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
	void Stage_02::update() {

	}

	void Stage_02::Update_hierarchy() {

	}

	void Stage_02::stage_destroy() {
		for (auto& object : stage_parts) {
			Gameobject_manager::deleteGameobject(object, true);
		}
		stage_parts.clear();
	}

}
