
#include "physics_demo_obj.h"

#include "../../Scripts/Object/gameobject_manager.h"
#include "../../Scripts/Renderer/renderer_base.h"
#include "../../Scripts/Renderer/croth_renderer.h"

#include "../../Scripts/Imgui/imgui_all.h"

#include "collobject_creater.h"
#include "collider_comp.h"
//#include "collider_croth_comp.h"



#include <Adollibphysics.h>

using namespace Adollib;

void Physics_demo_obj::awake() {



}

void Physics_demo_obj::update() {

#pragma region IMGUI
	ImGuiWindowFlags flag = 0;

	if (ImGui::Begin("object_manage", 0, flag)) {
		static bool is_draw_raycast = false;
		static std::vector<Gameobject*> GOs;

		ImGui::Checkbox("is_draw_raycast", &is_draw_raycast);

		//objectの削除
		bool del = false;
		ImGui::Checkbox("delete", &del);
		if (del) {
			for (auto& GO : GOs) {
				Gameobject_manager::deleteGameobject(GO, true);
			}
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
				GOs.emplace_back(pearent);
				for (int i = 0; i < BOX_pyramid_count; i++) {
					for (int o = 0; o < BOX_pyramid_count - i; o++) {
						Collider_creater::create_box(Vector3(
							BOX_pyramid_size[0] * 2.0f * 1.2f * o - (BOX_pyramid_count - i) * 2.300001f / 2.0f + BOX_pyramid_pos[0],
							3.0f + BOX_pyramid_size[1] * 2 * i + BOX_pyramid_pos[1],
							BOX_pyramid_pos[2]),
							Vector3(0, 10, 0),
							Vector3(BOX_pyramid_size[0], BOX_pyramid_size[1], BOX_pyramid_size[2]),
							Vector3(0, 255, 255),
							pearent
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
				GOs.emplace_back(pearent);
				for (int i = 0; i < SPHERE_pyramid_count; i++) {
					for (int o = 0; o < SPHERE_pyramid_count - i; o++) {
						auto coll = Collider_creater::create_sphere(
							Vector3(2.50001f * o - (SPHERE_pyramid_count - i) * 2.500001f / 2.0f + SPHERE_pyramid_pos[0],
								5.0f + 2.50001f * i + SPHERE_pyramid_pos[1],
								SPHERE_pyramid_pos[2]),
							size,
							Vector3(0, 255, 255),
							pearent

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

			if (summon == true) {
				Gameobject* pearent = Gameobject_manager::create("Capsulepyramid");
				GOs.emplace_back(pearent);

				for (int i = 0; i < CAPSULE_pyramid_count; i++) {
					for (int o = 0; o < CAPSULE_pyramid_count - i; o++) {
						Collider_creater::create_capsule(
							Vector3(
								2.50001f * o - (CAPSULE_pyramid_count - i) * 2.500001f / 2.0f + CAPSULE_pyramid_pos[0],
								5.0f + 4.50001f * i + CAPSULE_pyramid_pos[1],
								CAPSULE_pyramid_pos[2]),
							1,
							1,
							Vector3(255, 0, 255),
							pearent
						);
					}

				}
			}
			imgui_num++;
		}

		/*
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
		*/

		//deskpyramid
		{
			static int count = 5;
			static float pos[3] = { 0 };
			bool summon = false;
			ImGui::Separator();
			ImGui::Text("Desk_pyramid"); ImGui::NextColumn();
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
						Daruma = Gameobject_manager::create("Desk", GO_tag::Sphere);
						Daruma->transform->local_pos = Vector3(position[0], position[1], position[2]);
						Daruma->transform->local_scale = Vector3(1, 1, 1) * 0.5f;

						Collider_comp* coll = Daruma->addComponent<Collider_comp>();

						{
							Box* box[5] = { nullptr };
							box[0] = coll->get_collider()->add_shape<Box>();
							box[1] = coll->get_collider()->add_shape<Box>();
							box[2] = coll->get_collider()->add_shape<Box>();
							box[3] = coll->get_collider()->add_shape<Box>();
							box[4] = coll->get_collider()->add_shape<Box>();

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

						GOs.emplace_back(Daruma);
					}

				}
			}
			imgui_num++;
		}

		{}

		/*
		////treepyramid
		//{

		//	static int TREE_pyramid_count = 5;
		//	static float TREE_pyramid_pos[3] = { 0 };
		//	bool summon = false;
		//	ImGui::Separator();
		//	ImGui::Text("TREE_pyramid"); ImGui::NextColumn();
		//	ImGui::Checkbox(std::to_string(imgui_num + 100).c_str(), &summon); ImGui::NextColumn();
		//	ImGui::DragFloat3(std::to_string(imgui_num + 200).c_str(), TREE_pyramid_pos, 0.1f); ImGui::NextColumn(); ImGui::NextColumn();
		//	ImGui::DragInt(std::to_string(imgui_num + 300).c_str(), &TREE_pyramid_count, 1, 1, 100000); ImGui::NextColumn();

		//	if (summon == true) {
		//		Gameobject* pearent = Gameobject_manager::create("Treepyramid");
		//		GOs.emplace_back(pearent);
		//		for (int i = 0; i < TREE_pyramid_count; i++) {
		//			for (int o = 0; o < TREE_pyramid_count - i; o++) {
		//				pearent->add_child(
		//					set_tree(
		//						Vector3(10.0f * o - (TREE_pyramid_count - i) * 10.0f / 2.0f + TREE_pyramid_pos[0],
		//							TREE_pyramid_pos[1] + i * 13.0f,
		//							TREE_pyramid_pos[2]),
		//						Vector3(1),
		//						Vector3(0),
		//						Vector3(1)
		//					));
		//			}

		//		}
		//	}
		//	imgui_num++;
		//}
		*/


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

			if (summon == true) {
				Gameobject* joint_base = Gameobject_manager::create("BallJoint_Shpererope_joint_base");
				Gameobject* pearent = Gameobject_manager::create("BallJoint_Shpererope");
				pearent->add_child(joint_base);
				GOs.emplace_back(pearent);
				Collider_comp* old_coll = nullptr;
				Gameobject* old_go = nullptr;

				{
					Gameobject* go = nullptr;
					auto coll_comp = Collider_creater::create_sphere(Vector3(
						JointBox_pyramid_pos[0],
						JointBox_pyramid_pos[1] + 2,
						JointBox_pyramid_pos[2]
					),
						2,
						Vector3(0, 255, 255)
					);

					go = coll_comp->gameobject;
					pearent->add_child(go);

					coll_comp->get_collider()->physics_data.inertial_mass = 10;

					old_coll = coll_comp;
					old_go = go;
				}

				for (int i = 0; i < JointBox_pyramid_count; i++) {
					Gameobject* go = nullptr;
					auto coll_comp = Collider_creater::create_sphere(Vector3(
						JointBox_pyramid_pos[0] + 0.0001f,
						JointBox_pyramid_pos[1] + i * 2 + 5,
						JointBox_pyramid_pos[2]
					),
						1,
						Vector3(255, 0, (255.0f / JointBox_pyramid_count) * i)
					);
					go = coll_comp->gameobject;

					pearent->add_child(go);

					if (old_coll != nullptr) {
						if (i == 0)
							Joint::add_balljoint(old_coll->get_collider(), coll_comp->get_collider(), Vector3(0, 2, 0), Vector3(0, -1, 0), 0.1f);
						else
							Joint::add_balljoint(old_coll->get_collider(), coll_comp->get_collider(), Vector3(0, 1, 0), Vector3(0, -1, 0), 0.1f);
					}

					old_coll = coll_comp;
					old_go = go;
				}


				if (old_coll != nullptr) {
					old_coll->get_collider()->physics_data.is_moveable = false;
					joint_base->add_child(old_go);
				}



			}

			imgui_num++;
		}

		/*
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
				GOs.emplace_back(pearent);
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
		*/

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

				Collider_comp* coll = GEAR->addComponent<Collider_comp>();
				coll->get_collider()->tag |= Collider_tags::Stage;
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
					Box* box = coll->get_collider()->add_shape<Box>();
					box->size = size;
					box->rotate = Vector3(0, 0, 360.0f / TREE_pyramid_count * gear_tooth_num);
					box->center = Vector3(0, size.y * 2, 0);
					box->center = Adollib::vector3_quatrotate(box->center, quaternion_axis_angle(Vector3(0, 0, 1), 360.0f / TREE_pyramid_count * gear_tooth_num));

				}


				Gameobject* gear_joint = Gameobject_manager::createCube("gear_joint");
				gear_joint->transform->local_pos = pos;
				Collider_comp* gear_joint_collider = gear_joint->addComponent<Collider_comp>();
				gear_joint_collider->get_collider()->physics_data.is_moveable = false;

				Joint::add_Hingejoint(gear_joint_collider->get_collider(), coll->get_collider(), Vector3(0, 0, +10), Vector3(0, 0, -10), Vector3(0, 0, +10), Vector3(0, 0, -10), 1, 0.1f);

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

				Collider_comp* coll = PATAPATA->addComponent<Collider_comp>();
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
					Box* box = coll->get_collider()->add_shape<Box>();
					box->size = size;
					box->rotate = Vector3(0, 360.0f / TREE_pyramid_count * gear_tooth_num, 0);
					box->center = Vector3(0, size.y * 0, 0);
					box->center = Adollib::vector3_quatrotate(box->center, quaternion_axis_angle(Vector3(0, 1, 0), 360.0f / TREE_pyramid_count * gear_tooth_num));

				}


				Gameobject* gear_joint = Gameobject_manager::createCube("gear_joint");
				gear_joint->transform->local_pos = pos;
				Collider_comp* gear_joint_collider = gear_joint->addComponent<Collider_comp>();
				gear_joint_collider->get_collider()->physics_data.is_moveable = false;

				//auto hinge = Joint::add_Hingejoint(gear_joint_collider, coll, Vector3(0, +10, 0), Vector3(0, -10, 0), Vector3(0, +10, 0), Vector3(0, -10, 0), 2, 0.1f);
				auto hinge = Joint::add_Hingejoint(
					gear_joint_collider->get_collider(),
					coll->get_collider(),
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

		/*
		//ConeTwist
		{

			static int TREE_pyramid_count = 3;
			static float TREE_pyramid_pos[3] = { 0 };
			bool summon = false;
			ImGui::Separator();
			ImGui::Text("ConeWist"); ImGui::NextColumn();
			ImGui::Checkbox(std::to_string(imgui_num + 100).c_str(), &summon); ImGui::NextColumn();
			ImGui::DragFloat3(std::to_string(imgui_num + 200).c_str(), TREE_pyramid_pos, 0.1f); ImGui::NextColumn(); ImGui::NextColumn();
			ImGui::DragInt(std::to_string(imgui_num + 300).c_str(), &TREE_pyramid_count, 1, 1, 100000); ImGui::NextColumn();

			if (summon == true) {
				const Vector3 pos = Vector3(TREE_pyramid_pos[0], TREE_pyramid_pos[1] + 8, TREE_pyramid_pos[2]);
				const Vector3 size = Vector3(1, 2, 1);


				Gameobject* Twist = Gameobject_manager::create("Twist");
				GOs.emplace_back(Twist);
				Twist->transform->local_pos = pos;

				Collider* save_coll = nullptr;
				for (int sphere_num = 0; sphere_num < TREE_pyramid_count; sphere_num++) {

					Gameobject* GO = Gameobject_manager::createCube("part");
					GO->transform->local_scale = size;
					GO->transform->local_orient = quaternion_from_euler(90, 0, 0);
					GO->transform->local_pos = Vector3(size.x * sphere_num * 2, size.y, 0);

					Collider* coll = GO->addComponent<Collider>();
					coll->add_shape<Capsule>();

					if (sphere_num == 0)coll->physics_data.is_moveable = false;

					if (save_coll != nullptr) {
						auto cone = Joint::add_Conetwistjoint(
							save_coll, coll,
							Vector3(0, -(size.y + 2), 0), Vector3(0, +(size.y + 2), 0),
							Vector3(0, -1, 0), Vector3(0, -1, 0),
							0.1f
						);

						cone->limit = 60;
					}

					save_coll = coll;
					GO->renderer->color = Vector4(1, 1, 0, 1);
					Twist->add_child(GO);

					GOs.emplace_back(GO);
				}
			}
			imgui_num++;

		}
		*/

		{}

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
				Human->transform->local_pos = Vector3(pos[0], pos[1] + 8, pos[2]);
				Human->update_world_trans_to_children();

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
				std::shared_ptr<Collider> Head_collider = Head->addComponent<Collider_comp>()->get_collider();
				std::shared_ptr<Collider> Rsholder_collider = Rsholder->addComponent<Collider_comp>()->get_collider();
				std::shared_ptr<Collider> Relbow_collider = Relbow->addComponent<Collider_comp>()->get_collider();
				std::shared_ptr<Collider> Lsholder_collider = Lsholder->addComponent<Collider_comp>()->get_collider();
				std::shared_ptr<Collider> Lelbow_collider = Lelbow->addComponent<Collider_comp>()->get_collider();
				std::shared_ptr<Collider> Body_collider = Body->addComponent<Collider_comp>()->get_collider();
				std::shared_ptr<Collider> Waist_collider = Waist->addComponent<Collider_comp>()->get_collider();
				std::shared_ptr<Collider> Rleg_collider = Rleg->addComponent<Collider_comp>()->get_collider();
				std::shared_ptr<Collider> Rfoot_collider = Rfoot->addComponent<Collider_comp>()->get_collider();
				std::shared_ptr<Collider> Lleg_collider = Lleg->addComponent<Collider_comp>()->get_collider();
				std::shared_ptr<Collider> Lfoot_collider = Lfoot->addComponent<Collider_comp>()->get_collider();

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

		////croth plane16
		//{
		//static int TREE_pyramid_count = 3;
		//static float pos[3] = { 0 };
		//static float scale[3] = { 1,1,1 };
		//bool summon = false;
		//ImGui::Separator();
		//ImGui::Text("Croth_plane02x"); ImGui::NextColumn();
		//ImGui::Checkbox(std::to_string(imgui_num + 100).c_str(), &summon); ImGui::NextColumn();
		//ImGui::DragFloat3(std::to_string(imgui_num + 200).c_str(), pos, 0.1f); ImGui::NextColumn();
		//ImGui::DragFloat3(std::to_string(imgui_num + 250).c_str(), scale, 0.1f); ImGui::NextColumn();
		//ImGui::DragInt(std::to_string(imgui_num + 300).c_str(), &TREE_pyramid_count, 1, 1, 100000); ImGui::NextColumn();

		//if (summon == true) {
		//	auto go = Gameobject_manager::createFromFBX("plane", "./DefaultModel/plane_02x02vertex.fbx");
		//	GOs.push_back(go);

		//	go->transform->local_pos = Vector3(pos[0], pos[1], pos[2]);
		//	go->transform->local_scale = Vector3(scale[0], scale[1], scale[2]);
		//	go->update_world_trans_to_children();

		//	auto coll = go->addComponent<Collider_croth_comp>();
		//	//coll->mesh_scale = Vector3(scale[0], scale[1], scale[2]);
		//	//coll->load_file("./DefaultModel/plane_16x16vertex.fbx", true, false);

		//	//Physics_data data;
		//	//data = coll->get_collider()->get_vertex_data(0, 259);
		//	//data.is_moveable = false;
		//	//coll->get_collider()->set_vertex_data(0, 259, data);
		//	//data = coll->get_collider()->get_vertex_data(0, 274);
		//	//data.is_moveable = false;
		//	//coll->get_collider()->set_vertex_data(0, 274, data);

		//	//for (int i = 259; i < 275; ++i) {
		//	//	data = coll->get_collider()->get_vertex_data(0, i);
		//	//	data.is_moveable = false;
		//	//	coll->get_collider()->set_vertex_data(0, i, data);
		//	//}


		//	auto renderer = go->addComponent<Croth_renderer>();
		//	renderer->set_meshoffset(coll->get_collider()->get_vertex_offset());

		//}
		//imgui_num++;
		//}
		////croth plane16
		//{
		//	static int TREE_pyramid_count = 3;
		//	static float pos[3] = { 0 };
		//	static float scale[3] = { 1,1,1 };
		//	bool summon = false;
		//	ImGui::Separator();
		//	ImGui::Text("Croth_plane16x"); ImGui::NextColumn();
		//	ImGui::Checkbox(std::to_string(imgui_num + 100).c_str(), &summon); ImGui::NextColumn();
		//	ImGui::DragFloat3(std::to_string(imgui_num + 200).c_str(), pos, 0.1f); ImGui::NextColumn();
		//	ImGui::DragFloat3(std::to_string(imgui_num + 250).c_str(), scale, 0.1f); ImGui::NextColumn();
		//	ImGui::DragInt(std::to_string(imgui_num + 300).c_str(), &TREE_pyramid_count, 1, 1, 100000); ImGui::NextColumn();

		//	if (summon == true) {
		//		auto go = Gameobject_manager::createFromFBX("plane", "./DefaultModel/plane_16x16vertex.fbx");
		//		GOs.push_back(go);

		//		go->transform->local_pos = Vector3(pos[0], pos[1], pos[2]);
		//		go->transform->local_scale = Vector3(scale[0], scale[1], scale[2]);
		//		go->update_world_trans_to_children();

		//		auto coll = go->addComponent<Collider_croth_comp>();
		//		//coll->mesh_scale = Vector3(scale[0], scale[1], scale[2]);
		//		//coll->load_file("./DefaultModel/plane_16x16vertex.fbx", true, false);

		//		Physics_data data;
		//		data = coll->get_collider()->get_vertex_data(0, 259);
		//		data.is_moveable = false;
		//		coll->get_collider()->set_vertex_data(0, 259, data);
		//		data = coll->get_collider()->get_vertex_data(0, 274);
		//		data.is_moveable = false;
		//		coll->get_collider()->set_vertex_data(0, 274, data);

		//		for (int i = 259; i < 275; ++i) {
		//			data = coll->get_collider()->get_vertex_data(0, i);
		//			data.is_moveable = false;
		//			coll->get_collider()->set_vertex_data(0, i, data);
		//		}


		//		auto renderer = go->addComponent<Croth_renderer>();
		//		renderer->set_meshoffset(coll->get_collider()->get_vertex_offset());

		//	}
		//	imgui_num++;
		//}



		ImGui::Columns(1);
		ImGui::End();
	}

#pragma endregion
}
