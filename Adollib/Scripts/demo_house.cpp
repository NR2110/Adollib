

#include "demo_house.h"
#include "../Adollib/Scripts/Object/gameobject_manager.h"
#include "../Adollib/Scripts/Scene/scene_manager.h"

#include "../Adollib/Scripts/Math/closest_func.h"
#include "../Adollib/Scripts/Main/defines.h"
#include "../Adollib/Scripts/Imgui/imgui_all.h"
#include "../Adollib/Scripts/Physics/ALP__physics_manager.h"

#include "../Adollib/Scripts/Physics/ray.h"

#include "../Adollib/Scripts/Imgui/debug.h"

#include "../Adollib/Scripts/Main/systems.h"

#include "../Adollib/Scripts/Physics/joint.h"

#include "../Adollib/Scripts/Renderer/renderer_base.h"

namespace Adollib
{
	// 所属するシーンの初期化時に一度だけ呼ばれる
	void Demo_house::awake() {

	}

	void Demo_house::start()
	{
		camera = Gameobject_manager::find("camera");

		enum class Stage_types {

			Cube,
			Sphere,
			Capsule,
			Mesh_Cube,
			Shaclo,
			Teach
		};


		Gameobject* GO = nullptr;


		{
			GO = Gameobject_manager::createCube("stage");
			GO->transform->local_pos = Vector3(0, -60, 0);
			GO->transform->local_scale = Vector3(80, 60, 80);
			//GO->transform->local_orient = quaternion_from_euler(0, 45, 0);
			GO->renderer->color = Vector4(188, 214, 54, 255) / 255.0f;

			Collider* coll = GO->addComponent<Collider>();
			Box* box = coll->add_shape<Box>();
			//Meshcoll* R = GO->addComponent<Meshcoll>();
			//R->load_mesh("./DefaultModel/cube.fbx");
			coll->physics_data.is_moveable = false;
			coll->is_static = true;
			coll->tag = Collider_tags::Stage;
		}

		//Tree
		{
			Gameobject* pearent = Gameobject_manager::create("Trees");
			int numn_max = 8;
			for (int x_num = 0; x_num < numn_max; x_num++) {

				for (int z_num = 0; z_num < numn_max; z_num++) {

					if (1 < z_num && z_num < 6 &&
						1 < x_num && x_num < 6)continue;

					pearent->add_child(
						set_tree(
							Vector3(x_num * 17 - 60 + (float)(rand() % 6) - 3,
								0,
								z_num * 17 - 60 + (float)(rand() % 6) - 3
							),
							Vector3(1, 0.9f + (float)(rand() % 100) * 0.01f * 0.5f, 1),
							Vector3(0, (float)(rand() % 180), 0),
							Vector3(1)
						));
				}
			}


		}

		//house
		{
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
				wall_pearent->transform->local_pos = pos[c];
				wall_pearent->transform->local_orient = quaternion_from_euler(rotate[c]);

				for (int i = 0; i < count.y; i++) {
					if ((i + c) % 2 == 0) {
						for (int o = 0; o < count.x; o++) {
							wall_pearent->add_child(set_box(Vector3(
								size.x * 2 * o - size.x * count.x,
								size.y * 2 * i + size.y,
								0
							),
								size,
								Vector3(0, 0, 0),
								color + Vector3(rand() % 5 / 255.0f))
							);
						}
					}
					else {
						for (int o = 0; o < count.x + 1; o++) {
							wall_pearent->add_child(set_box(Vector3(
								size.x * 2 * o - size.x - size.x * count.x,
								size.y * 2 * i + size.y,
								0
							),
								size,
								Vector3(0, 0, 0),
								color + Vector3(rand() % 5 / 255.0f))
							);
						}
					}

				}
			}
		}

