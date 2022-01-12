

#include "stage_demo_2.h"

#include "../Adollib/Scripts/Object/gameobject_manager.h"
#include "../Adollib/Scripts/Scene/scene_manager.h"

#include "../Adollib/Scripts/Math/closest_func.h"
#include "../Adollib/Scripts/Imgui/imgui_all.h"
#include "../Adollib/Scripts/Physics/ALP__physics_manager.h"

#include "../Adollib/Scripts/Physics/ray.h"
#include "../Adollib/Scripts/Imgui/debug.h"
#include "../Adollib/Scripts/Physics/joint.h"
#include "../Adollib/Scripts/Physics/collider_croth.h"
#include "../Adollib/Scripts/Physics/collider_rope.h"

#include "../Adollib/Scripts/Renderer/mesh_renderer.h"
#include "../Adollib/Scripts/Renderer/sprite_renderer.h"
#include "../Adollib/Scripts/Object/component_camera.h"

#include "../Adollib/Scripts/Main/systems.h"

#include "stage_manager.h"
#include "../Adollib/Scripts/Renderer/croth_renderer.h"
#include "../Adollib/Scripts/Renderer/rope_renderer.h"


namespace Adollib
{
	void Stage_demo_02::stage_awake()
	{
		player_respown_pos = Vector3(-2.7f, 80, -5.f);
		respown_num = -1;
		next_stage = Stage_types::none;

		//player_respown_pos = Vector3(-2.7f, 10, -5);
		//player_respown_pos = Vector3(90, 16.0f, 66.0f);
		y_respown_pos = 30; // respownしたときのYの座標

		Gameobject* p = Gameobject_manager::create("stage");
		stage_parts.emplace_back(p);
		set_box(Vector3(0, -60, -0), Vector3(300, 60, 300), Vector3(0), Vector3(188, 214, 54) / 255.0f, p);

		all_pearent = Gameobject_manager::create("all_pearent");
		stage_parts.emplace_back(all_pearent);

		//auto cube = Gameobject_manager::createFromFBX("Shaclo", "../Data/FBX/Model_Shaclo_Winter_Edit.fbx");
		//auto cube = Gameobject_manager::createFromFBX("bunny", "../Data/FBX/bunny.obj");
		//auto cube = Gameobject_manager::createCube();
		//auto cube = Gameobject_manager::createSphere();
		//cube->addComponent<Croth_renderer>();
		//stage_parts.emplace_back(cube);
		//cube->addComponent<Mesh_renderer>();

		//stage_manager = Gameobject_manager::find("Stage_manager")->findComponent<Stage_manager>();
	}

