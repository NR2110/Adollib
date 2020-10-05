
#include "../Adollib/gameobject_manager.h"

#include "object_manager.h"
#include "object_fall.h"
#include "../Adollib/scene_manager.h"

#include "../Adollib/closest_func.h"

#include "../Adollib/imgui_all.h"

#include "../Adollib/physics_global.h"
namespace Adollib
{
	// 所属するシーンの初期化時に一度だけ呼ばれる
	void object_manager::awake() {

	}

	void object_manager::start()
	{
		Gameobject* GO = Gameobject_manager::createCube();
		GO->transform->local_pos = vector3(0, -80, 50);
		GO->transform->local_scale = vector3(220, 60, 120);
		GO->transform->local_orient = quaternion_from_euler(0, 0, 0);
		GO->material->color = vector4(1, 1, 1, 1);

		Box* R = GO->add_collider<Box>();
		//Meshcoll* R = GO->add_collider<Meshcoll>();
		//R->load_mesh("./DefaultModel/cube.fbx");
		R->move = false;

	}

	// 毎フレーム呼ばれる更新処理
	void object_manager::update()
	{
		ImGuiWindowFlags flag = 0;
		//flag |= ImGuiWindowFlags_AlwaysAutoResize;


		if(ImGui::Begin("object_manage", 0, flag)) {

			bool del = false;
			ImGui::Checkbox("delete", &del); //objectの削除

			if (del) {
				for (int i = 0; i < GOs.size(); i++) {
					GOs[i]->clear_collider();
					GOs[i]->active = false;
				}
				GOs.clear();
			}

			//重力の調整
			//ImGui::InputFloat("gravity", &physics_g::gravity, 0.001f, 1.0f, "%.3f");

			//正確さの調整
			ImGui::InputInt("accuracy", &physics_g::accuracy, 1, 200);

			//貫通時のばねの強さ
			ImGui::InputFloat("bias", &physics_g::bias, 0.01, 0.1, "%.3f");
			//貫通許容誤差
			ImGui::InputFloat("slop", &physics_g::slop, 0.0001, 0.001, "%.4f");

			ImGui::Columns(4, "columnListIDs");
			ImGui::Separator();
			ImGui::Text("NAME"); ImGui::NextColumn();
			ImGui::Text("SUMMON"); ImGui::NextColumn();
			ImGui::Text("POSITION"); ImGui::NextColumn();
			ImGui::Text("COUNT"); ImGui::NextColumn();
			ImGui::Separator();

			int imgui_num = 0;
			//BOXpyramid
			{
				static int BOX_pyramid_count = 5;
				static float BOX_pyramid_pos[3] = { 0 };
				bool summon = false;
				ImGui::Separator();
				ImGui::Text("BOX_pyramid"); ImGui::NextColumn();
				ImGui::Checkbox(std::to_string(imgui_num + 100).c_str(), &summon); ImGui::NextColumn();
				ImGui::InputFloat3(std::to_string(imgui_num + 200).c_str(), BOX_pyramid_pos, "%.3f"); ImGui::NextColumn();
				ImGui::InputInt(std::to_string(imgui_num + 300).c_str(), &BOX_pyramid_count, 1, 20); ImGui::NextColumn();

				if (summon == true)
					for (int i = 0; i < BOX_pyramid_count; i++) {
						for (int o = 0; o < BOX_pyramid_count - i; o++) {
							set_box(vector3(2.00001 * o - (BOX_pyramid_count - i) * 2.000001 / 2.0 + BOX_pyramid_pos[0], 3.0 + 2.00001 * i + BOX_pyramid_pos[1], BOX_pyramid_pos[2]), vector3(1, 1, 1), vector3(0, 0, 0), vector3(0, 1, 1));
						}

					}

				imgui_num++;
			}

			//SPHEREpyramid
			{
				static int SPHERE_pyramid_count = 5;
				static float SPHERE_pyramid_pos[3] = { 0 };
				bool summon = false;
				ImGui::Separator();
				ImGui::Text("SPHERE_pyramid"); ImGui::NextColumn();
				ImGui::Checkbox(std::to_string(imgui_num + 100).c_str(), &summon); ImGui::NextColumn();
				ImGui::InputFloat3(std::to_string(imgui_num + 200).c_str(), SPHERE_pyramid_pos, "%.3f"); ImGui::NextColumn();
				ImGui::InputInt(std::to_string(imgui_num + 300).c_str(), &SPHERE_pyramid_count, 1, 20); ImGui::NextColumn();

				if (summon == true)
					for (int i = 0; i < SPHERE_pyramid_count; i++) {
						for (int o = 0; o < SPHERE_pyramid_count - i; o++) {
							set_sphere(vector3(2.50001 * o - (SPHERE_pyramid_count - i) * 2.500001 / 2.0 + SPHERE_pyramid_pos[0], 5.0 + 2.50001 * i + SPHERE_pyramid_pos[1], SPHERE_pyramid_pos[2]), 1, vector3(0, 1, 1));
						}

					}
				imgui_num++;
			}

			//MIXpyramid
			{
				static int MIX_pyramid_count = 5;
				static float MIX_pyramid_pos[3] = { 0 };
				bool summon = false;
				ImGui::Separator();
				ImGui::Text("MIX_pyramid"); ImGui::NextColumn();
				ImGui::Checkbox(std::to_string(imgui_num + 100).c_str(), &summon); ImGui::NextColumn();
				ImGui::InputFloat3(std::to_string(imgui_num + 200).c_str(), MIX_pyramid_pos, "%.3f"); ImGui::NextColumn();
				ImGui::InputInt(std::to_string(imgui_num + 300).c_str(), &MIX_pyramid_count, 1, 20); ImGui::NextColumn();

				if (summon == true)
					for (int i = 0; i < MIX_pyramid_count; i++) {
						for (int o = 0; o < MIX_pyramid_count - i; o++) {
							if ((i + o) % 2 == 0)
								set_box(vector3(2.50001 * o - (MIX_pyramid_count - i) * 2.500001 / 2.0 + MIX_pyramid_pos[0], 5.0 + 2.50001 * i + MIX_pyramid_pos[1], MIX_pyramid_pos[2]), vector3(1, 1, 1), vector3(30, 0, 30), vector3(0, 1, 1));
							else
								set_sphere(vector3(2.50001 * o - (MIX_pyramid_count - i) * 2.500001 / 2.0 + MIX_pyramid_pos[0], 5.0 + 2.50001 * i + MIX_pyramid_pos[1], MIX_pyramid_pos[2]), 1, vector3(0, 1, 1));
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
				ImGui::InputFloat3(std::to_string(imgui_num + 200).c_str(), Mesh_pyramid_pos, "%.3f"); ImGui::NextColumn();
				ImGui::InputInt(std::to_string(imgui_num + 300).c_str(), &Mesh_pyramid_count, 1, 20); ImGui::NextColumn();

				if (summon == true)
					for (int i = 0; i < Mesh_pyramid_count; i++) {
						for (int o = 0; o < Mesh_pyramid_count - i; o++) {
							set_meshbox(vector3(2.00001 * o - (Mesh_pyramid_count - i) * 2.000001 / 2.0 + Mesh_pyramid_pos[0], 3.0 + 2.00001 * i + Mesh_pyramid_pos[1], Mesh_pyramid_pos[2]), vector3(1, 1, 1), vector3(180, 0, 0), vector3(0, 1, 1));
						}

					}
				imgui_num++;
			}

			//SPHEREplane
			{
				static int SPHERE_plane_count = 5;
				static float SPHERE_plane_pos[3] = { 0 };
				bool summon = false;
				ImGui::Separator();
				ImGui::Text("SPHERE_plane"); ImGui::NextColumn();
				ImGui::Checkbox(std::to_string(imgui_num + 100).c_str(), &summon); ImGui::NextColumn();
				ImGui::InputFloat3(std::to_string(imgui_num + 200).c_str(), SPHERE_plane_pos, "%.3f"); ImGui::NextColumn();
				ImGui::InputInt(std::to_string(imgui_num + 300).c_str(), &SPHERE_plane_count, 1, 20); ImGui::NextColumn();

				if (summon == true)
					for (int i = 0; i < SPHERE_plane_count; i++) {

						for (int o = 0; o < SPHERE_plane_count; o++) {

							set_sphere(vector3(2.50001 * i - (SPHERE_plane_count - i) * 5.000001 / 2.0 + SPHERE_plane_pos[0], 10 + SPHERE_plane_pos[1], 2.50001 * o - (SPHERE_plane_count - o) * 2.500001 / 2.0 + SPHERE_plane_pos[2]), 1, vector3(0, 1, 1));
						}

					}
				imgui_num++;
			}



			ImGui::Columns(1);
			ImGui::End();
		}
		
		if (input->getKeyTrigger(Key::M)) {

			{ 

				Gameobject* object = nullptr;
				object = Gameobject_manager::createCube("");
				vector4 C = vector4(1,0,0,1);
				object->material->color = C;

				object->transform->local_orient = quaternion_from_euler(0,0,0);
				object->transform->local_pos = vector3(0,5,0);
				object->transform->local_scale = vector3(1,1,1);

				//Box* M = object->add_collider<Box>();
				Meshcoll* M = object->add_collider<Meshcoll>();
				M->load_mesh("./DefaultModel/cube.fbx");
				GOs.emplace_back(object);


			}

		}


		if (input->getKeyTrigger(Key::N)) {

			{ 

				Gameobject* object = nullptr;
				object = Gameobject_manager::createCube("");
				vector4 C = vector4(0, 1, 0, 1);
				object->material->color = C;

				object->transform->local_orient = quaternion_from_euler(0, 0, 0);
				object->transform->local_pos = vector3(0, 5, 0);
				object->transform->local_scale = vector3(1, 1, 1);

				Box* M = object->add_collider<Box>();
				GOs.emplace_back(object);

			}

		}

		if (input->getKeyTrigger(Key::B)) {

			{

				Gameobject* object = nullptr;
				object = Gameobject_manager::createCube("");
				vector4 C = vector4(0, 1, 0, 1);
				object->material->color = C;

				//object->addComponent<object_fall>();
				object->transform->local_orient = quaternion_from_euler(0, 0, 45);
				object->transform->local_pos = vector3(0, 5, 0);
				object->transform->local_scale = vector3(1, 1, 1);

				Box* M = object->add_collider<Box>();
				GOs.emplace_back(object);


			}

			{

				Gameobject* object = nullptr;
				object = Gameobject_manager::createCube("");
				vector4 C = vector4(1, 0, 0, 1);
				object->material->color = C;

				//object->addComponent<object_fall>();
				object->transform->local_orient = quaternion_from_euler(0, 0, 45);
				object->transform->local_pos = vector3(0, 5, 0);
				object->transform->local_scale = vector3(1, 1, 1);

				//Box* M = object->add_collider<Box>();
				Meshcoll* M = object->add_collider<Meshcoll>();
				M->load_mesh("./DefaultModel/cube.fbx");
				GOs.emplace_back(object);


			}

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
	Gameobject* object_manager::set_sphere(vector3 pos, float r, vector3 color) {
		Gameobject* object = nullptr;
		object = Gameobject_manager::createSphere("");
		vector4 C = vector4(color.x, color.y, color.z, 1);
		object->material->color = C;

		//object->addComponent<object_fall>();
		object->transform->local_pos = pos;
		object->transform->local_scale = vector3(r, r, r);

		object->add_collider<Sphere>();
		GOs.emplace_back(object);
		return object;
	}

	Gameobject* object_manager::set_box(vector3 pos, vector3 size, vector3 rotate, vector3 color) {
		Gameobject* object = nullptr;
		object = Gameobject_manager::createCube("");
		vector4 C = vector4(color.x, color.y, color.z, 1);
		object->material->color = C;

		//object->addComponent<object_fall>();
		object->transform->local_orient = quaternion_from_euler(rotate);
		object->transform->local_pos = pos;
		object->transform->local_scale = size;

		Box* M = object->add_collider<Box>();
		//Meshcoll* M = object->add_collider<Meshcoll>();
		//M->load_mesh("./DefaultModel/cube.fbx");
		M->inertial_mass = 0.2;
		GOs.emplace_back(object);
		return object;
	}

	Gameobject* object_manager::set_meshbox(vector3 pos, vector3 size, vector3 rotate, vector3 color) {
		Gameobject* object = nullptr;
		object = Gameobject_manager::create("./DefaultModel/cone.fbx");
		vector4 C = vector4(color.x, color.y, color.z, 1);
		object->material->color = C;

		//object->addComponent<object_fall>();
		object->transform->local_orient = quaternion_from_euler(rotate);
		object->transform->local_pos = pos;
		object->transform->local_scale = size;

		//Box* M = object->add_collider<Box>();
		Meshcoll* M = object->add_collider<Meshcoll>();
		M->load_mesh("./DefaultModel/cone.fbx");
		//M->inertial_mass = 1;
		GOs.emplace_back(object);
		return object;
	}

	Gameobject* object_manager::set_plane(vector3 pos, vector3 normal, vector3 color) {
		Gameobject* object = nullptr;
		object = Gameobject_manager::createCube("");
		vector4 C = vector4(color.x, color.y, color.z, 1);
		object->material->color = C;

		object->transform->local_orient = quaternion_from_euler(normal);
		object->transform->local_pos = pos;
		object->transform->local_scale = vector3(30, 0.1, 30);

		object->add_collider<Plane>();
		GOs.emplace_back(object);
		return object;
	}

	//
	/*Gameobject* object_manager::set_fall_sphere(vector3 pos, float r, vector3 color) {
		Gameobject* object = nullptr;
		object = Gameobject_manager::createSphere("");
		vector4 C = vector4(color.x, color.y, color.z, 1);
		object->material->color = C;

		object->transform->local_pos = pos;
		object->transform->local_scale = vector3(r, r, r);

		Sphere* S = object->add_collider<Sphere>();

		object_fall* F = object->addComponent<object_fall>();
		F->collier = S;

		return object;
	}

	Gameobject* object_manager::set_fall_box(vector3 pos, vector3 size, vector3 rotate, vector3 color) {
		Gameobject* object = nullptr;
		object = Gameobject_manager::createCube("");
		vector4 C = vector4(color.x, color.y, color.z, 1);
		object->material->color = C;

		object->transform->local_orient = quaternion_from_euler(rotate);
		object->transform->local_pos = pos;
		object->transform->local_scale = size;

		Box* B = object->add_collider<Box>();
		B->inertial_mass = 1;

		object_fall* F = object->addComponent<object_fall>();
		F->collier = B;

		return object;
	}

	Gameobject* object_manager::set_nohit_sphere(vector3 pos, float r, vector3 color) {
		Gameobject* object = nullptr;
		object = Gameobject_manager::createSphere("");
		vector4 C = vector4(color.x, color.y, color.z, 1);
		object->material->color = C;

		object->transform->local_pos = pos;
		object->transform->local_scale = vector3(r, r, r);

		return object;
	}

	Gameobject* object_manager::set_nohit_box(vector3 pos, vector3 size, vector3 rotate, vector3 color) {
		Gameobject* object = nullptr;
		object = Gameobject_manager::createCube("");
		vector4 C = vector4(color.x, color.y, color.z, 1);
		object->material->color = C;

		object->transform->local_orient = quaternion_from_euler(rotate);
		object->transform->local_pos = pos;
		object->transform->local_scale = size;

		return object;
	}*/

	//Gameobject* object_manager::set_nohit_plane(vector3 pos, vector3 normal, vector3 color) {
	//	Gameobject* object = nullptr;
	//	object = Gameobject_manager::createCube("");
	//	vector4 C = vector4(color.x, color.y, color.z, 1);
	//	object->material->color = C;

	//	object->addComponent<object_fall>();
	//	object->transform->local_orient = quaternion_from_euler(normal);
	//	object->transform->local_pos = pos;
	//	object->transform->local_scale = vector3(30, 30, 0.1);

	//	std::vector<std::string> no;
	//	no.push_back("all");
	//	object->add_collider<Plane>(pos, normal, 1, "plane", no);

	//	return object;
	//}


}