		//desk
		{

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
					Gameobject* Daruma = nullptr;
					Daruma = Gameobject_manager::create("Daruma", GO_tag::Sphere);
					Daruma->transform->local_pos = Vector3(position[0], position[1], position[2]);
					Daruma->transform->local_scale = Vector3(1, 1, 1) * 0.5f;

					Collider* coll = Daruma->addComponent<Collider>();
					coll->tag = Collider_tags::Stage;
					//coll->set_tensor(tensor);
					//{
					//	Sphere* sphere[2] = { nullptr };
					//	sphere[0] = coll->add_shape<Sphere>();
					//	sphere[1] = coll->add_shape<Sphere>();

					//	sphere[0]->center = Vector3(0, 1, 0);
					//	sphere[0]->r = 1;
					//	sphere[1]->center = Vector3(0, 2, 0);
					//	sphere[1]->r = 0.5f;
					//}

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
					//parts[0]->renderer->color = Vector4(238, 229, 224, 255) / 255;
					////parts[1]->renderer->color = Vector4(207, 171, 142, 255) / 255;
					//parts[1]->renderer->color = Vector4(255, 77, 17, 255) / 255;

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

					Daruma->add_child(parts[0]);
					Daruma->add_child(parts[1]);
					Daruma->add_child(parts[2]);
					Daruma->add_child(parts[3]);
					Daruma->add_child(parts[4]);


				}

			}
		}

		{
			Gameobject* pearent = Gameobject_manager::create("BallJoint_Shperenet");
			pearent->transform->local_pos = Vector3(0, 60, 0);
			std::vector<Collider*>colls;

			int sphere_size = 10;
			const int colls_size = sphere_size * sphere_size;
			colls.resize(colls_size);

			for (int xaxis = 0; xaxis < sphere_size; xaxis++) {
				for (int zaxis = 0; zaxis < sphere_size; zaxis++) {
					int index = xaxis * sphere_size + zaxis;

					Collider* coll = nullptr;
					Gameobject* go = nullptr;

					go = set_sphere(coll, Vector3(
						(xaxis - sphere_size * 0.5f) * 2,
						2,
						(zaxis - sphere_size * 0.5f) * 2
					),
						1,
						Vector3(1, 0, (1.0f / (sphere_size * sphere_size)) * index)
					);

					pearent->add_child(go);
					colls.at(index) = (coll);

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

	}

	// 毎フレーム呼ばれる更新処理
	void Demo_house::update()
	{
		//ImGuiWindowFlags flag = 0;

		//if (input->getMouseTrigger(Mouse::LBUTTON)) {
		//	auto go = Gameobject_manager::createSphere();
		//	auto coll = go->addComponent<Collider>();
		//	coll->add_shape<Sphere>();

		//	go->transform->local_pos = camera->transform->local_pos;
		//	coll->add_force(vector3_quatrotate(Vector3(0, 0, 1), camera->transform->local_orient) * 4000);
	//	}

		//flag |= ImGuiWindowFlags_AlwaysAutoResize;

	/*	Quaternion a = quaternion_from_euler(0, 180, 0);
		Vector3 b = Vector3(0, 180, 0);
		Vector3 c = Vector3(0, -180, 0);
		Quaternion d = quaternion_from_to_rotate(b.unit_vect(), c.unit_vect());*/

		//#ifdef UseImgui
		//		if (ImGui::Begin("object_manage", 0, flag)) {
		//
		//			ImGui::InputFloat3(std::string("clear_color").c_str(), &Systems::clear_color[0], "%.3f"); ImGui::NextColumn();
		//
		//			ImGui::Columns(5, "columnListIDs");
		//			ImGui::Separator();
		//			ImGui::Text("NAME"); ImGui::NextColumn();
		//			ImGui::Text("SUMMON"); ImGui::NextColumn();
		//			ImGui::Text("POSITION"); ImGui::NextColumn();
		//			ImGui::Text("SIZE"); ImGui::NextColumn();
		//			ImGui::Text("COUNT"); ImGui::NextColumn();
		//			ImGui::Separator();
		//
		//			int imgui_num = 0;
		//			//BOXpyramid
		//			{
		//				static int BOX_pyramid_count = 5;
		//				static float BOX_pyramid_pos[3] = { 0 };
		//				static int BOX_pyramid_size[3] = { 1,1,1 };
		//				bool summon = false;
		//				ImGui::Separator();
		//				ImGui::Text("BOX_pyramid"); ImGui::NextColumn();
		//				ImGui::Checkbox(std::to_string(imgui_num + 100).c_str(), &summon); ImGui::NextColumn();
		//				ImGui::DragFloat3(std::to_string(imgui_num + 200).c_str(), BOX_pyramid_pos, 0.1f); ImGui::NextColumn();
		//				ImGui::DragInt3(std::to_string(imgui_num + 250).c_str(), BOX_pyramid_size, 0.1f); ImGui::NextColumn();
		//				ImGui::DragInt(std::to_string(imgui_num + 300).c_str(), &BOX_pyramid_count, 1, 1, 100000); ImGui::NextColumn();
		//
		//				if (summon == true) {
		//					Vector3 size = Vector3(2, 1, 1);
		//					Gameobject* pearent = Gameobject_manager::create("BOXpyramid");
		//					for (int i = 0; i < BOX_pyramid_size[1]; i++) {
		//						if (i % 2 == 0) {
		//							for (int o = 0; o < BOX_pyramid_size[0]; o++) {
		//								pearent->add_child(set_box(Vector3(
		//									BOX_pyramid_pos[0] + size.x * 2 * o - size.x * BOX_pyramid_size[0],
		//									BOX_pyramid_pos[1] + size.y * 2 * i + size.y,
		//									BOX_pyramid_pos[2]),
		//									size,
		//									Vector3(0, 0, 0),
		//									Vector3(0, 1, 1))
		//								);
		//							}
		//						}
		//						else {
		//							for (int o = 0; o < BOX_pyramid_size[0] + 1; o++) {
		//								pearent->add_child(set_box(Vector3(
		//									BOX_pyramid_pos[0] + size.x * 2 * o - size.x - size.x * BOX_pyramid_size[0],
		//									BOX_pyramid_pos[1] + size.y * 2 * i + size.y,
		//									BOX_pyramid_pos[2]),
		//									size,
		//									Vector3(0, 0, 0),
		//									Vector3(0, 1, 1))
		//								);
		//							}
		//						}
		//
		//					}
		//				}
		//				imgui_num++;
		//			}
		//
		//			//SPHEREpyramid
		//			{
		//				static int SPHERE_pyramid_count = 5;
		//				static float SPHERE_pyramid_pos[3] = { 0 };
		//				bool summon = false;
		//				ImGui::Separator();
		//				ImGui::Text("SPHERE_pyramid"); ImGui::NextColumn();
		//				ImGui::Checkbox(std::to_string(imgui_num + 100).c_str(), &summon); ImGui::NextColumn();
		//				ImGui::DragFloat3(std::to_string(imgui_num + 200).c_str(), SPHERE_pyramid_pos, 0.1f); ImGui::NextColumn(); ImGui::NextColumn();
		//				ImGui::DragInt(std::to_string(imgui_num + 300).c_str(), &SPHERE_pyramid_count, 1, 1, 100000); ImGui::NextColumn();
		//
		//				if (summon == true) {
		//					Gameobject* pearent = Gameobject_manager::create("Spherepyramid");
		//					for (int i = 0; i < SPHERE_pyramid_count; i++) {
		//						for (int o = 0; o < SPHERE_pyramid_count - i; o++) {
		//							pearent->add_child(
		//								set_sphere(
		//									Vector3(2.50001f * o - (SPHERE_pyramid_count - i) * 2.500001f / 2.0f + SPHERE_pyramid_pos[0],
		//										5.0f + 2.50001f * i + SPHERE_pyramid_pos[1],
		//										SPHERE_pyramid_pos[2]),
		//									1,
		//									Vector3(0, 1, 1)
		//								));
		//						}
		//
		//					}
		//				}
		//				imgui_num++;
		//			}
		//
		//			//CAPSULEpyramid
		//			{
		//				static int CAPSULE_pyramid_count = 5;
		//				static float CAPSULE_pyramid_pos[3] = { 0 };
		//				bool summon = false;
		//				ImGui::Separator();
		//				ImGui::Text("CAPSULE_pyramid"); ImGui::NextColumn();
		//				ImGui::Checkbox(std::to_string(imgui_num + 100).c_str(), &summon); ImGui::NextColumn();
		//				ImGui::DragFloat3(std::to_string(imgui_num + 200).c_str(), CAPSULE_pyramid_pos, 0.1f); ImGui::NextColumn(); ImGui::NextColumn();
		//				ImGui::DragInt(std::to_string(imgui_num + 300).c_str(), &CAPSULE_pyramid_count, 1, 1, 100000); ImGui::NextColumn();
		//
		//				if (summon == true)
		//					for (int i = 0; i < CAPSULE_pyramid_count; i++) {
		//						for (int o = 0; o < CAPSULE_pyramid_count - i; o++) {
		//							set_capsule(Vector3(2.50001f * o - (CAPSULE_pyramid_count - i) * 2.500001f / 2.0f + CAPSULE_pyramid_pos[0], 5.0f + 4.50001f * i + CAPSULE_pyramid_pos[1], CAPSULE_pyramid_pos[2]), 1, 1, Vector3(0, 1, 1));
		//						}
		//
		//					}
		//				imgui_num++;
		//			}
		//
		//			//MIXpyramid
		//			{
		//				static int MIX_pyramid_count = 5;
		//				static float MIX_pyramid_pos[3] = { 0 };
		//				bool summon = false;
		//				ImGui::Separator();
		//				ImGui::Text("MIX_pyramid"); ImGui::NextColumn();
		//				ImGui::Checkbox(std::to_string(imgui_num + 100).c_str(), &summon); ImGui::NextColumn();
		//				ImGui::DragFloat3(std::to_string(imgui_num + 200).c_str(), MIX_pyramid_pos, 0.1f); ImGui::NextColumn(); ImGui::NextColumn();
		//				ImGui::DragInt(std::to_string(imgui_num + 300).c_str(), &MIX_pyramid_count, 1, 1, 100000); ImGui::NextColumn();
		//
		//				if (summon == true)
		//					for (int i = 0; i < MIX_pyramid_count; i++) {
		//						for (int o = 0; o < MIX_pyramid_count - i; o++) {
		//							if ((i + o) % 2 == 0)
		//								set_box(Vector3(2.50001f * o - (MIX_pyramid_count - i) * 2.500001f / 2.0f + MIX_pyramid_pos[0], 5.0f + 2.50001f * i + MIX_pyramid_pos[1], MIX_pyramid_pos[2]), Vector3(1, 1, 1), Vector3(30, 0, 30), Vector3(0, 1, 1));
		//							else
		//								set_sphere(Vector3(2.50001f * o - (MIX_pyramid_count - i) * 2.500001f / 2.0f + MIX_pyramid_pos[0], 5.0f + 2.50001f * i + MIX_pyramid_pos[1], MIX_pyramid_pos[2]), 1, Vector3(0, 1, 1));
		//						}
		//
		//					}
		//				imgui_num++;
		//			}
		//
		//			//deskpyramid
		//			{
		//				static int count = 5;
		//				static float pos[3] = { 0 };
		//				bool summon = false;
		//				ImGui::Separator();
		//				ImGui::Text("desk_pyramid"); ImGui::NextColumn();
		//				ImGui::Checkbox(std::to_string(imgui_num + 100).c_str(), &summon); ImGui::NextColumn();
		//				ImGui::DragFloat3(std::to_string(imgui_num + 200).c_str(), pos, 0.1f); ImGui::NextColumn(); ImGui::NextColumn();
		//				ImGui::DragInt(std::to_string(imgui_num + 300).c_str(), &count, 1, 1, 100000); ImGui::NextColumn();
		//
		//				float dis = 1.0f;
		//				if (summon == true) {
		//					Matrix33 tensor = matrix33_identity();
		//					Vector3 s = Vector3(2, 0.75f, 1.5f);
		//					tensor._11 = 0.3f * (s.y * s.y + s.z * s.z);
		//					tensor._22 = 0.3f * (s.z * s.z + s.x * s.x);
		//					tensor._33 = 0.3f * (s.x * s.x + s.y * s.y);
		//
		//					for (int i = 0; i < count; i++) {
		//
		//						for (int o = 0; o < count - i; o++) {
		//							const Vector3& position = Vector3(4.20001f * o - (count - i) * 4.500001f / 2.0f + pos[0],
		//								5.0f + 2.50001f * i + pos[1],
		//								pos[2]);
		//
		//
		//							// darumaの生成とcolliderのアタッチ
		//							Gameobject* Daruma = nullptr;
		//							Daruma = Gameobject_manager::create("Daruma", GO_tag::Sphere);
		//							Daruma->transform->local_pos = Vector3(position[0], position[1], position[2]);
		//							Daruma->transform->local_scale = Vector3(1, 1, 1) * 0.5f;
		//
		//							Collider* coll = Daruma->addComponent<Collider>();
		//							//coll->set_tensor(tensor);
		//							//{
		//							//	Sphere* sphere[2] = { nullptr };
		//							//	sphere[0] = coll->add_shape<Sphere>();
		//							//	sphere[1] = coll->add_shape<Sphere>();
		//
		//							//	sphere[0]->center = Vector3(0, 1, 0);
		//							//	sphere[0]->r = 1;
		//							//	sphere[1]->center = Vector3(0, 2, 0);
		//							//	sphere[1]->r = 0.5f;
		//							//}
		//
		//							{
		//								Box* box[5] = { nullptr };
		//								box[0] = coll->add_shape<Box>();
		//								box[1] = coll->add_shape<Box>();
		//								box[2] = coll->add_shape<Box>();
		//								box[3] = coll->add_shape<Box>();
		//								box[4] = coll->add_shape<Box>();
		//
		//								box[0]->center = Vector3(0, 0.75f, 0);
		//								box[0]->size = Vector3(4, 0.5f, 3);
		//
		//								box[1]->center = Vector3(+2.8f, -0.75f, +1.8f);
		//								box[1]->size = Vector3(0.5f, 1, 0.5f);
		//								box[2]->center = Vector3(+2.8f, -0.75f, -1.8f);
		//								box[2]->size = Vector3(0.5f, 1, 0.5f);
		//								box[3]->center = Vector3(-2.8f, -0.75f, +1.8f);
		//								box[3]->size = Vector3(0.5f, 1, 0.5f);
		//								box[4]->center = Vector3(-2.8f, -0.75f, -1.8f);
		//								box[4]->size = Vector3(0.5f, 1, 0.5f);
		//							}
		//
		//
		//							// darumaが描画されるようにGOをdarumaにつける
		//							Gameobject* parts[5] = { nullptr };
		//							parts[0] = Gameobject_manager::createCube();
		//							parts[1] = Gameobject_manager::createCube();
		//							parts[2] = Gameobject_manager::createCube();
		//							parts[3] = Gameobject_manager::createCube();
		//							parts[4] = Gameobject_manager::createCube();
		//							Vector4 C = Vector4(207, 171, 142, 255) / 255;
		//							//parts[0]->renderer->color = Vector4(238, 229, 224, 255) / 255;
		//							////parts[1]->renderer->color = Vector4(207, 171, 142, 255) / 255;
		//							//parts[1]->renderer->color = Vector4(255, 77, 17, 255) / 255;
		//
		//							parts[0]->renderer->color = C;
		//							parts[1]->renderer->color = C;
		//							parts[2]->renderer->color = C;
		//							parts[3]->renderer->color = C;
		//							parts[4]->renderer->color = C;
		//
		//							parts[0]->transform->local_pos = Vector3(0, 0.75f, 0);
		//							parts[0]->transform->local_scale = Vector3(4, 0.5f, 3);
		//
		//							parts[1]->transform->local_pos = Vector3(+2.8f, -0.75f, +1.8f);
		//							parts[1]->transform->local_scale = Vector3(0.5f, 1, 0.5f);
		//							parts[2]->transform->local_pos = Vector3(+2.8f, -0.75f, -1.8f);
		//							parts[2]->transform->local_scale = Vector3(0.5f, 1, 0.5f);
		//							parts[3]->transform->local_pos = Vector3(-2.8f, -0.75f, +1.8f);
		//							parts[3]->transform->local_scale = Vector3(0.5f, 1, 0.5f);
		//							parts[4]->transform->local_pos = Vector3(-2.8f, -0.75f, -1.8f);
		//							parts[4]->transform->local_scale = Vector3(0.5f, 1, 0.5f);
		//
		//
		//							//coll->tag = Collider_tags::Sphere;
		//
		//							Daruma->add_child(parts[0]);
		//							Daruma->add_child(parts[1]);
		//							Daruma->add_child(parts[2]);
		//							Daruma->add_child(parts[3]);
		//							Daruma->add_child(parts[4]);
		//						}
		//
		//					}
		//				}
		//				imgui_num++;
		//			}
		//
		//			//treepyramid
		//			{
		//				static int TREE_pyramid_count = 5;
		//				static float TREE_pyramid_pos[3] = { 0 };
		//				bool summon = false;
		//				ImGui::Separator();
		//				ImGui::Text("TREE_pyramid"); ImGui::NextColumn();
		//				ImGui::Checkbox(std::to_string(imgui_num + 100).c_str(), &summon); ImGui::NextColumn();
		//				ImGui::DragFloat3(std::to_string(imgui_num + 200).c_str(), TREE_pyramid_pos, 0.1f); ImGui::NextColumn(); ImGui::NextColumn();
		//				ImGui::DragInt(std::to_string(imgui_num + 300).c_str(), &TREE_pyramid_count, 1, 1, 100000); ImGui::NextColumn();
		//
		//				if (summon == true) {
		//					Gameobject* pearent = Gameobject_manager::create("Treepyramid");
		//					for (int i = 0; i < TREE_pyramid_count; i++) {
		//						for (int o = 0; o < TREE_pyramid_count - i; o++) {
		//							pearent->add_child(
		//								set_tree(
		//									Vector3(5.0f * o - (TREE_pyramid_count - i) * 5.0f / 2.0f + TREE_pyramid_pos[0],
		//										0,
		//										TREE_pyramid_pos[2]),
		//									Vector3(1),
		//									Vector3(0),
		//									Vector3(1)
		//								));
		//						}
		//
		//					}
		//				}
		//				imgui_num++;
		//			}
		//
		//			ImGui::Columns(1);
		//			ImGui::End();
		//		}
		//#endif

	}




	// このスクリプトがアタッチされているGOのactiveSelfがtrueになった時呼ばれる
	void Demo_house::onEnable()
	{

	}

	// このスクリプトがアタッチされているGOのactiveSelfがfalseになった時呼ばれる
	void Demo_house::onDisable()
	{

	}

}

namespace Adollib
{
	Gameobject* Demo_house::set_tree(Vector3 pos, Vector3 scale, Vector3 rotate, Vector3 color) {
		Gameobject* tree = nullptr;
		tree = Gameobject_manager::create(GO_tag::None);

		Vector4 stem_color = Vector4(90, 47, 27, 255) / 255.0f;
		Vector4 reaf_color = Vector4(48, 188, 0, 255) / 255.0f;
		Vector4 floar_color = Vector4(188, 214, 54, 255) / 255.0f;

		tree->transform->local_pos = pos;
		tree->transform->local_scale = scale;
		tree->transform->local_orient = quaternion_from_euler(rotate);

		const int Tree_size = 7;
		Gameobject* tree_parts[Tree_size] = { nullptr };
		for (int i = 0; i < Tree_size; i++) {
			tree_parts[i] = Gameobject_manager::createCube();
		}

		tree_parts[0]->transform->local_pos = Vector3(0, 7, 0);
		tree_parts[0]->transform->local_scale = Vector3(1, 6.9f, 1);
		tree_parts[0]->renderer->color = stem_color;

		tree_parts[1]->transform->local_pos = Vector3(0, 15, 0);
		tree_parts[1]->transform->local_scale = Vector3(1.5, 1, 1.5);
		tree_parts[1]->transform->local_orient = quaternion_from_euler(0, 0, 0);
		tree_parts[1]->renderer->color = reaf_color;

		tree_parts[2]->transform->local_pos = Vector3(0, 13, 0);
		tree_parts[2]->transform->local_scale = Vector3(2.0, 1, 2.0);
		tree_parts[2]->transform->local_orient = quaternion_from_euler(0, 1.7f, 0);
		tree_parts[2]->renderer->color = reaf_color;

		tree_parts[3]->transform->local_pos = Vector3(0, 11, 0);
		tree_parts[3]->transform->local_scale = Vector3(3.0, 1, 3.0);
		tree_parts[3]->transform->local_orient = quaternion_from_euler(0, 13, 0);
		tree_parts[3]->renderer->color = reaf_color;

		tree_parts[4]->transform->local_pos = Vector3(0, 9, 0);
		tree_parts[4]->transform->local_scale = Vector3(3.5, 1, 3.5);
		tree_parts[4]->transform->local_orient = quaternion_from_euler(0, 28, 0);
		tree_parts[4]->renderer->color = reaf_color;

		tree_parts[5]->transform->local_pos = Vector3(0, 7, 0);
		tree_parts[5]->transform->local_scale = Vector3(4.5, 1, 4.5);
		tree_parts[5]->transform->local_orient = quaternion_from_euler(0, 14, 0);
		tree_parts[5]->renderer->color = reaf_color;

		tree_parts[6]->transform->local_pos = Vector3(0, 0.2f, 0);
		tree_parts[6]->transform->local_scale = Vector3(3, 0.2f, 3);
		tree_parts[6]->transform->local_orient = quaternion_from_euler(0, 0, 0);
		tree_parts[6]->renderer->color = floar_color;

		for (int i = 0; i < Tree_size; i++) {
			tree->add_child(tree_parts[i]);
		}

		Box* boxes[Tree_size];
		Collider* coll = tree->addComponent<Collider>();
		coll->physics_data.inertial_mass = 30;
		coll->tag = Collider_tags::Stage;

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

		return tree;
	}


	Gameobject* Demo_house::set_sphere(Collider*& coll, Vector3 pos, float r, Vector3 color) {
		Gameobject* object = nullptr;
		object = Gameobject_manager::createSphere();
		//Vector4 C = Vector4(color.x, color.y, color.z, 1);
		Vector4 C = Vector4(1, 0, 1, 1);
		object->renderer->color = C;

		//object->addComponent<object_fall>();
		object->transform->local_pos = pos;
		object->transform->local_scale = Vector3(r, r, r);


		coll = object->addComponent<Collider>();
		coll->add_shape<Sphere>();

		coll->tag = Collider_tags::Sphere;
		return object;
	}

	Gameobject* Demo_house::set_box(Vector3 pos, Vector3 size, Vector3 rotate, Vector3 color) {
		Gameobject* object = nullptr;
		object = Gameobject_manager::createCube();
		Vector4 C = Vector4(color.x, color.y, color.z, 1);
		object->renderer->color = C;

		//object->addComponent<object_fall>();
		object->transform->local_orient = quaternion_from_euler(rotate);
		object->transform->local_pos = pos;
		object->transform->local_scale = size;

		Collider* coll = object->addComponent<Collider>();
		coll->physics_data.inertial_mass = 3;
		coll->tag = Collider_tags::Stage;

		Box* box = coll->add_shape<Box>();

		//coll->physics_data.is_moveable = false;

		return object;
	}
	Gameobject* Demo_house::set_capsule(Vector3 pos, float r, float length, Vector3 rotate, Vector3 color) {
		Gameobject* object = nullptr;
		object = Gameobject_manager::create(GO_tag::Box);
		Vector4 C = Vector4(color.x, color.y, color.z, 1);
		//object->renderer->color = C;

		//object->addComponent<object_fall>();
		object->transform->local_orient = quaternion_from_euler(rotate);
		object->transform->local_pos = pos;

		Collider* coll = object->addComponent<Collider>();
		Capsule* capsule = coll->add_shape<Capsule>();
		capsule->r = r;
		capsule->length = length;
		//Meshcoll* M = object->addComponent<Meshcoll>();
		//M->load_mesh("./DefaultModel/cube.fbx");
		//M->inertial_mass = 1;
		coll->tag = Collider_tags::Cylinder;

		return object;
	}

	Gameobject* Demo_house::set_meshbox(Vector3 pos, Vector3 size, Vector3 rotate, Vector3 color) {
		Gameobject* object = nullptr;
		object = Gameobject_manager::createFromFBX("", "./DefaultModel/cone.fbx");
		Vector4 C = Vector4(color.x, color.y, color.z, 1);
		object->renderer->color = C;

		//object->addComponent<object_fall>();
		object->transform->local_orient = quaternion_from_euler(rotate);
		object->transform->local_pos = pos;
		object->transform->local_scale = size;

		//Box* M = object->addComponent<Box>();
		Collider* coll = object->addComponent<Collider>();
		coll->add_mesh_shape("./DefaultModel/cone.fbx");

		//M->inertial_mass = 1;

		return object;
	}

	Gameobject* Demo_house::set_plane(Vector3 pos, Vector3 normal, Vector3 color) {
		Gameobject* object = nullptr;
		object = Gameobject_manager::createCube();
		Vector4 C = Vector4(color.x, color.y, color.z, 1);
		object->renderer->color = C;

		object->transform->local_orient = quaternion_from_euler(normal);
		object->transform->local_pos = pos;
		object->transform->local_scale = Vector3(30, 0.1f, 30);

		Collider* coll = object->addComponent<Collider>();
		Plane* plane = coll->add_shape<Plane>();

		return object;
	}


}