	// 毎フレーム呼ばれる更新処理
	void Stage_demo_02::update() {

		if (stage_manager == nullptr)stage_manager = Gameobject_manager::find("Stage_manager")->findComponent<Stage_manager>();

		if (stage_manager->now_stage != Stage_types::demo_2)return;

		ImGuiWindowFlags flag = 0;
		if (ImGui::Begin("object_manage", 0, flag)) {

			ImGui::InputFloat3(std::string("clear_color").c_str(), &Systems::clear_color[0], "%.3f"); ImGui::NextColumn();

			//objectの削除
			bool del = false;
			ImGui::Checkbox("delete", &del);
			if (del) {
				for (auto& GO : *all_pearent->children()) {
					Gameobject_manager::deleteGameobject(GO, true);
				}
			}

			ImGui::Columns(5, "columnListIDs");
			ImGui::Separator();
			ImGui::Text("NAME"); ImGui::NextColumn();
			ImGui::Text("SUMMON"); ImGui::NextColumn();
			ImGui::Text("POSITION"); ImGui::NextColumn();
			ImGui::Text("SIZE"); ImGui::NextColumn();
			ImGui::Text("COUNT"); ImGui::NextColumn();
			ImGui::Separator();

			int imgui_num = 0;
			//BOXpyramid
			{
				static int BOX_pyramid_count = 5;
				static float BOX_pyramid_pos[3] = { 0 };
				static float BOX_pyramid_size[3] = { 1,1,1 };
				bool summon = false;
				ImGui::Separator();
				ImGui::Text("BOX_pyramid"); ImGui::NextColumn();
				ImGui::Checkbox(std::to_string(imgui_num + 100).c_str(), &summon); ImGui::NextColumn();
				ImGui::DragFloat3(std::to_string(imgui_num + 200).c_str(), BOX_pyramid_pos, 0.1f); ImGui::NextColumn();
				ImGui::DragFloat3(std::to_string(imgui_num + 250).c_str(), BOX_pyramid_size, 0.1f); ImGui::NextColumn();
				ImGui::DragInt(std::to_string(imgui_num + 300).c_str(), &BOX_pyramid_count, 1, 1, 100000); ImGui::NextColumn();

				if (summon == true) {
					Gameobject* pearent = Gameobject_manager::create("BOXpyramid");
					all_pearent->add_child(pearent);
					for (int i = 0; i < BOX_pyramid_count; i++) {
						for (int o = 0; o < BOX_pyramid_count - i; o++) {
							set_box(Vector3(
								BOX_pyramid_size[0] * 2.0f * 1.2f * o - ((BOX_pyramid_count - i) * 2.300001f / 2.0f + BOX_pyramid_pos[0]) * BOX_pyramid_size[0],
								3.0f + BOX_pyramid_size[1] * 2 * i + BOX_pyramid_pos[1],
								BOX_pyramid_pos[2]),
								Vector3(BOX_pyramid_size[0], BOX_pyramid_size[1], BOX_pyramid_size[2]),
								Vector3(0, 10, 0),
								Vector3(0, 1, 1),
								pearent,
								false
							);
						}

					}
				}
				imgui_num++;
			}

			//SPHEREpyramid
			{
				static int SPHERE_pyramid_count = 5;
				static float SPHERE_pyramid_pos[3] = { 0 };
				static float size = 1;
				bool summon = false;
				ImGui::Separator();
				ImGui::Text("SPHERE_pyramid"); ImGui::NextColumn();
				ImGui::Checkbox(std::to_string(imgui_num + 100).c_str(), &summon); ImGui::NextColumn();
				ImGui::DragFloat3(std::to_string(imgui_num + 200).c_str(), SPHERE_pyramid_pos, 0.1f); ImGui::NextColumn();
				ImGui::DragFloat(std::to_string(imgui_num + 220).c_str(), &size, 0.1f, 0.01f, 100000); ImGui::NextColumn();
				ImGui::DragInt(std::to_string(imgui_num + 300).c_str(), &SPHERE_pyramid_count, 1, 1, 100000); ImGui::NextColumn();

				if (summon == true) {
					Gameobject* pearent = Gameobject_manager::create("Spherepyramid");
					all_pearent->add_child(pearent);
					for (int i = 0; i < SPHERE_pyramid_count; i++) {
						for (int o = 0; o < SPHERE_pyramid_count - i; o++) {
							set_sphere(
								Vector3(2.50001f * o - (SPHERE_pyramid_count - i) * 2.500001f / 2.0f + SPHERE_pyramid_pos[0],
									5.0f + 2.50001f * i + SPHERE_pyramid_pos[1],
									SPHERE_pyramid_pos[2]),
								size,
								Vector3(0, 1, 1),
								pearent,
								false
							);
						}

					}
				}
				imgui_num++;
			}

			//CAPSULEpyramid
			{
				static int CAPSULE_pyramid_count = 5;
				static float CAPSULE_pyramid_pos[3] = { 0 };
				bool summon = false;
				ImGui::Separator();
				ImGui::Text("CAPSULE_pyramid"); ImGui::NextColumn();
				ImGui::Checkbox(std::to_string(imgui_num + 100).c_str(), &summon); ImGui::NextColumn();
				ImGui::DragFloat3(std::to_string(imgui_num + 200).c_str(), CAPSULE_pyramid_pos, 0.1f); ImGui::NextColumn(); ImGui::NextColumn();
				ImGui::DragInt(std::to_string(imgui_num + 300).c_str(), &CAPSULE_pyramid_count, 1, 1, 100000); ImGui::NextColumn();

				if (summon == true)
					for (int i = 0; i < CAPSULE_pyramid_count; i++) {
						for (int o = 0; o < CAPSULE_pyramid_count - i; o++) {
							set_capsule(
								Vector3(2.50001f * o - (CAPSULE_pyramid_count - i) * 2.500001f / 2.0f + CAPSULE_pyramid_pos[0], 5.0f + 4.50001f * i + CAPSULE_pyramid_pos[1], CAPSULE_pyramid_pos[2]),
								1, 1, Vector3(0, 1, 1), Vector3(0, 1, 1), all_pearent, false);
						}

					}
				imgui_num++;
			}

			//deskpyramid
			{
				static int count = 5;
				static float pos[3] = { 0 };
				bool summon = false;
				ImGui::Separator();
				ImGui::Text("Daruma_pyramid"); ImGui::NextColumn();
				ImGui::Checkbox(std::to_string(imgui_num + 100).c_str(), &summon); ImGui::NextColumn();
				ImGui::DragFloat3(std::to_string(imgui_num + 200).c_str(), pos, 0.1f); ImGui::NextColumn(); ImGui::NextColumn();
				ImGui::DragInt(std::to_string(imgui_num + 300).c_str(), &count, 1, 1, 100000); ImGui::NextColumn();

				float dis = 1.0f;
				if (summon == true) {
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

							all_pearent->add_child(Daruma);
							//GOs.emplace_back(Daruma);
							//GOs.emplace_back(parts[0]);
							//GOs.emplace_back(parts[1]);
							//GOs.emplace_back(parts[2]);
							//GOs.emplace_back(parts[3]);
							//GOs.emplace_back(parts[4]);
						}

					}
				}
				imgui_num++;
			}

			{}

			//treepyramid
			{

				static int TREE_pyramid_count = 5;
				static float TREE_pyramid_pos[3] = { 0 };
				bool summon = false;
				ImGui::Separator();
				ImGui::Text("TREE_pyramid"); ImGui::NextColumn();
				ImGui::Checkbox(std::to_string(imgui_num + 100).c_str(), &summon); ImGui::NextColumn();
				ImGui::DragFloat3(std::to_string(imgui_num + 200).c_str(), TREE_pyramid_pos, 0.1f); ImGui::NextColumn();ImGui::NextColumn();
				ImGui::DragInt(std::to_string(imgui_num + 300).c_str(), &TREE_pyramid_count, 1, 1, 100000); ImGui::NextColumn();

				if (summon == true) {
					Gameobject* pearent = Gameobject_manager::create("Treepyramid");
					all_pearent->add_child(pearent);
					for (int i = 0; i < TREE_pyramid_count; i++) {
						for (int o = 0; o < TREE_pyramid_count - i; o++) {
							set_tree(
								Vector3(10.0f * o - (TREE_pyramid_count - i) * 10.0f / 2.0f + TREE_pyramid_pos[0],
									TREE_pyramid_pos[1] + i * 13.0f,
									TREE_pyramid_pos[2]),
								Vector3(1),
								Vector3(0),
								1, 1,
								pearent
							);
						}

					}
				}
				imgui_num++;
			}


