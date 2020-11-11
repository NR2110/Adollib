

#include "object_manager.h"
#include "../Adollib/Scripts/Object/gameobject_manager.h"
#include "../Adollib/Scripts/Scene/scene_manager.h"

#include "../Adollib/Scripts/Math/closest_func.h"

#include "../Adollib/Scripts/Imgui/imgui_all.h"

#include "../Adollib/Scripts/Physics/ALP__physics_manager.h"

namespace Adollib
{
	// 所属するシーンの初期化時に一度だけ呼ばれる
	void object_manager::awake() {

	}

	void object_manager::start()
	{
#if 1
		//set_sphere(n_vector3(0, 0, 50), 10);
		//set_sphere(n_vector3(0, 0, 0), 10);
		//set_moveable_sphere(n_vector3(0, 30, 0), 5);
		//set_moveable_sphere(n_vector3(0, 50, 0), 0.1);

		//set_plane(Vector3(0, 0, 0), Vector3(0, 1, 0), Vector3(1, 0, 1));
		//	set_nohit_plane(n_vector3(1000, -1, 1000), n_vector3(0, 1, 0), n_vector3(1, 0, 1));
			//set_moveable_box(n_vector3(0, 200, 0), n_vector3(10, 10, 10), n_vector3(45, 45, 0), n_vector3(0, 1, 1));
			//set_moveable_box(n_vector3(0, 200, 0), n_vector3(10, 10, 10), n_vector3(0, 0, 0), n_vector3(0, 1, 1));
		if (0) {
			{
				Gameobject* GO = Gameobject_manager::createCube();
				GO->transform->local_pos = Vector3(0, -60, 0);
				GO->transform->local_scale = Vector3(60, 60, 60);
				GO->transform->local_orient = quaternion_from_euler(0, 0, 0);
				GO->material->color = Vector4(1, 1, 1, 1);

				Box* R = GO->addComponent<Box>();
				//Meshcoll* R = GO->addComponent<Meshcoll>();
				//R->load_mesh("./DefaultModel/cube.fbx");
				R->is_moveable = false;
			}

			{
				//Gameobject* GO = Gameobject_manager::createCube();
				//GO->transform->local_pos = Vector3(0, -90, 0.5);
				//GO->transform->local_scale = Vector3(60, 60, 60);
				//GO->transform->local_orient = quaternion_from_euler(45, 0, 0);
				//GO->material->color = Vector4(1, 1, 1, 1);

				//Box* R = GO->addComponent<Box>();
				//R->is_moveable = false;

				////Box* R = GO->addComponent<Box>();
				//Meshcoll* M = GO->addComponent<Meshcoll>();
				//M->load_mesh("./DefaultModel/cube.fbx");
				//M->is_moveable = false;
			}
		}
		else {
			{
				Gameobject* GO = Gameobject_manager::createFromFBX("../Data/FBX/0311_collisions.fbx");
				GO->transform->local_pos = Vector3(-10, 15, -25);
				GO->transform->local_scale = Vector3(0.001f, 0.001f, 0.001f);
				GO->transform->local_orient = quaternion_from_euler(0, 180, 0);
				Meshcoll* R = GO->addComponent<Meshcoll>();
				R->load_mesh("../Data/FBX/0311_collisions.fbx");
				R->is_moveable = false;

			}

			{
				//Gameobject* GO = Gameobject_manager::createCube();
				//GO->transform->local_pos = Vector3(0, -60, 0);
				//GO->transform->local_scale = Vector3(60, 60, 60);
				//GO->transform->local_orient = quaternion_from_euler(0, 0, 0);
				//GO->material->color = Vector4(1, 1, 1, 1);

			}
		}


#else
#endif


	}

