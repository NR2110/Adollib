

#include "stage_demo.h"

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

namespace Adollib
{
	void Stage_demo::stage_awake()
	{
		player_respown_pos = Vector3(-2.7f, 100, -150);
		player_respown_pos = Vector3(-2.7f, 100, -50);
		respown_num = -1;

		auto floar_go = Gameobject_manager::create("floar");
		stage_parts.emplace_back(floar_go);
		//set_box(Vector3(0, -60, 0), Vector3(10,10,10), Vector3(0), Vector3(188, 214, 54) / 255.0f);
		set_box(Vector3(0, -60, 0), Vector3(80, 60, 80), Vector3(0), Vector3(188, 214, 54) / 255.0f, floar_go);
		set_box(Vector3(0, -60, -98), Vector3(30, 30, 60), Vector3(0), Vector3(188, 214, 54) / 255.0f, floar_go);


		if (0)
		{
			auto plane = set_plane(Vector3(0, 10, 0), Vector3(40, 20, 0), Vector3(0));
			auto R = plane->findComponent<Sprite_renderer>();

			auto subcamera_go = Gameobject_manager::create("camera_02");
			auto subcamera_comp = subcamera_go->addComponent<Camera_component>();
			subcamera_comp->is_draw_main_RenderTargetView = false;
			plane->add_child(subcamera_go);
			///subcamera_go->transform->local_orient = quaternion_from_euler(-90, 0, 0);

			stage_parts.emplace_back(subcamera_go);

			plane->transform->local_orient = quaternion_from_euler(0, 180, 0);

			R->get_material()->set_texture(subcamera_comp->get_color_texture());
		}



#if _DEBUG
#else
		//Tree
		{
			Gameobject* pearent = Gameobject_manager::create("Trees");
			stage_parts.emplace_back(pearent);
			int numn_max = 8;
			for (int x_num = 0; x_num < numn_max; x_num++) {

				for (int z_num = 0; z_num < numn_max; z_num++) {

					if (1 < z_num && z_num < 6 &&
						1 < x_num && x_num < 6)continue;

					set_tree(
						Vector3(x_num * 17 - 60 + rand() % 6 - 3,
							0,
							z_num * 17 - 60 + rand() % 6 - 3
						),
						Vector3(1, 0.9f + (rand() % 100) * 0.01f * 0.5f, 1),
						Vector3(0, rand() % 180, 0),
						pearent
					);
				}
			}

		}

		//house
		{
			Gameobject* house = Gameobject_manager::create("House");
			stage_parts.emplace_back(house);

			float mass = 4;
			Vector3 color = Vector3(187, 185, 181) / 255;
			//Vector3 color = Vector3(220, 220, 220) / 255;
			Vector3 size = Vector3(2, 1, 1);
			Vector2 count = Vector2(10, 10);
			Vector3 pos[4] = {
				Vector3(-21,0,-2),
				Vector3(+2,0,-21),
				Vector3(+21,0,-2),
				Vector3(+2,0,+21)
			};
			Vector3 rotate[4] = {
				Vector3(0,90,0),
				Vector3(0,0,0),
				Vector3(0,90,0),
				Vector3(0,0,0)
			};

			for (int c = 0; c < 4; c++) {
				Gameobject* wall_pearent = Gameobject_manager::create("Wall");
				house->add_child(wall_pearent);
				wall_pearent->transform->local_pos = pos[c];
				wall_pearent->transform->local_orient = quaternion_from_euler(rotate[c]);

				for (int i = 0; i < count.y; i++) {
					if ((i + c) % 2 == 0) {
						for (int o = 0; o < count.x; o++) {
							auto coll = set_box(Vector3(
								size.x * 2 * o - size.x * count.x,
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
							//coll->physics_data.dynamic_friction = 0.8f;
							wall_pearent->add_child(coll->gameobject);
						}
					}
					else {
						for (int o = 0; o < count.x + 1; o++) {
							auto coll = set_box(Vector3(
								size.x * 2 * o - size.x - size.x * count.x,
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
							wall_pearent->add_child(coll->gameobject);
						}
					}

				}
			}
		}

		//desk
		{
			Gameobject* desks = Gameobject_manager::create("desks");
			stage_parts.emplace_back(desks);

			int count = 5;
			Vector3 pos = Vector3(0);
			Matrix33 tensor = matrix33_identity();
			Vector3 s = Vector3(2, 0.75f, 1.5f);
			tensor._11 = 0.3f * (s.y * s.y + s.z * s.z);
			tensor._22 = 0.3f * (s.z * s.z + s.x * s.x);
			tensor._33 = 0.3f * (s.x * s.x + s.y * s.y);

			for (int i = 0; i < count; i++) {

				for (int o = 0; o < count - i; o++) {

					const Vector3& position = Vector3(4.20001f * o - (count - i) * 4.500001f / 2.0f + pos[0],
						5.0f + 2.50001f * i + pos[1],
						pos[2]);

					// darumaの生成とcolliderのアタッチ
					Gameobject* desk = nullptr;
					desk = Gameobject_manager::create("Desk", GO_tag::Sphere);
					desks->add_child(desk);

					desk->transform->local_pos = Vector3(position[0], position[1], position[2]);
					desk->transform->local_scale = Vector3(1, 1, 1) * 0.5f;

					Collider* coll = desk->addComponent<Collider>();
					//coll->tag = Collider_tags::Stage | Collider_tags::Kinematic_Stage | Collider_tags::Jumpable_Stage;
					coll->tag = Collider_tags::Stage | Collider_tags::Kinematic_Stage | Collider_tags::Caera_not_sunk_Stage;
					coll->physics_data.inertial_mass = 5;

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

					parts[0]->renderer->color = C;
					parts[1]->renderer->color = C;
					parts[2]->renderer->color = C;
					parts[3]->renderer->color = C;
					parts[4]->renderer->color = C;

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


				}

			}
		}

		{}

		//croth
		if(0)
		{
			Gameobject* pearent = Gameobject_manager::create("BallJoint_Shperenet");
			pearent->transform->local_pos = Vector3(0, 60, 0);
			std::vector<Collider*>colls;
			stage_parts.emplace_back(pearent);

			int sphere_size = 8;
			const int colls_size = sphere_size * sphere_size;
			colls.resize(colls_size);

			for (int xaxis = 0; xaxis < sphere_size; xaxis++) {
				for (int zaxis = 0; zaxis < sphere_size; zaxis++) {
					int index = xaxis * sphere_size + zaxis;

					Collider* coll = nullptr;
					Gameobject* go = nullptr;

					coll = set_sphere(Vector3(
						(xaxis - sphere_size * 0.5f) * 2,
						2,
						(zaxis - sphere_size * 0.5f) * 2
					),
						1,
						Vector3(1, 0, (1.0f / (sphere_size * sphere_size)) * index),
						nullptr,
						false
					);
					go = coll->gameobject;

					pearent->add_child(go);
					colls.at(index) = (coll);

					coll->tag &= ~Collider_tags::Caera_not_sunk_Stage;
					coll->physics_data.inertial_mass = 0.1f;
				}
			}

			for (int xaxis = 0; xaxis < sphere_size; xaxis++) {
				for (int zaxis = 0; zaxis < sphere_size; zaxis++) {
					int index = xaxis * sphere_size + zaxis;

					if (xaxis > 0) Joint::add_balljoint(colls[index], colls[index - sphere_size], Vector3(-1, 0, 0), Vector3(1, 0, 0), 0.1f);
					if (zaxis > 0) Joint::add_balljoint(colls[index], colls[index - 1], Vector3(0, 0, -1), Vector3(0, 0, 1), 0.1f);
				}
			}
		}
#endif

		// 壁
		{
			set_box(Vector3(-10, 8, 81), Vector3(6, 8, 1), Vector3(0), Vector3(188, 214, 54) / 255.0f * 0.95f, floar_go);
			set_box(Vector3(+10, 8, 81), Vector3(6, 8, 1), Vector3(0), Vector3(188, 214, 54) / 255.0f * 0.95f, floar_go);
			set_box(Vector3(0, -1, 83), Vector3(4, 1, 3), Vector3(0), Vector3(188, 214, 54) / 255.0f * 0.95f, floar_go);
			set_box(Vector3(0, 16, 81), Vector3(4, 8, 1), Vector3(0), Vector3(188, 214, 54) / 255.0f * 0.95f, floar_go);

			set_door(Vector3(-2, 4, 81), Vector3(2, 4, 0.5f), Vector3(0),true , floar_go);
			set_door(Vector3(+2, 4, 81), Vector3(2, 4, 0.5f), Vector3(0),false, floar_go);

			set_goal_area(Vector3(0, -8, 90), Vector3(10, 4, 10), Vector3(0), Stage_types::demo_1, floar_go);
		}

		// 下
		{
			{
				Vector3 base_pos = Vector3(120, 2, 0);
				Vector3 off = Vector3(0);
				for (int i = 0; i < 5; i++) {
					set_box(base_pos + off, Vector3(40, 4, 40), Vector3(0), Vector3(188, 214, 54) / 255.0f * pow(1.03f, i + 1), floar_go);
					off += Vector3(10, 8, 0);
				}
			}
			set_box(Vector3(140, 18, 60), Vector3(65, 20, 20), Vector3(0), Vector3(188, 214, 54) / 255.0f, floar_go);

			{
				auto coll = set_box(Vector3(0, -29, -100), Vector3(2, 2, 2), Vector3(0), Vector3(0.8f), floar_go, false);
				coll->tag &= ~Collider_tags::Caera_not_sunk_Stage;
				coll->physics_data.inertial_mass = 50;
			}

			//buttan
			set_buttan(Vector3(0, -2.9f - 30, -115), Vector3(3, 3, 3), Vector3(0), Stage_parts::Stageparts_tags::Flag_0, Vector3(255, 80, 80) / 255.0f, floar_go);

			set_gimmickdoor(
				Vector3(0, -89.5f, -95), Vector3(0, -60, -95),
				Vector3(0, 0, 0), Vector3(0, 0, 0),
				3, 1,
				Vector3(25, 60, 15),
				Stage_parts::Stageparts_tags::Flag_0,
				Vector3(188, 214, 54) / 255.0f * 0.8f,
				floar_go
			);

			set_sphere_rope(Vector3(64, 60, 65), 1, Vector3(0, 0, 0), 10, 5);
		}
	}

	// 毎フレーム呼ばれる更新処理
	void Stage_demo::update() {

	}

	void Stage_demo::Update_hierarchy() {

	}

	void Stage_demo::stage_destroy() {
		for (auto& object : stage_parts) {
			Gameobject_manager::deleteGameobject(object, true);
		}
		stage_parts.clear();
	}

}