			//gear
			{
				static int TREE_pyramid_count = 11;
				static float TREE_pyramid_pos[3] = { 0 };
				bool summon = false;
				ImGui::Separator();
				ImGui::Text("GEAR"); ImGui::NextColumn();
				ImGui::Checkbox(std::to_string(imgui_num + 100).c_str(), &summon); ImGui::NextColumn();
				ImGui::DragFloat3(std::to_string(imgui_num + 200).c_str(), TREE_pyramid_pos, 0.1f); ImGui::NextColumn(); ImGui::NextColumn();
				ImGui::DragInt(std::to_string(imgui_num + 300).c_str(), &TREE_pyramid_count, 1, 1, 100000); ImGui::NextColumn();

				if (summon == true) {
					const Vector3 pos = Vector3(TREE_pyramid_pos[0], TREE_pyramid_pos[1] + 8, TREE_pyramid_pos[2]);
					const Vector3 size = Vector3(0.5f, 2, 1);


					Gameobject* GEAR = Gameobject_manager::create("GEAR");
					all_pearent->add_child(GEAR);

					GEAR->transform->local_pos = pos;

					Collider* coll = GEAR->addComponent<Collider>();
					coll->tag |= Collider_tags::Stage;
					//coll->set_tensor(make_box_tensor(Vector3(5, 5, 50), 1));

					for (int gear_tooth_num = 0; gear_tooth_num < TREE_pyramid_count; gear_tooth_num++) {
						Gameobject* GO = Gameobject_manager::createCube("tooth");
						GO->transform->local_scale = size;
						GO->transform->local_orient = quaternion_from_euler(0, 0, 360.0f / TREE_pyramid_count * gear_tooth_num);
						GO->transform->local_pos = Vector3(0, size.y * 2, 0);
						GO->transform->local_pos = vector3_quatrotate(GO->transform->local_pos, quaternion_axis_angle(Vector3(0, 0, 1), 360.0f / TREE_pyramid_count * gear_tooth_num));

						GO->renderer->color = Vector4(1, 1, 0, 1);
						GEAR->add_child(GO);
					}

					for (int gear_tooth_num = 0; gear_tooth_num < TREE_pyramid_count; gear_tooth_num++) {
						Box* box = coll->add_shape<Box>();
						box->size = size;
						box->rotate = Vector3(0, 0, 360.0f / TREE_pyramid_count * gear_tooth_num);
						box->center = Vector3(0, size.y * 2, 0);
						box->center = vector3_quatrotate(box->center, quaternion_axis_angle(Vector3(0, 0, 1), 360.0f / TREE_pyramid_count * gear_tooth_num));

					}


					Gameobject* gear_joint = Gameobject_manager::createCube("gear_joint");
					all_pearent->add_child(gear_joint);
					gear_joint->transform->local_pos = pos;
					Collider* gear_joint_collider = gear_joint->addComponent<Collider>();
					gear_joint_collider->physics_data.is_moveable = false;

					Joint::add_Hingejoint(gear_joint_collider, coll, Vector3(0, 0, +10), Vector3(0, 0, -10), Vector3(0, 0, +10), Vector3(0, 0, -10), 1, 0.1f);

				}
				imgui_num++;
			}

			//patapata
			{

				static int TREE_pyramid_count = 3;
				static float TREE_pyramid_pos[3] = { 0 };
				bool summon = false;
				ImGui::Separator();
				ImGui::Text("patapata"); ImGui::NextColumn();
				ImGui::Checkbox(std::to_string(imgui_num + 100).c_str(), &summon); ImGui::NextColumn();
				ImGui::DragFloat3(std::to_string(imgui_num + 200).c_str(), TREE_pyramid_pos, 0.1f); ImGui::NextColumn(); ImGui::NextColumn();
				ImGui::DragInt(std::to_string(imgui_num + 300).c_str(), &TREE_pyramid_count, 1, 1, 100000); ImGui::NextColumn();

				if (summon == true) {
					const Vector3 pos = Vector3(TREE_pyramid_pos[0], TREE_pyramid_pos[1] + 8, TREE_pyramid_pos[2]);
					const Vector3 size = Vector3(6, 3, 1);


					Gameobject* PATAPATA = Gameobject_manager::create("PATAPATA");
					PATAPATA->transform->local_pos = pos;

					Collider* coll = PATAPATA->addComponent<Collider>();
					//coll->set_tensor(make_box_tensor(Vector3(5, 5, 50), 1));

					for (int gear_tooth_num = 0; gear_tooth_num < TREE_pyramid_count; gear_tooth_num++) {
						Gameobject* GO = Gameobject_manager::createCube("tooth");
						GO->transform->local_scale = size;
						GO->transform->local_orient = quaternion_from_euler(0, 360.0f / TREE_pyramid_count * gear_tooth_num, 0);
						GO->transform->local_pos = Vector3(0, size.y * 0, 0);
						GO->transform->local_pos = vector3_quatrotate(GO->transform->local_pos, quaternion_axis_angle(Vector3(0, 1, 0), 360.0f / TREE_pyramid_count * gear_tooth_num));

						GO->renderer->color = Vector4(1, 1, 0, 1);
						PATAPATA->add_child(GO);
					}

					for (int gear_tooth_num = 0; gear_tooth_num < TREE_pyramid_count; gear_tooth_num++) {
						Box* box = coll->add_shape<Box>();
						box->size = size;
						box->rotate = Vector3(0, 360.0f / TREE_pyramid_count * gear_tooth_num, 0);
						box->center = Vector3(0, size.y * 0, 0);
						box->center = vector3_quatrotate(box->center, quaternion_axis_angle(Vector3(0, 1, 0), 360.0f / TREE_pyramid_count * gear_tooth_num));

					}


					Gameobject* gear_joint = Gameobject_manager::createCube("gear_joint");
					gear_joint->transform->local_pos = pos;
					Collider* gear_joint_collider = gear_joint->addComponent<Collider>();
					gear_joint_collider->physics_data.is_moveable = false;

					//auto hinge = Joint::add_Hingejoint(gear_joint_collider, coll, Vector3(0, +10, 0), Vector3(0, -10, 0), Vector3(0, +10, 0), Vector3(0, -10, 0), 2, 0.1f);
					auto hinge = Joint::add_Hingejoint(
						gear_joint_collider,
						coll,
						Vector3(0, 10, 0).unit_vect() * 10, Vector3(0, -10, 0).unit_vect() * 10,
						Vector3(0, +10, 0), Vector3(0, -10, 0),
						2, 0.1f
					);
					//auto hinge = Joint::add_Hingejoint(
					//	gear_joint_collider, coll,
					//	Vector3(0, -10, 0), Vector3(0, +10, 0),
					//	Vector3(0, +10, 0), Vector3(0, -10, 0),
					//	2, 0.1f
					//);
					hinge->limit = Vector2(0, 90);

				}
				imgui_num++;
			}