	// 毎フレーム呼ばれる更新処理
	void object_manager::update()
	{
		ImGuiWindowFlags flag = 0;
		//flag |= ImGuiWindowFlags_AlwaysAutoResize;


		if(ImGui::Begin("object_manage", 0, flag)) {

			//objectの削除
			bool del = false;
			ImGui::Checkbox("delete", &del); 
			if (del) {
				for (u_int i = 0; i < GOs.size(); i++) {
					GOs[i]->active = false;
					GOs[i]->clearComponent();
				}
				GOs.clear();
			}

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
							set_box(Vector3(2.00001f * o - (BOX_pyramid_count - i) * 2.000001f / 2.0f + BOX_pyramid_pos[0], 3.0f + 2.00001f * i + BOX_pyramid_pos[1], BOX_pyramid_pos[2]), Vector3(1, 1, 1), Vector3(0, 0, 0), Vector3(0, 1, 1));
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
							set_sphere(Vector3(2.50001f * o - (SPHERE_pyramid_count - i) * 2.500001f / 2.0f + SPHERE_pyramid_pos[0], 5.0f + 2.50001f * i + SPHERE_pyramid_pos[1], SPHERE_pyramid_pos[2]), 1, Vector3(0, 1, 1));
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
								set_box(Vector3(2.50001f * o - (MIX_pyramid_count - i) * 2.500001f / 2.0f + MIX_pyramid_pos[0], 5.0f + 2.50001f * i + MIX_pyramid_pos[1], MIX_pyramid_pos[2]), Vector3(1, 1, 1), Vector3(30, 0, 30), Vector3(0, 1, 1));
							else
								set_sphere(Vector3(2.50001f * o - (MIX_pyramid_count - i) * 2.500001f / 2.0f + MIX_pyramid_pos[0], 5.0f + 2.50001f * i + MIX_pyramid_pos[1], MIX_pyramid_pos[2]), 1, Vector3(0, 1, 1));
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
							set_meshbox(Vector3(2.00001f * o - (Mesh_pyramid_count - i) * 2.000001f / 2.0f + Mesh_pyramid_pos[0], 3.0f + 2.00001f * i + Mesh_pyramid_pos[1], Mesh_pyramid_pos[2]), Vector3(1, 1, 1), Vector3(180, 0, 0), Vector3(0, 1, 1));
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

							set_sphere(Vector3(2.50001f * i - (SPHERE_plane_count - i) * 5.000001f / 2.0f + SPHERE_plane_pos[0], 10 + SPHERE_plane_pos[1], 2.50001f * o - (SPHERE_plane_count - o) * 2.500001f / 2.0f + SPHERE_plane_pos[2]), 1, Vector3(0, 1, 1));
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
				object = Gameobject_manager::createCube(GO_tag::Mesh);
				Vector4 C = Vector4(1,0,0,1);
				object->material->color = C;

				object->transform->local_orient = quaternion_from_euler(0,0,0);
				object->transform->local_pos = Vector3(0,5,0);
				object->transform->local_scale = Vector3(1,1,1);

				//Box* M = object->addComponent<Box>();
				Meshcoll* M = object->addComponent<Meshcoll>();
				M->load_mesh("./DefaultModel/cube.fbx");
				GOs.emplace_back(object);


			}

		}


		if (input->getKeyTrigger(Key::N)) {

			{ 

				Gameobject* object = nullptr;
				object = Gameobject_manager::createCube(GO_tag::Box);
				Vector4 C = Vector4(0, 1, 0, 1);
				object->material->color = C;

				object->transform->local_orient = quaternion_from_euler(0, 0, 0);
				object->transform->local_pos = Vector3(0, 5, 0);
				object->transform->local_scale = Vector3(1, 1, 1);

				Box* M = object->addComponent<Box>();
				GOs.emplace_back(object);

			}

		}

		if (input->getKeyTrigger(Key::B)) {

			{

				Gameobject* object = nullptr;
				object = Gameobject_manager::createCube(GO_tag::Box);
				Vector4 C = Vector4(0, 1, 0, 1);
				object->material->color = C;

				//object->addComponent<object_fall>();
				object->transform->local_orient = quaternion_from_euler(0, 0, 45);
				object->transform->local_pos = Vector3(0, 5, 0);
				object->transform->local_scale = Vector3(1, 1, 1);

				Box* M = object->addComponent<Box>();
				GOs.emplace_back(object);


			}

			{

				Gameobject* object = nullptr;
				object = Gameobject_manager::createCube(GO_tag::Mesh);
				Vector4 C = Vector4(1, 0, 0, 1);
				object->material->color = C;

				//object->addComponent<object_fall>();
				object->transform->local_orient = quaternion_from_euler(0, 0, 45);
				object->transform->local_pos = Vector3(0, 5, 0);
				object->transform->local_scale = Vector3(1, 1, 1);

				//Box* M = object->addComponent<Box>();
				Meshcoll* M = object->addComponent<Meshcoll>();
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
	Gameobject* object_manager::set_sphere(Vector3 pos, float r, Vector3 color) {
		Gameobject* object = nullptr;
		object = Gameobject_manager::createSphere(GO_tag::Sphere);
		Vector4 C = Vector4(color.x, color.y, color.z, 1);
		object->material->color = C;

		//object->addComponent<object_fall>();
		object->transform->local_pos = pos;
		object->transform->local_scale = Vector3(r, r, r);

		Collider* coll = object->addComponent<Sphere>();
		GOs.emplace_back(object);
		return object;
	}

	Gameobject* object_manager::set_box(Vector3 pos, Vector3 size, Vector3 rotate, Vector3 color) {
		Gameobject* object = nullptr;
		object = Gameobject_manager::createCube(GO_tag::Box);
		Vector4 C = Vector4(color.x, color.y, color.z, 1);
		object->material->color = C;

		//object->addComponent<object_fall>();
		object->transform->local_orient = quaternion_from_euler(rotate);
		object->transform->local_pos = pos;
		object->transform->local_scale = size;

		Box* M = object->addComponent<Box>();
		//Meshcoll* M = object->addComponent<Meshcoll>();
		//M->load_mesh("./DefaultModel/cube.fbx");
		//M->inertial_mass = 1;
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
		Meshcoll* M = object->addComponent<Meshcoll>();
		M->load_mesh("./DefaultModel/cone.fbx");
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

		Collider* coll = object->addComponent<Plane>();
		GOs.emplace_back(object);
		return object;
	}


}