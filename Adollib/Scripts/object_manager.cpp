

#include "object_manager.h"
#include "../Adollib/Scripts/Object/gameobject_manager.h"
#include "../Adollib/Scripts/Scene/scene_manager.h"

#include "../Adollib/Scripts/Math/closest_func.h"
#include "../Adollib/Scripts/Imgui/imgui_all.h"
#include "../Adollib/Scripts/Physics/ALP__physics_manager.h"
#include "../Adollib/Scripts/Physics/joint.h"

#include "../Adollib/Scripts/Physics/ray.h"

#include "../Adollib/Scripts/Imgui/debug.h"

#include "../Adollib/Scripts/Main/systems.h"

#include "player.h"


namespace Adollib
{
	// 所属するシーンの初期化時に一度だけ呼ばれる
	void object_manager::awake() {
	}

	void object_manager::start()
	{
		camera = Gameobject_manager::find("camera");

		{
			sphere_go = Gameobject_manager::createSphere(std::string("render_notmal_s"));
			sphere_go->transform->local_scale = Vector3(1) * 0.05f;
			sphere_go->material->color = Vector4(1, 0, 0, 1);
		}
		{
			normal_go = Gameobject_manager::createCube(std::string("render_normal"));
			normal_go->transform->local_scale = Vector3(1, 1, 20) * 0.025f;
			normal_go->material->color = Vector4(1, 0, 0, 1);
		}

		enum class Stage_types {

			Cube,
			Sphere,
			Capsule,
			Mesh_Cube,
			Shaclo,
			Shaclo_stage,
			Teach
		};

		Stage_types stage_type = Stage_types::Cube;
		Gameobject* GO = nullptr;

		if (stage_type == Stage_types::Cube) {
			{
				GO = Gameobject_manager::createCube("stage");
				GO->transform->local_pos = Vector3(0, -60, 0);
				GO->transform->local_scale = Vector3(60, 60, 60);
				//GO->transform->local_orient = quaternion_from_euler(0, 45, 0);
				GO->material->color = Vector4(1, 1, 1, 1);

				Collider* coll = GO->addComponent<Collider>();
				Box* box = coll->add_shape<Box>();
				//Meshcoll* R = GO->addComponent<Meshcoll>();
				//R->load_mesh("./DefaultModel/cube.fbx");
				coll->physics_data.is_moveable = false;
				coll->is_static = true;
			}

		}
		if (stage_type == Stage_types::Mesh_Cube) {
			{
				//C:\Users\waaaa\Documents\Adollib\Adollib\Adollib\DefaultModel
				GO = Gameobject_manager::createFromFBX("../Adollib/DefaultModel/cube.fbx");
				GO->transform->local_pos = Vector3(0, -60, 0);
				GO->transform->local_scale = Vector3(60, 60, 60);
				//GO->transform->local_orient = quaternion_from_euler(0, 45, 0);
				GO->material->color = Vector4(1, 1, 1, 1);

				Collider* coll = GO->addComponent<Collider>();
				coll->add_shape("../Adollib/DefaultModel/cube.fbx", true);
				coll->physics_data.is_moveable = false;
				coll->is_static = true;
			}

		}
		else if (stage_type == Stage_types::Sphere) {
			{
				GO = Gameobject_manager::createSphere("stage");
				GO->transform->local_pos = Vector3(0, -6000, 0);
				GO->transform->local_scale = Vector3(6000, 6000, 6000);
				GO->material->color = Vector4(1, 1, 1, 1);
				Collider* coll = GO->addComponent<Collider>();
				Sphere* sphere = coll->add_shape<Sphere>();
				//Meshcoll* R = GO->addComponent<Meshcoll>();
				//R->load_mesh("./DefaultModel/cube.fbx");
				coll->physics_data.is_moveable = false;
				coll->is_static = true;

			}

		}
		else if (stage_type == Stage_types::Capsule) {
			{
				GO = Gameobject_manager::create("stage");
				GO->transform->local_pos = Vector3(0, -60, 0);
				GO->transform->local_scale = Vector3(60, 60, 60);

				Collider* coll = GO->addComponent<Collider>();
				Capsule* capsule = coll->add_shape<Capsule>();
				//Meshcoll* R = GO->addComponent<Meshcoll>();
				//R->load_mesh("./DefaultModel/cube.fbx");
				coll->physics_data.is_moveable = false;
				coll->is_static = true;

			}

		}
		else if (stage_type == Stage_types::Shaclo) {
			{
				GO = Gameobject_manager::createFromFBX("../Data/FBX/Model_Shaclo_Winter_Edit.fbx");
				//objGO = Gameobject_manager::create();
				//objGO = Gameobject_manager::createFromFBX("../Adollib/DefaultModel/cylinder.fbx");
				//GO->transform->local_pos = Vector3(-10, 15, -25);
				//objGO->transform->local_scale = Vector3(0.01f, 0.02f, 0.03f);
				//GO->transform->local_scale = Vector3(0.01f, 0.01f, 0.01f);
				//objGO->transform->local_orient = quaternion_from_euler(45, 45, 45);
				GO->transform->local_orient = quaternion_from_euler(0, 180, 0);

				Collider* coll = GO->addComponent<Collider>();
				coll->add_shape("../Data/FBX/Model_Shaclo_Winter_Edit.fbx", true, true);
				//R->load_mesh("../Adollib/DefaultModel/cylinder.fbx");
				coll->physics_data.is_moveable = false;
				coll->is_static = true;
			}
		}
		else if (stage_type == Stage_types::Shaclo_stage) {
			{
				//objGO = Gameobject_manager::createFromFBX("../Data/FBX/big_plane.fbx");
				GO = Gameobject_manager::createFromFBX("../Data/FBX/stage_col.fbx");
				//objGO = Gameobject_manager::createFromFBX("../Data/FBX/0311_collisions.fbx");
				//objGO = Gameobject_manager::create();
				//objGO = Gameobject_manager::createFromFBX("../Adollib/DefaultModel/cylinder.fbx");
				GO->transform->local_pos = Vector3(-10, 0, -25);
				//objGO->transform->local_scale = Vector3(0.01f, 0.02f, 0.03f);
				GO->transform->local_scale = Vector3(1, 1, 1);
				//objGO->transform->local_orient = quaternion_from_euler(45, 45, 45);
				GO->transform->local_orient = quaternion_from_euler(0, 0, 0);

				Collider* coll = GO->addComponent<Collider>();
				coll->add_shape("../Data/FBX/stage_col.fbx", true);
				//R->load_mesh("../Adollib/DefaultModel/cylinder.fbx");
				coll->physics_data.is_moveable = false;
				coll->is_static = true;

			}

		}
		else if (stage_type == Stage_types::Teach) {
			{
				GO = Gameobject_manager::createFromFBX("../Data/FBX/0311_collisions.fbx");
				//objGO = Gameobject_manager::create();
				//objGO = Gameobject_manager::createFromFBX("../Adollib/DefaultModel/cylinder.fbx");
				GO->transform->local_pos = Vector3(-10, 15, -25);
				//objGO->transform->local_scale = Vector3(0.01f, 0.02f, 0.03f);
				GO->transform->local_scale = Vector3(0.01f, 0.01f, 0.01f);
				//objGO->transform->local_orient = quaternion_from_euler(45, 45, 45);
				GO->transform->local_orient = quaternion_from_euler(0, 180, 0);

				Collider* coll = GO->addComponent<Collider>();
				coll->add_shape("../Data/FBX/0311_collisions.fbx", true);
				//R->load_mesh("../Adollib/DefaultModel/cylinder.fbx");
				coll->physics_data.is_moveable = false;
				coll->is_static = true;
			}
		}

		Vector3 axisA = Vector3(1, 1, 0).unit_vect();
		Quaternion x90 = quaternion_from_euler(90, 0, 0);
		Quaternion Y90 = quaternion_from_euler(0, 90, 0);
		Quaternion Z90 = quaternion_from_euler(0, 0, 90);

		Quaternion xy90 = quaternion_from_euler(90, 90, 0);
		Quaternion yz90 = quaternion_from_euler(0, 90, 90);
		Quaternion zx90 = quaternion_from_euler(90, 0, 90);

		Vector3 axisB = Vector3(0, 0, 1);
		if (vector3_dot(axisA, axisB) == 0) axisB = Vector3(0, 1, 0);

		Vector3 axis = vector3_cross(axisA, axisB).unit_vect();


		int dsdvf = 0;
	}