			//RagDoll
			{

				static int TREE_pyramid_count = 3;
				static float pos[3] = { 0 };
				bool summon = false;
				ImGui::Separator();
				ImGui::Text("RagDoll"); ImGui::NextColumn();
				ImGui::Checkbox(std::to_string(imgui_num + 100).c_str(), &summon); ImGui::NextColumn();
				ImGui::DragFloat3(std::to_string(imgui_num + 200).c_str(), pos, 0.1f); ImGui::NextColumn(); ImGui::NextColumn();
				ImGui::DragInt(std::to_string(imgui_num + 300).c_str(), &TREE_pyramid_count, 1, 1, 100000); ImGui::NextColumn();

				if (summon == true) {
					Gameobject* Human = Gameobject_manager::create("Human");
					all_pearent->add_child(Human);
					Human->transform->local_pos = Vector3(pos[0], pos[1] + 8, pos[2]);

					//::: Gameobjectの生成 :::
					Gameobject* Head = Gameobject_manager::createCube("Head");

					Gameobject* Rsholder = Gameobject_manager::createCube("Rsholder");
					Gameobject* Relbow = Gameobject_manager::createCube("Relbow");
					Gameobject* Lsholder = Gameobject_manager::createCube("Lsholder");
					Gameobject* Lelbow = Gameobject_manager::createCube("Lelbow");

					Gameobject* Body = Gameobject_manager::createCube("Body");

					Gameobject* Waist = Gameobject_manager::createCube("Waist");

					Gameobject* Rleg = Gameobject_manager::createCube("Rleg");
					Gameobject* Rfoot = Gameobject_manager::createCube("Rfoot");
					Gameobject* Lleg = Gameobject_manager::createCube("Lleg");
					Gameobject* Lfoot = Gameobject_manager::createCube("Lfoot");

					Vector4 face_color = Vector4(0.5f, 0.5f, 0.5f, 1);
					{
						Gameobject* eye0 = Gameobject_manager::createSphere("eye0");
						Head->add_child(eye0);
						eye0->transform->local_pos = Vector3(+0.5f, 0.5f, -1);
						eye0->transform->local_scale = Vector3(0.25f, 0.25f, 0.25f);
						eye0->renderer->color = face_color;
					}
					{
						Gameobject* eye1 = Gameobject_manager::createSphere("eye1");
						Head->add_child(eye1);
						eye1->transform->local_pos = Vector3(-0.5f, 0.5f, -1);
						eye1->transform->local_scale = Vector3(0.25f, 0.25f, 0.25f);
						eye1->renderer->color = face_color;
					}
					{
						Gameobject* mouth = Gameobject_manager::createCube("mouth");
						Head->add_child(mouth);
						mouth->transform->local_pos = Vector3(0, -0.45f, -1);
						mouth->transform->local_scale = Vector3(0.7f, 0.25f, 0.3f);
						mouth->renderer->color = face_color;
					}

					{
						Gameobject* belt = Gameobject_manager::createCube("belt");
						Waist->add_child(belt);
						belt->transform->local_pos = Vector3(0, -0.45f, 0);
						belt->transform->local_scale = Vector3(1.1f, 0.25f, 1.1f);
						belt->renderer->color = face_color;
					}


					//::: 親子関係の設定 :::
					Human->add_child(Body);

					Human->add_child(Head);

					Human->add_child(Rsholder);
					Human->add_child(Relbow);
					Human->add_child(Lsholder);
					Human->add_child(Lelbow);

					Human->add_child(Waist);

					Human->add_child(Rleg);
					Human->add_child(Rfoot);
					Human->add_child(Lleg);
					Human->add_child(Lfoot);

					//::: gameobjectの座標設定 :::
					Vector3 head_size = Vector3(1, 1, 1);
					Vector3 body_size = Vector3(0.8f, 0.6f, 0.8f);
					Vector3 Waist_size = Vector3(0.81f, 0.7f, 0.81f);
					{
						{
							Head->transform->local_scale = head_size;
							Head->transform->local_pos = Vector3(0, body_size.y + head_size.y + 0.2f, 0);
						}
					}

					{
						{
							auto& GO = Body;
							GO->transform->local_scale = body_size;
							GO->transform->local_orient = quaternion_from_euler(0, 0, 0);
							GO->transform->local_pos = Vector3(0, 0, 0);
						}
						{
							auto& GO = Waist;
							GO->transform->local_scale = Waist_size;
							GO->transform->local_pos = Vector3(0, -0.7f * 2, 0);
						}
					}

					Vector3 arm_size = Vector3(0.4f, 0.5f, 0.4f);
					float arm_y_pos = 0.18f;
					{
						{
							auto& GO = Rsholder;
							GO->transform->local_scale = arm_size;
							GO->transform->local_orient = quaternion_from_euler(0, 0, -90);
							GO->transform->local_pos = Vector3(-(body_size.x + arm_size.x * 2), arm_y_pos, 0);
						}
						{
							auto& GO = Relbow;
							GO->transform->local_scale = arm_size;
							GO->transform->local_orient = quaternion_from_euler(0, 0, -90);
							GO->transform->local_pos = Vector3(-(body_size.x + arm_size.x * 5), arm_y_pos, 0);
						}
						{
							auto& GO = Lsholder;
							GO->transform->local_scale = arm_size;
							GO->transform->local_orient = quaternion_from_euler(0, 0, +90);
							GO->transform->local_pos = Vector3(+(body_size.x + arm_size.x * 2), arm_y_pos, 0);
						}
						{
							auto& GO = Lelbow;
							GO->transform->local_scale = arm_size;
							GO->transform->local_orient = quaternion_from_euler(0, 0, +90);
							GO->transform->local_pos = Vector3(+(body_size.x + arm_size.x * 5), arm_y_pos, 0);
						}
					}

					Vector3 Foot_size = Vector3(0.4f, 0.25, 0.4f);
					float leg_x_pos = 0.6f;
					{
						{
							auto& GO = Rleg;
							GO->transform->local_scale = Foot_size;
							GO->transform->local_orient = quaternion_from_euler(0, 0, 0);
							GO->transform->local_pos = Vector3(-leg_x_pos, -(body_size.y + Waist_size.y * 2 + Foot_size.y * 2), 0);
						}
						{
							auto& GO = Rfoot;
							GO->transform->local_scale = Foot_size;
							GO->transform->local_orient = quaternion_from_euler(0, 0, 0);
							GO->transform->local_pos = Vector3(-leg_x_pos, -(body_size.y + Waist_size.y * 2 + Foot_size.y * 5), 0);
						}
						{
							auto& GO = Lleg;
							GO->transform->local_scale = Foot_size;
							GO->transform->local_orient = quaternion_from_euler(0, 0, 0);
							GO->transform->local_pos = Vector3(+leg_x_pos, -(body_size.y + Waist_size.y * 2 + Foot_size.y * 2), 0);
						}
						{
							auto& GO = Lfoot;
							GO->transform->local_scale = Foot_size;
							GO->transform->local_orient = quaternion_from_euler(0, 0, 0);
							GO->transform->local_pos = Vector3(+leg_x_pos, -(body_size.y + Waist_size.y * 2 + Foot_size.y * 5), 0);
						}
					}

					//::: collider,shapeのアタッチ :::
					Collider* Head_collider = Head->addComponent<Collider>();
					Collider* Rsholder_collider = Rsholder->addComponent<Collider>();
					Collider* Relbow_collider = Relbow->addComponent<Collider>();
					Collider* Lsholder_collider = Lsholder->addComponent<Collider>();
					Collider* Lelbow_collider = Lelbow->addComponent<Collider>();
					Collider* Body_collider = Body->addComponent<Collider>();
					Collider* Waist_collider = Waist->addComponent<Collider>();
					Collider* Rleg_collider = Rleg->addComponent<Collider>();
					Collider* Rfoot_collider = Rfoot->addComponent<Collider>();
					Collider* Lleg_collider = Lleg->addComponent<Collider>();
					Collider* Lfoot_collider = Lfoot->addComponent<Collider>();


					Sphere* Head_shape = Head_collider->add_shape<Sphere>();
					Capsule* Rsholder_shape = Rsholder_collider->add_shape<Capsule>();
					Capsule* Relbow_shape = Relbow_collider->add_shape<Capsule>();
					Capsule* Lsholder_shape = Lsholder_collider->add_shape<Capsule>();
					Capsule* Lelbow_shape = Lelbow_collider->add_shape<Capsule>();
					Box* Body_shape = Body_collider->add_shape<Box>();
					Box* Waist_shape = Waist_collider->add_shape<Box>();
					Sphere* Rleg_shape = Rleg_collider->add_shape<Sphere>();
					Sphere* Rfoot_shape = Rfoot_collider->add_shape<Sphere>();
					Sphere* Lleg_shape = Lleg_collider->add_shape<Sphere>();
					Sphere* Lfoot_shape = Lfoot_collider->add_shape<Sphere>();

					//::: 質量の調整 :::
					Head_collider->physics_data.inertial_mass = 3;
					Rsholder_collider->physics_data.inertial_mass = 2;
					Relbow_collider->physics_data.inertial_mass = 1;
					Lsholder_collider->physics_data.inertial_mass = 2;
					Lelbow_collider->physics_data.inertial_mass = 1;
					Body_collider->physics_data.inertial_mass = 5;
					Waist_collider->physics_data.inertial_mass = 4;
					Rleg_collider->physics_data.inertial_mass = 2;
					Rfoot_collider->physics_data.inertial_mass = 1;
					Lleg_collider->physics_data.inertial_mass = 2;
					Lfoot_collider->physics_data.inertial_mass = 1;


					//::: capsuleの調整 :::
					Relbow_shape->length *= (Relbow->transform->local_scale.y - Relbow->transform->local_scale.x) / (Relbow->transform->local_scale.y);
					Rsholder_shape->length *= (Rsholder->transform->local_scale.y - Rsholder->transform->local_scale.x) / (Rsholder->transform->local_scale.y);
					Lelbow_shape->length *= (Lelbow->transform->local_scale.y - Lelbow->transform->local_scale.x) / (Lelbow->transform->local_scale.y);
					Lsholder_shape->length *= (Lsholder->transform->local_scale.y - Lsholder->transform->local_scale.x) / (Lsholder->transform->local_scale.y);

					//Body_shape->length *= (Body->transform->local_scale.y - Body->transform->local_scale.x) / (Body->transform->local_scale.y);

					//Rleg_shape->length *= Rleg->transform->local_scale.y / (Rleg->transform->local_scale.x + Rleg->transform->local_scale.y);
					//Rfoot_shape->length *= Rfoot->transform->local_scale.y / (Rfoot->transform->local_scale.x + Rfoot->transform->local_scale.y);
					//Lleg_shape->length *= Lleg->transform->local_scale.y / (Lleg->transform->local_scale.x + Lleg->transform->local_scale.y);
					//Lfoot_shape->length *= Lfoot->transform->local_scale.y / (Lfoot->transform->local_scale.x + Lfoot->transform->local_scale.y);

					//::: sphereの調整 :::
					Rleg_shape->r = Rleg->transform->local_scale.y * 2;
					Rfoot_shape->r = Rfoot->transform->local_scale.y * 2;
					Lleg_shape->r = Lleg->transform->local_scale.y * 2;
					Lfoot_shape->r = Lfoot->transform->local_scale.y * 2;




					//::: Jointの設定

					{
						auto Ball = Joint::add_Conejoint(Head_collider, Body_collider, Vector3(0, -1, 0), Vector3(0, 0.8f, 0), Vector3(0, 1, 0), Vector3(0, 1, 0));
						Ball->limit = 30;

						auto Twist = Joint::add_Twistjoint(Body_collider, Head_collider, Vector3(0, 1, 0), Vector3(0, 1, 0));
						Twist->limit = Vector2(360 - 90, 90);
					}
					{
						auto Cone = Joint::add_Conejoint(Body_collider, Rsholder_collider, Vector3(-1.1f, 0.2f, 0), Vector3(0, 0.5f, 0), Vector3(-1, 0, -1).unit_vect(), Vector3(0, -1, 0).unit_vect());
						Cone->limit = 120;

						auto Twist = Joint::add_Twistjoint(Body_collider, Rsholder_collider, Vector3(0, 1, 0), Vector3(0, 1, 0));
						Twist->limit = Vector2(360 - 90, 90);
					}
					{
						auto hinge = Joint::add_Hingejoint(Rsholder_collider, Relbow_collider, Vector3(-1, -0.6f, 0), Vector3(+1, -0.6f, 0), Vector3(-1, 0.6f, 0), Vector3(+1, 0.6f, 0));
						hinge->limit = Vector2(230, 360);
					}

					{
						auto Cone = Joint::add_Conejoint(Body_collider, Lsholder_collider, Vector3(+1.1f, 0.2f, 0), Vector3(0, 0.5f, 0), Vector3(-1, 0, -1).unit_vect(), Vector3(0, -1, 0).unit_vect());
						Cone->limit = 120;

						auto Twist = Joint::add_Twistjoint(Body_collider, Lsholder_collider, Vector3(0, 1, 0), Vector3(0, 1, 0));
						Twist->limit = Vector2(360 - 90, 90);
					}
					{
						auto hinge = Joint::add_Hingejoint(Lsholder_collider, Lelbow_collider, Vector3(-1, -0.6f, 0), Vector3(+1, -0.6f, 0), Vector3(-1, 0.6f, 0), Vector3(+1, 0.6f, 0));
						hinge->limit = Vector2(230, 360);
					}

					{
						auto Cone = Joint::add_Conejoint(Body_collider, Waist_collider, Vector3(0, -1.0f, 0), Vector3(0, 0.5f, 0), Vector3(0, 1, 0).unit_vect(), Vector3(0, 1.5f, -1).unit_vect());
						Cone->limit = 40;

						auto Twist = Joint::add_Twistjoint(Body_collider, Waist_collider, Vector3(0, 1, 0), Vector3(0, 1, 0));
						Twist->limit = Vector2(360 - 30, 30);
					}

					{
						auto Cone = Joint::add_Conejoint(Waist_collider, Rleg_collider, Vector3(-0.6f, -0.8f, 0), Vector3(0, 0.3f, 0), Vector3(0, -1, -1.02f).unit_vect(), Vector3(0, -1, 0).unit_vect());
						Cone->limit = 48;

						auto Twist = Joint::add_Twistjoint(Waist_collider, Rleg_collider, Vector3(0, 1, 0), Vector3(0, 1, 0));
						Twist->limit = Vector2(360 - 5, 15);
					}
					{
						auto hinge = Joint::add_Hingejoint(Rleg_collider, Rfoot_collider, Vector3(-1, -0.4f, 0), Vector3(+1, -0.3f, 0), Vector3(-1, +0.3f, 0), Vector3(+1, +0.3f, 0));
						hinge->limit = Vector2(0, 60);
					}
					{
						auto Cone = Joint::add_Conejoint(Waist_collider, Lleg_collider, Vector3(+0.6f, -0.8f, 0), Vector3(0, 0.3f, 0), Vector3(0, -1, -1.02f).unit_vect(), Vector3(0, -1, 0).unit_vect());
						Cone->limit = 48;

						auto Twist = Joint::add_Twistjoint(Waist_collider, Lleg_collider, Vector3(0, 1, 0), Vector3(0, 1, 0));
						Twist->limit = Vector2(360 - 15, 5);
					}
					{
						auto hinge = Joint::add_Hingejoint(Lleg_collider, Lfoot_collider, Vector3(-1, -0.4f, 0), Vector3(+1, -0.3f, 0), Vector3(-1, +0.3f, 0), Vector3(+1, +0.3f, 0));
						hinge->limit = Vector2(0, 60);
					}



				}
				imgui_num++;

			}

			//rope
			{
				static int TREE_pyramid_count = 10;
				static float TREE_pyramid_sphere_radius = 0.5f;
				static float pos[3] = { 0 };
				bool summon = false;
				ImGui::Separator();
				ImGui::Text("Rope"); ImGui::NextColumn();
				ImGui::Checkbox(std::to_string(imgui_num + 100).c_str(), &summon); ImGui::NextColumn();
				ImGui::DragFloat3(std::to_string(imgui_num + 200).c_str(), pos, 0.1f); ImGui::NextColumn();
				ImGui::DragFloat(std::to_string(imgui_num + 250).c_str(), &TREE_pyramid_sphere_radius, 0.01f, 0.01f, 100000); ImGui::NextColumn();
				ImGui::DragInt(std::to_string(imgui_num + 300).c_str(), &TREE_pyramid_count, 1, 1, 100000); ImGui::NextColumn();

				if (summon == true) {
					auto go = Gameobject_manager::create("rope");
					go->transform->local_pos = Vector3(pos[0], pos[1], pos[2]);

					auto coll = go->addComponent<Collider_Rope>();
					coll->sphere_num_size = TREE_pyramid_count;
					coll->sphere_size_r = TREE_pyramid_sphere_radius;
					coll->sphree_offset_size = TREE_pyramid_sphere_radius * 2;
					coll->create_rope();

					Physics_data data;
					data = coll->get_vertex_data(0);
					data.is_moveable = false;
					coll->set_vertex_data(0, data);

					auto renderer = go->addComponent<Rope_renderer>();
					renderer->radius = TREE_pyramid_sphere_radius;
					renderer->split_count = 30;
					renderer->set_meshoffset(coll->get_vertex_offset());

					all_pearent->add_child(go);
				}
				imgui_num++;
			}