	// 毎フレーム呼ばれる更新処理
	void object_manager::update()
	{
		ImGuiWindowFlags flag = 0;

		//flag |= ImGuiWindowFlags_AlwaysAutoResize;

	/*	Quaternion a = quaternion_from_euler(0, 180, 0);
		Vector3 b = Vector3(0, 180, 0);
		Vector3 c = Vector3(0, -180, 0);
		Quaternion d = quaternion_from_to_rotate(b.unit_vect(), c.unit_vect());*/
		///	Adollib::Ray


#pragma region IMGUI

		if (ImGui::Begin("object_manage", 0, flag)) {

			ImGui::InputFloat3(std::string("clear_color").c_str(), &Systems::clear_color[0], "%.3f"); ImGui::NextColumn();
			ImGui::Checkbox("is_draw_raycast", &is_draw_raycast);

			//objectの削除
			bool del = false;
			ImGui::Checkbox("delete", &del);
			if (del) {
				for (auto& GO : GOs) {
					Gameobject_manager::deleteGameobject(GO);
				}
				boxes.clear();
				GOs.clear();
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
					for (int i = 0; i < BOX_pyramid_count; i++) {
						for (int o = 0; o < BOX_pyramid_count - i; o++) {
							Collider* out_coll;
							pearent->add_child(set_box(out_coll, Vector3(
								BOX_pyramid_size[0] * 2.0f * 1.2f * o - (BOX_pyramid_count - i) * 2.300001f / 2.0f + BOX_pyramid_pos[0],
								3.0f + BOX_pyramid_size[1] * 2 * i + BOX_pyramid_pos[1],
								BOX_pyramid_pos[2]),
								Vector3(BOX_pyramid_size[0], BOX_pyramid_size[1], BOX_pyramid_size[2]),
								Vector3(0, 10, 0),
								Vector3(0, 1, 1))
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
					for (int i = 0; i < SPHERE_pyramid_count; i++) {
						for (int o = 0; o < SPHERE_pyramid_count - i; o++) {
							Collider* coll;
							pearent->add_child(
								set_sphere(coll,
									Vector3(2.50001f * o - (SPHERE_pyramid_count - i) * 2.500001f / 2.0f + SPHERE_pyramid_pos[0],
										5.0f + 2.50001f * i + SPHERE_pyramid_pos[1],
										SPHERE_pyramid_pos[2]),
									size,
									Vector3(0, 1, 1)
								));
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
							set_capsule(Vector3(2.50001f * o - (CAPSULE_pyramid_count - i) * 2.500001f / 2.0f + CAPSULE_pyramid_pos[0], 5.0f + 4.50001f * i + CAPSULE_pyramid_pos[1], CAPSULE_pyramid_pos[2]), 1, 1, Vector3(0, 1, 1));
						}

					}
				imgui_num++;
			}

			//Meshpyramid
			{
				static int Mesh_pyramid_count = 5;
				static float Mesh_pyramid_pos[3] = { 0 };
				bool summon = false;
				ImGui::Separator();
				ImGui::Text("Mesh_pyramid"); ImGui::NextColumn();
				ImGui::Checkbox(std::to_string(imgui_num + 100).c_str(), &summon); ImGui::NextColumn();
				ImGui::DragFloat3(std::to_string(imgui_num + 200).c_str(), Mesh_pyramid_pos, 0.1f); ImGui::NextColumn(); ImGui::NextColumn();
				ImGui::DragInt(std::to_string(imgui_num + 300).c_str(), &Mesh_pyramid_count, 1, 1, 100000); ImGui::NextColumn();

				if (summon == true)
					for (int i = 0; i < Mesh_pyramid_count; i++) {
						for (int o = 0; o < Mesh_pyramid_count - i; o++) {
							set_meshbox(Vector3(2.00001f * o - (Mesh_pyramid_count - i) * 2.000001f / 2.0f + Mesh_pyramid_pos[0], 3.0f + 2.00001f * i + Mesh_pyramid_pos[1], Mesh_pyramid_pos[2]), Vector3(1, 1, 1), Vector3(180, 0, 0), Vector3(0, 1, 1));
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
							//parts[0]->material->color = Vector4(238, 229, 224, 255) / 255;
							////parts[1]->material->color = Vector4(207, 171, 142, 255) / 255;
							//parts[1]->material->color = Vector4(255, 77, 17, 255) / 255;

							parts[0]->material->color = C;
							parts[1]->material->color = C;
							parts[2]->material->color = C;
							parts[3]->material->color = C;
							parts[4]->material->color = C;

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

							GOs.emplace_back(Daruma);
							GOs.emplace_back(parts[0]);
							GOs.emplace_back(parts[1]);
							GOs.emplace_back(parts[2]);
							GOs.emplace_back(parts[3]);
							GOs.emplace_back(parts[4]);
						}

					}
				}
				imgui_num++;
			}


			//treepyramid
			{
				static int TREE_pyramid_count = 5;
				static float TREE_pyramid_pos[3] = { 0 };
				bool summon = false;
				ImGui::Separator();
				ImGui::Text("TREE_pyramid"); ImGui::NextColumn();
				ImGui::Checkbox(std::to_string(imgui_num + 100).c_str(), &summon); ImGui::NextColumn();
				ImGui::DragFloat3(std::to_string(imgui_num + 200).c_str(), TREE_pyramid_pos, 0.1f); ImGui::NextColumn(); ImGui::NextColumn();
				ImGui::DragInt(std::to_string(imgui_num + 300).c_str(), &TREE_pyramid_count, 1, 1, 100000); ImGui::NextColumn();

				if (summon == true) {
					Gameobject* pearent = Gameobject_manager::create("Treepyramid");
					for (int i = 0; i < TREE_pyramid_count; i++) {
						for (int o = 0; o < TREE_pyramid_count - i; o++) {
							pearent->add_child(
								set_tree(
									Vector3(10.0f * o - (TREE_pyramid_count - i) * 10.0f / 2.0f + TREE_pyramid_pos[0],
										TREE_pyramid_pos[1] + i * 13.0f,
										TREE_pyramid_pos[2]),
									Vector3(1),
									Vector3(0),
									Vector3(1)
								));
						}

					}
				}
				imgui_num++;
			}


			//BallJoint_Shpererope
			{
				static int JointBox_pyramid_count = 5;
				static float JointBox_pyramid_pos[3] = { 0 };
				static float JointBox_pyramid_size[3] = { 1,1,1 };
				bool summon = false;
				ImGui::Separator();
				ImGui::Text("BallJoint_Shpererope"); ImGui::NextColumn();
				ImGui::Checkbox(std::to_string(imgui_num + 100).c_str(), &summon); ImGui::NextColumn();
				ImGui::DragFloat3(std::to_string(imgui_num + 200).c_str(), JointBox_pyramid_pos, 0.1f); ImGui::NextColumn();
				ImGui::DragFloat3(std::to_string(imgui_num + 250).c_str(), JointBox_pyramid_size, 0.1f); ImGui::NextColumn();
				ImGui::DragInt(std::to_string(imgui_num + 300).c_str(), &JointBox_pyramid_count, 1, 1, 100000); ImGui::NextColumn();

				static Gameobject* joint_base = Gameobject_manager::create("BallJoint_Shpererope_joint_base");
				if (summon == true) {
					Gameobject* pearent = Gameobject_manager::create("BallJoint_Shpererope");
					Collider* old_coll = nullptr;
					Gameobject* old_go = nullptr;

					{
						Collider* coll = nullptr;
						Gameobject* go = nullptr;
						go = set_sphere(coll, Vector3(
							JointBox_pyramid_pos[0],
							JointBox_pyramid_pos[1] + 2,
							JointBox_pyramid_pos[2]
						),
							2,
							Vector3(0, 1, 1))
							;

						coll->physics_data.inertial_mass = 10;
						pearent->add_child(go);

						old_coll = coll;
						old_go = go;
					}

					for (int i = 0; i < JointBox_pyramid_count; i++) {
						Collider* coll = nullptr;
						Gameobject* go = nullptr;
						go = set_sphere(coll, Vector3(
							JointBox_pyramid_pos[0] + 0.0001f,
							JointBox_pyramid_pos[1] + i * 2 + 5,
							JointBox_pyramid_pos[2]
						),
							1,
							Vector3(1, 0, (1.0f / JointBox_pyramid_count) * i)
						);

						pearent->add_child(go);

						if (old_coll != nullptr) {
							if (i == 0)
								Joint::add_balljoint(old_coll, coll, Vector3(0, 2, 0), Vector3(0, -1, 0), 0.1f);
							else
								Joint::add_balljoint(old_coll, coll, Vector3(0, 1, 0), Vector3(0, -1, 0), 0.1f);
						}

						old_coll = coll;
						old_go = go;
					}


					if (old_coll != nullptr) {
						old_coll->physics_data.is_moveable = false;
						joint_base->add_child(old_go);
					}



				}

				imgui_num++;
			}

			//BallJoint_Boxrope
			{
				static int JointBox_pyramid_count = 5;
				static float JointBox_pyramid_pos[3] = { 0 };
				static float JointBox_pyramid_size[3] = { 1,1,1 };
				bool summon = false;
				ImGui::Separator();
				ImGui::Text("BallJoint_Boxrope"); ImGui::NextColumn();
				ImGui::Checkbox(std::to_string(imgui_num + 100).c_str(), &summon); ImGui::NextColumn();
				ImGui::DragFloat3(std::to_string(imgui_num + 200).c_str(), JointBox_pyramid_pos, 0.1f); ImGui::NextColumn();
				ImGui::DragFloat3(std::to_string(imgui_num + 250).c_str(), JointBox_pyramid_size, 0.1f); ImGui::NextColumn();
				ImGui::DragInt(std::to_string(imgui_num + 300).c_str(), &JointBox_pyramid_count, 1, 1, 100000); ImGui::NextColumn();

				static Gameobject* joint_base = Gameobject_manager::create("BallJoint_Boxrope_joint_base");
				if (summon == true) {
					Gameobject* pearent = Gameobject_manager::create("BOXpyramid");
					Collider* old_coll = nullptr;
					Gameobject* old_go = nullptr;

					for (int i = 0; i < JointBox_pyramid_count; i++) {
						Collider* coll = nullptr;
						Gameobject* go = nullptr;
						go = set_box(coll, Vector3(
							JointBox_pyramid_size[0] + i * 2 - JointBox_pyramid_count * 2,
							JointBox_pyramid_pos[1],
							JointBox_pyramid_pos[2] + i * 2 - JointBox_pyramid_count * 2
						),
							Vector3(JointBox_pyramid_size[0], JointBox_pyramid_size[1], JointBox_pyramid_size[2]),
							Vector3(0, 0, 0),
							Vector3(0, 1, 1))
							;

						pearent->add_child(go);

						if (old_coll != nullptr) {
							Joint::add_balljoint(old_coll, coll, Vector3(1, 1, 1), Vector3(-1, 1, -1), 0.1f);
						}
						old_coll = coll;
						old_go = go;
					}
					if (old_coll != nullptr) {
						old_coll->physics_data.is_moveable = false;
						joint_base->add_child(old_go);
					}



				}

				imgui_num++;
			}

			//BallJoint_Shperenet
			{
				static int JointBox_pyramid_count = 5;
				static float JointBox_pyramid_pos[3] = { 0 };
				static float JointBox_pyramid_size[3] = { 1,1,1 };
				bool summon = false;
				ImGui::Separator();
				ImGui::Text("BallJoint_Shperenet"); ImGui::NextColumn();
				ImGui::Checkbox(std::to_string(imgui_num + 100).c_str(), &summon); ImGui::NextColumn();
				ImGui::DragFloat3(std::to_string(imgui_num + 200).c_str(), JointBox_pyramid_pos, 0.1f); ImGui::NextColumn();
				ImGui::DragFloat3(std::to_string(imgui_num + 250).c_str(), JointBox_pyramid_size, 0.1f); ImGui::NextColumn();
				ImGui::DragInt(std::to_string(imgui_num + 300).c_str(), &JointBox_pyramid_count, 1, 1, 100000); ImGui::NextColumn();

				static Gameobject* joint_base = Gameobject_manager::create("BallJoint_Shpererope_joint_base");
				if (summon == true) {
					Gameobject* pearent = Gameobject_manager::create("BallJoint_Shpererope");
					std::vector<Collider*>colls;

					const int colls_size = JointBox_pyramid_count * JointBox_pyramid_count;
					colls.resize(colls_size);

					for (int xaxis = 0; xaxis < JointBox_pyramid_count; xaxis++) {
						for (int zaxis = 0; zaxis < JointBox_pyramid_count; zaxis++) {
							int index = xaxis * JointBox_pyramid_count + zaxis;

							Collider* coll = nullptr;
							Gameobject* go = nullptr;

							go = set_sphere(coll, Vector3(
								JointBox_pyramid_pos[0] + (xaxis - JointBox_pyramid_count * 0.5f) * 2,
								JointBox_pyramid_pos[1] + 2,
								JointBox_pyramid_pos[2] + (zaxis - JointBox_pyramid_count * 0.5f) * 2
							),
								1,
								Vector3(1, 0, (1.0f / (JointBox_pyramid_count * JointBox_pyramid_count)) * index)
							);

							pearent->add_child(go);
							colls.at(index) = (coll);
						}
					}

					for (int xaxis = 0; xaxis < JointBox_pyramid_count; xaxis++) {
						for (int zaxis = 0; zaxis < JointBox_pyramid_count; zaxis++) {
							int index = xaxis * JointBox_pyramid_count + zaxis;

							if (xaxis > 0) Joint::add_balljoint(colls[index], colls[index - JointBox_pyramid_count], Vector3(-1, 0, 0), Vector3(1, 0, 0), 0.1f);
							if (zaxis > 0) Joint::add_balljoint(colls[index], colls[index - 1], Vector3(0, 0, -1), Vector3(0, 0, 1), 0.1f);
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
					GEAR->transform->local_pos = pos;

					Collider* coll = GEAR->addComponent<Collider>();
					//coll->set_tensor(make_box_tensor(Vector3(5, 5, 50), 1));

					for (int gear_tooth_num = 0; gear_tooth_num < TREE_pyramid_count; gear_tooth_num++) {
						Gameobject* GO = Gameobject_manager::createCube("tooth");
						GO->transform->local_scale = size;
						GO->transform->local_orient = quaternion_from_euler(0, 0, 360.0f / TREE_pyramid_count * gear_tooth_num);
						GO->transform->local_pos = Vector3(0, size.y * 2, 0);
						GO->transform->local_pos = vector3_quatrotate(GO->transform->local_pos, quaternion_angle_axis(360.0f / TREE_pyramid_count * gear_tooth_num, Vector3(0, 0, 1)));

						GO->material->color = Vector4(1, 1, 0, 1);
						GEAR->add_child(GO);
					}

					for (int gear_tooth_num = 0; gear_tooth_num < TREE_pyramid_count; gear_tooth_num++) {
						Box* box = coll->add_shape<Box>();
						box->size = size;
						box->rotate = Vector3(0, 0, 360.0f / TREE_pyramid_count * gear_tooth_num);
						box->center = Vector3(0, size.y * 2, 0);
						box->center = vector3_quatrotate(box->center, quaternion_angle_axis(360.0f / TREE_pyramid_count * gear_tooth_num, Vector3(0, 0, 1)));

					}


					Gameobject* gear_joint = Gameobject_manager::createCube("gear_joint");
					gear_joint->transform->local_pos = pos;
					Collider* gear_joint_collider = gear_joint->addComponent<Collider>();
					gear_joint_collider->physics_data.is_moveable = false;

					Joint::add_Hingejoint(gear_joint_collider, coll, Vector3(0, 0, +10), Vector3(0, 0, -10), Vector3(0, 0, +10), Vector3(0, 0, -10), 2, 0.1f);

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
						GO->transform->local_pos = vector3_quatrotate(GO->transform->local_pos, quaternion_angle_axis(360.0f / TREE_pyramid_count * gear_tooth_num, Vector3(0, 1, 0)));

						GO->material->color = Vector4(1, 1, 0, 1);
						PATAPATA->add_child(GO);
					}

					for (int gear_tooth_num = 0; gear_tooth_num < TREE_pyramid_count; gear_tooth_num++) {
						Box* box = coll->add_shape<Box>();
						box->size = size;
						box->rotate = Vector3(0, 360.0f / TREE_pyramid_count * gear_tooth_num, 0);
						box->center = Vector3(0, size.y * 0, 0);
						box->center = vector3_quatrotate(box->center, quaternion_angle_axis(360.0f / TREE_pyramid_count * gear_tooth_num, Vector3(0, 1, 0)));

					}


					Gameobject* gear_joint = Gameobject_manager::createCube("gear_joint");
					gear_joint->transform->local_pos = pos;
					Collider* gear_joint_collider = gear_joint->addComponent<Collider>();
					gear_joint_collider->physics_data.is_moveable = false;

					//auto hinge = Joint::add_Hingejoint(gear_joint_collider, coll, Vector3(0, +10, 0), Vector3(0, -10, 0), Vector3(0, +10, 0), Vector3(0, -10, 0), 2, 0.1f);
					auto hinge = Joint::add_Hingejoint(
						gear_joint_collider,
						coll,
						Vector3(0, 10, 10).unit_vect() * 10, Vector3(0, -10, -10).unit_vect() * 10,
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

			ImGui::Columns(1);
			ImGui::End();
		}

#pragma endregion

		if (is_draw_raycast) {
			Ray ray;
			ray.position = camera->transform->position;
			ray.direction = vector3_quatrotate(Vector3(0, 0, 1), camera->transform->orientation);

			float tmin = 0, tmax = 0;
			Vector3 nor;
			float ray_min = 0;
			ray.ray_cast(UINT_MAX, ray_min, tmin, tmax, nor);

			Debug::set("", nor.get_XM3());

			sphere_go->transform->local_pos = ray.position + tmin * ray.direction;
			normal_go->transform->local_orient = quaternion_from_to_rotate(Vector3(0, 0, 1), nor);
			normal_go->transform->local_pos = sphere_go->transform->local_pos + nor * (sphere_go->transform->scale.x + normal_go->transform->scale.z);
		}
		else {
			normal_go->transform->local_pos = Vector3(10000, 10000, 10000);
			sphere_go->transform->local_pos = Vector3(10000, 10000, 10000);
		}
	}




	// このスクリプトがアタッチされているGOのactiveSelfがtrueになった時呼ばれる
	void object_manager::onEnable()
	{

	}

	// このスクリプトがアタッチされているGOのactiveSelfがfalseになった時呼ばれる
	void object_manager::onDisable()
	{

	}

}

namespace Adollib
{
	Gameobject* object_manager::set_sphere(Collider*& out_coll, Vector3 pos, float r, Vector3 color) {
		Gameobject* object = nullptr;
		object = Gameobject_manager::createSphere(GO_tag::Sphere);
		//Vector4 C = Vector4(color.x, color.y, color.z, 1);
		Vector4 C = Vector4(color, 1);
		object->material->color = C;

		//object->addComponent<object_fall>();
		object->transform->local_pos = pos;
		object->transform->local_scale = Vector3(r, r, r);


		Collider* coll = object->addComponent<Collider>();
		coll->add_shape<Sphere>();
		coll->tag = Collider_tags::croth;
		//coll->ignore_tags = Collider_tags::croth;
		//coll->ignore_tags = UINT_MAX;

		coll->tag |= Collider_tags::Sphere;
		GOs.emplace_back(object);

		out_coll = coll;
		return object;
	}

	Gameobject* object_manager::set_box(Collider*& out_coll, Vector3 pos, Vector3 size, Vector3 rotate, Vector3 color) {
		Gameobject* object = nullptr;
		object = Gameobject_manager::createCube(GO_tag::Box);
		Vector4 C = Vector4(color.x, color.y, color.z, 1);
		object->material->color = C;

		//object->addComponent<object_fall>();
		object->transform->local_orient = quaternion_from_euler(rotate);
		object->transform->local_pos = pos;
		object->transform->local_scale = size;

		Collider* coll = object->addComponent<Collider>();
		Box* box = coll->add_shape<Box>();

		coll->tag = Collider_tags::Box;

		GOs.emplace_back(object);
		boxes.emplace_back(coll);

		out_coll = coll;
		return object;
	}
	Gameobject* object_manager::set_capsule(Vector3 pos, float r, float length, Vector3 rotate, Vector3 color) {
		Gameobject* object = nullptr;
		object = Gameobject_manager::create(GO_tag::Box);
		Vector4 C = Vector4(color.x, color.y, color.z, 1);
		//object->material->color = C;

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
		GOs.emplace_back(object);
		return object;
	}

	Gameobject* object_manager::set_meshbox(Vector3 pos, Vector3 size, Vector3 rotate, Vector3 color) {
		Gameobject* object = nullptr;
		object = Gameobject_manager::createFromFBX("./DefaultModel/cone.fbx");
		Vector4 C = Vector4(color.x, color.y, color.z, 1);
		object->material->color = C;

		//object->addComponent<object_fall>();
		object->transform->local_orient = quaternion_from_euler(rotate);
		object->transform->local_pos = pos;
		object->transform->local_scale = size;

		//Box* M = object->addComponent<Box>();
		Collider* coll = object->addComponent<Collider>();
		coll->add_shape("./DefaultModel/cone.fbx");

		//M->inertial_mass = 1;
		GOs.emplace_back(object);
		return object;
	}

	Gameobject* object_manager::set_plane(Vector3 pos, Vector3 normal, Vector3 color) {
		Gameobject* object = nullptr;
		object = Gameobject_manager::createCube(GO_tag::Plane);
		Vector4 C = Vector4(color.x, color.y, color.z, 1);
		object->material->color = C;

		object->transform->local_orient = quaternion_from_euler(normal);
		object->transform->local_pos = pos;
		object->transform->local_scale = Vector3(30, 0.1f, 30);

		Collider* coll = object->addComponent<Collider>();
		Plane* plane = coll->add_shape<Plane>();

		GOs.emplace_back(object);
		return object;
	}

	Gameobject* object_manager::set_tree(Vector3 pos, Vector3 scale, Vector3 rotate, Vector3 color) {
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
			tree_parts[i] = Gameobject_manager::createCube(GO_tag::Box);
		}

		tree_parts[0]->transform->local_pos = Vector3(0, 7, 0);
		tree_parts[0]->transform->local_scale = Vector3(1, 6.9f, 1);
		tree_parts[0]->material->color = stem_color;

		tree_parts[1]->transform->local_pos = Vector3(0, 15, 0);
		tree_parts[1]->transform->local_scale = Vector3(1.5, 1, 1.5);
		tree_parts[1]->transform->local_orient = quaternion_from_euler(0, 0, 0);
		tree_parts[1]->material->color = reaf_color;

		tree_parts[2]->transform->local_pos = Vector3(0, 13, 0);
		tree_parts[2]->transform->local_scale = Vector3(2.0, 1, 2.0);
		tree_parts[2]->transform->local_orient = quaternion_from_euler(0, 1.7f, 0);
		tree_parts[2]->material->color = reaf_color;

		tree_parts[3]->transform->local_pos = Vector3(0, 11, 0);
		tree_parts[3]->transform->local_scale = Vector3(3.0, 1, 3.0);
		tree_parts[3]->transform->local_orient = quaternion_from_euler(0, 13, 0);
		tree_parts[3]->material->color = reaf_color;

		tree_parts[4]->transform->local_pos = Vector3(0, 9, 0);
		tree_parts[4]->transform->local_scale = Vector3(3.5, 1, 3.5);
		tree_parts[4]->transform->local_orient = quaternion_from_euler(0, 28, 0);
		tree_parts[4]->material->color = reaf_color;

		tree_parts[5]->transform->local_pos = Vector3(0, 7, 0);
		tree_parts[5]->transform->local_scale = Vector3(4.5, 1, 4.5);
		tree_parts[5]->transform->local_orient = quaternion_from_euler(0, 14, 0);
		tree_parts[5]->material->color = reaf_color;

		tree_parts[6]->transform->local_pos = Vector3(0, 0.2f, 0);
		tree_parts[6]->transform->local_scale = Vector3(3, 0.2f, 3);
		tree_parts[6]->transform->local_orient = quaternion_from_euler(0, 0, 0);
		tree_parts[6]->material->color = floar_color;

		for (int i = 0; i < Tree_size; i++) {
			tree->add_child(tree_parts[i]);
		}

		Box* boxes[Tree_size];
		Collider* coll = tree->addComponent<Collider>();
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


}