			//croth sphere
			{
				static int TREE_pyramid_count = 3;
				static float pos[3] = { 0 };
				bool summon = false;
				ImGui::Separator();
				ImGui::Text("Croth_Sphere"); ImGui::NextColumn();
				ImGui::Checkbox(std::to_string(imgui_num + 100).c_str(), &summon); ImGui::NextColumn();
				ImGui::DragFloat3(std::to_string(imgui_num + 200).c_str(), pos, 0.1f); ImGui::NextColumn(); ImGui::NextColumn();
				ImGui::DragInt(std::to_string(imgui_num + 300).c_str(), &TREE_pyramid_count, 1, 1, 100000); ImGui::NextColumn();

				if (summon == true) {
					auto sphere = Gameobject_manager::createSphere();
					sphere->transform->local_pos = Vector3(pos[0], pos[1], pos[2]);

					auto coll = sphere->addComponent<Collider_Croth>();
					coll->load_file("./DefaultModel/sphere.fbx", true, false);

					auto renderer = sphere->addComponent<Croth_renderer>();
					renderer->set_meshoffset(coll->get_vertex_offset());

					all_pearent->add_child(sphere);
				}
				imgui_num++;
			}

			//croth box
			{
				static int TREE_pyramid_count = 3;
				static float pos[3] = { 0 };
				bool summon = false;
				ImGui::Separator();
				ImGui::Text("Croth_Vox"); ImGui::NextColumn();
				ImGui::Checkbox(std::to_string(imgui_num + 100).c_str(), &summon); ImGui::NextColumn();
				ImGui::DragFloat3(std::to_string(imgui_num + 200).c_str(), pos, 0.1f); ImGui::NextColumn(); ImGui::NextColumn();
				ImGui::DragInt(std::to_string(imgui_num + 300).c_str(), &TREE_pyramid_count, 1, 1, 100000); ImGui::NextColumn();

				if (summon == true) {
					auto sphere = Gameobject_manager::createCube();
					sphere->transform->local_pos = Vector3(pos[0], pos[1], pos[2]);

					auto coll = sphere->addComponent<Collider_Croth>();
					coll->load_file("./DefaultModel/cube.fbx", true, false);

					auto renderer = sphere->addComponent<Croth_renderer>();
					renderer->set_meshoffset(coll->get_vertex_offset());

					all_pearent->add_child(sphere);
				}
				imgui_num++;
			}

			//croth plane2
			{
				static int TREE_pyramid_count = 3;
				static float pos[3] = { 0 };
				bool summon = false;
				ImGui::Separator();
				ImGui::Text("Croth_plane02x"); ImGui::NextColumn();
				ImGui::Checkbox(std::to_string(imgui_num + 100).c_str(), &summon); ImGui::NextColumn();
				ImGui::DragFloat3(std::to_string(imgui_num + 200).c_str(), pos, 0.1f); ImGui::NextColumn(); ImGui::NextColumn();
				ImGui::DragInt(std::to_string(imgui_num + 300).c_str(), &TREE_pyramid_count, 1, 1, 100000); ImGui::NextColumn();

				if (summon == true) {
					auto go = Gameobject_manager::createFromFBX("plane", "./DefaultModel/plane_02x02vertex.fbx");
					go->transform->local_pos = Vector3(pos[0], pos[1], pos[2]);
					//go->transform->local_scale = Vector3(10, 10, 10);

					auto coll = go->addComponent<Collider_Croth>();
					coll->load_file("./DefaultModel/plane_02x02vertex.fbx", true, false);

					Physics_data data;
					data = coll->get_vertex_data(0, 8);
					data.is_moveable = false;
					coll->set_vertex_data(0, 8, data);
					data = coll->get_vertex_data(0, 7);
					data.is_moveable = false;
					coll->set_vertex_data(0, 7, data);


					auto renderer = go->addComponent<Croth_renderer>();
					renderer->set_meshoffset(coll->get_vertex_offset());

					all_pearent->add_child(go);
				}
				imgui_num++;
			}

			//croth plane16
			{
				static int TREE_pyramid_count = 3;
				static float pos[3] = { 0 };
				static float scale[3] = { 1,1,1 };
				bool summon = false;
				ImGui::Separator();
				ImGui::Text("Croth_plane16x"); ImGui::NextColumn();
				ImGui::Checkbox(std::to_string(imgui_num + 100).c_str(), &summon); ImGui::NextColumn();
				ImGui::DragFloat3(std::to_string(imgui_num + 200).c_str(), pos, 0.1f); ImGui::NextColumn();
				ImGui::DragFloat3(std::to_string(imgui_num + 250).c_str(), scale, 0.1f); ImGui::NextColumn();
				ImGui::DragInt(std::to_string(imgui_num + 300).c_str(), &TREE_pyramid_count, 1, 1, 100000); ImGui::NextColumn();

				if (summon == true) {
					auto go = Gameobject_manager::createFromFBX("plane", "./DefaultModel/plane_16x16vertex.fbx");
					go->transform->local_pos = Vector3(pos[0], pos[1], pos[2]);
					go->transform->local_scale = Vector3(scale[0], scale[1], scale[2]);

					auto coll = go->addComponent<Collider_Croth>();
					//coll->mesh_scale = Vector3(scale[0], scale[1], scale[2]);
					coll->load_file("./DefaultModel/plane_16x16vertex.fbx", true, false);

					Physics_data data;
					data = coll->get_vertex_data(0, 259);
					data.is_moveable = false;
					coll->set_vertex_data(0, 259, data);
					data = coll->get_vertex_data(0, 274);
					data.is_moveable = false;
					coll->set_vertex_data(0, 274, data);

					for (int i = 259; i < 275; ++i) {
						data = coll->get_vertex_data(0, i);
						data.is_moveable = false;
						coll->set_vertex_data(0, i, data);
					}


					auto renderer = go->addComponent<Croth_renderer>();
					renderer->set_meshoffset(coll->get_vertex_offset());

					all_pearent->add_child(go);
				}
				imgui_num++;
			}

			//croth plane32
			{
				static int TREE_pyramid_count = 3;
				static float pos[3] = { 0 };
				bool summon = false;
				ImGui::Separator();
				ImGui::Text("Croth_plane32x"); ImGui::NextColumn();
				ImGui::Checkbox(std::to_string(imgui_num + 100).c_str(), &summon); ImGui::NextColumn();
				ImGui::DragFloat3(std::to_string(imgui_num + 200).c_str(), pos, 0.1f); ImGui::NextColumn(); ImGui::NextColumn();
				ImGui::DragInt(std::to_string(imgui_num + 300).c_str(), &TREE_pyramid_count, 1, 1, 100000); ImGui::NextColumn();

				if (summon == true) {
					auto go = Gameobject_manager::createFromFBX("plane", "./DefaultModel/plane_32x32vertex.fbx");
					go->transform->local_pos = Vector3(pos[0], pos[1], pos[2]);

					auto coll = go->addComponent<Collider_Croth>();
					coll->load_file("./DefaultModel/plane_32x32vertex.fbx", true, false);

					Physics_data data;
					data = coll->get_vertex_data(0, 1027);
					data.is_moveable = false;
					coll->set_vertex_data(0, 1027, data);
					data = coll->get_vertex_data(0, 1058);
					data.is_moveable = false;
					coll->set_vertex_data(0, 1058, data);

					for (int i = 1026; i < 1059; ++i) {
						data = coll->get_vertex_data(0, i);
						data.is_moveable = false;
						coll->set_vertex_data(0, i, data);
					}


					auto renderer = go->addComponent<Croth_renderer>();
					renderer->set_meshoffset(coll->get_vertex_offset());

					all_pearent->add_child(go);
				}
				imgui_num++;
			}

			//croth plane64
			{
				static int TREE_pyramid_count = 3;
				static float pos[3] = { 0 };
				bool summon = false;
				ImGui::Separator();
				ImGui::Text("Croth_plane64x"); ImGui::NextColumn();
				ImGui::Checkbox(std::to_string(imgui_num + 100).c_str(), &summon); ImGui::NextColumn();
				ImGui::DragFloat3(std::to_string(imgui_num + 200).c_str(), pos, 0.1f); ImGui::NextColumn(); ImGui::NextColumn();
				ImGui::DragInt(std::to_string(imgui_num + 300).c_str(), &TREE_pyramid_count, 1, 1, 100000); ImGui::NextColumn();

				if (summon == true) {
					auto go = Gameobject_manager::createFromFBX("plane", "./DefaultModel/plane_64x64vertex.fbx");
					go->transform->local_pos = Vector3(pos[0], pos[1], pos[2]);
					//go->transform->local_scale = Vector3(10, 10, 10);

					auto coll = go->addComponent<Collider_Croth>();
					coll->load_file("./DefaultModel/plane_64x64vertex.fbx", true, false);

					Physics_data data;
					data = coll->get_vertex_data(0, 259);
					data.is_moveable = false;
					coll->set_vertex_data(0, 259, data);
					data = coll->get_vertex_data(0, 374);
					data.is_moveable = false;
					coll->set_vertex_data(0, 374, data);


					auto renderer = go->addComponent<Croth_renderer>();
					renderer->set_meshoffset(coll->get_vertex_offset());

					all_pearent->add_child(go);
				}
				imgui_num++;
			}

			ImGui::Columns(1);
			ImGui::End();
		}
	}

	void Stage_demo_02::Update_hierarchy() {



	}

	void Stage_demo_02::stage_destroy() {
		for (auto& object : stage_parts) {
			Gameobject_manager::deleteGameobject(object, true);
		}
		stage_parts.clear();
	}

}
