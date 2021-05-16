

#include "object_manager.h"
#include "../Adollib/Scripts/Object/gameobject_manager.h"
#include "../Adollib/Scripts/Scene/scene_manager.h"

#include "../Adollib/Scripts/Math/closest_func.h"
#include "../Adollib/Scripts/Imgui/imgui_all.h"
#include "../Adollib/Scripts/Physics/ALP__physics_manager.h"

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
		camera = Gameobject_manager::find_camera("camera");

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
			Teach
		};

		Stage_types stage_type = Stage_types::Mesh_Cube;
		Gameobject* GO = nullptr;

		if (stage_type == Stage_types::Cube) {
			{
				GO = Gameobject_manager::createCube("stage");
				GO->transform->local_pos = Vector3(0, -60, 0);
				GO->transform->local_scale = Vector3(60, 60, 60);
				//GO->transform->local_orient = quaternion_from_euler(0, 45, 0);
				GO->material->color = Vector4(1, 1, 1, 1);

				Box* R = GO->addComponent<Box>();
				//Meshcoll* R = GO->addComponent<Meshcoll>();
				//R->load_mesh("./DefaultModel/cube.fbx");
				R->physics_data.is_moveable = false;
				R->is_static = true;
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

				Meshcoll* R = GO->addComponent<Meshcoll>();
				R->load_mesh("../Adollib/DefaultModel/cube.fbx",true);
				//Meshcoll* R = GO->addComponent<Meshcoll>();
				//R->load_mesh("./DefaultModel/cube.fbx");
				R->physics_data.is_moveable = false;
				R->is_static = true;
			}

		}
		else if (stage_type == Stage_types::Sphere) {
			{
				GO = Gameobject_manager::createSphere("stage");
				GO->transform->local_pos = Vector3(0, -6000, 0);
				GO->transform->local_scale = Vector3(6000, 6000, 6000);
				GO->material->color = Vector4(1, 1, 1, 1);
				Sphere* R = GO->addComponent<Sphere>();
				//R->center = Vector3(1, 1, 1);
				//R->load_mesh("../Adollib/DefaultModel/sphere.fbx");
				R->physics_data.is_moveable = false;

			}

		}
		else if (stage_type == Stage_types::Capsule) {
			{
				GO = Gameobject_manager::createCapsule("stage");
				GO->transform->local_pos = Vector3(0, -6000, 0);
				GO->transform->local_scale = Vector3(6000, 6000, 6000);
				GO->material->color = Vector4(1, 1, 1, 1);
				Sphere* R = GO->addComponent<Sphere>();
				//R->center = Vector3(1, 1, 1);
				//R->load_mesh("../Adollib/DefaultModel/sphere.fbx");
				R->physics_data.is_moveable = false;

			}

		}
		else if (stage_type == Stage_types::Shaclo) {
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
				Meshcoll* R = GO->addComponent<Meshcoll>();
				R->load_mesh("../Data/FBX/stage_col.fbx",true);
				//R->load_mesh("../Adollib/DefaultModel/cylinder.fbx");
				R->physics_data.is_moveable = false;
				R->is_static = true;

			}

		}
		else if (stage_type == Stage_types::Teach) {
			{
				GO = Gameobject_manager::createFromFBX("../Data/FBX/0311_collisions.fbx");
				//objGO = Gameobject_manager::create();
				//objGO = Gameobject_manager::createFromFBX("../Adollib/DefaultModel/cylinder.fbx");
				GO->transform->local_pos = Vector3(-10, 15, -25);
				//objGO->transform->local_scale = Vector3(0.01f, 0.02f, 0.03f);
				GO->transform->local_scale = Vector3(0.01, 0.01, 0.01);
				//objGO->transform->local_orient = quaternion_from_euler(45, 45, 45);
				GO->transform->local_orient = quaternion_from_euler(0, 180, 0);
				Meshcoll* R = GO->addComponent<Meshcoll>();
				R->load_mesh("../Data/FBX/0311_collisions.fbx",true);
				//R->load_mesh("../Adollib/DefaultModel/cylinder.fbx");
				R->physics_data.is_moveable = false;
				R->is_static = true;
			}
		}

	}

	// 毎フレーム呼ばれる更新処理
	void object_manager::update()
	{
		ImGuiWindowFlags flag = 0;

		//flag |= ImGuiWindowFlags_AlwaysAutoResize;

#pragma region IMGUI

		if (ImGui::Begin("object_manage", 0, flag)) {

			ImGui::InputFloat3(std::string("clear_color").c_str(), &Systems::clear_color[0], "%.3f"); ImGui::NextColumn();
			ImGui::Checkbox("is_draw_raycast", &is_draw_raycast);

			//objectの削除
			bool del = false;
			ImGui::Checkbox("delete", &del);
			if (del) {
				for (auto& GO : GOs) {
					GO->active = false;
					GO->clearComponent();
					boxes.clear();
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
					Gameobject* pearent =  Gameobject_manager::create("BOXpyramid");
					for (int i = 0; i < BOX_pyramid_count; i++) {
						for (int o = 0; o < BOX_pyramid_count - i; o++) {
							pearent->add_child(set_box(Vector3(
								BOX_pyramid_size[0] * 2.0f * 1.2f * o - (BOX_pyramid_count - i) * 2.300001f / 2.0f + BOX_pyramid_pos[0],
								3.0f + BOX_pyramid_size[1] * 2 * i + BOX_pyramid_pos[1],
								BOX_pyramid_pos[2]),
								Vector3(BOX_pyramid_size[0], BOX_pyramid_size[1], BOX_pyramid_size[2]), Vector3(0, 10, 0), Vector3(0, 1, 1))
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
				bool summon = false;
				ImGui::Separator();
				ImGui::Text("SPHERE_pyramid"); ImGui::NextColumn();
				ImGui::Checkbox(std::to_string(imgui_num + 100).c_str(), &summon); ImGui::NextColumn();
				ImGui::DragFloat3(std::to_string(imgui_num + 200).c_str(), SPHERE_pyramid_pos, 0.1f); ImGui::NextColumn(); ImGui::NextColumn();
				ImGui::DragInt(std::to_string(imgui_num + 300).c_str(), &SPHERE_pyramid_count, 1, 1, 100000); ImGui::NextColumn();

				if (summon == true)
					for (int i = 0; i < SPHERE_pyramid_count; i++) {
						for (int o = 0; o < SPHERE_pyramid_count - i; o++) {
							set_sphere(Vector3(2.50001f * o - (SPHERE_pyramid_count - i) * 2.500001f / 2.0f + SPHERE_pyramid_pos[0], 5.0f + 2.50001f * i + SPHERE_pyramid_pos[1], SPHERE_pyramid_pos[2]), 1, Vector3(0, 1, 1));
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

			//MIXpyramid
			{
				static int MIX_pyramid_count = 5;
				static float MIX_pyramid_pos[3] = { 0 };
				bool summon = false;
				ImGui::Separator();
				ImGui::Text("MIX_pyramid"); ImGui::NextColumn();
				ImGui::Checkbox(std::to_string(imgui_num + 100).c_str(), &summon); ImGui::NextColumn();
				ImGui::DragFloat3(std::to_string(imgui_num + 200).c_str(), MIX_pyramid_pos, 0.1f); ImGui::NextColumn(); ImGui::NextColumn();
				ImGui::DragInt(std::to_string(imgui_num + 300).c_str(), &MIX_pyramid_count, 1, 1, 100000); ImGui::NextColumn();

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

			//SPHEREplane
			{
				static int SPHERE_plane_count = 5;
				static float SPHERE_plane_pos[3] = { 0 };
				bool summon = false;
				ImGui::Separator();
				ImGui::Text("SPHERE_plane"); ImGui::NextColumn();
				ImGui::Checkbox(std::to_string(imgui_num + 100).c_str(), &summon); ImGui::NextColumn();
				ImGui::DragFloat3(std::to_string(imgui_num + 200).c_str(), SPHERE_plane_pos, 0.1f); ImGui::NextColumn(); ImGui::NextColumn();
				ImGui::DragInt(std::to_string(imgui_num + 300).c_str(), &SPHERE_plane_count, 1, 1, 100000); ImGui::NextColumn();

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
	Gameobject* object_manager::set_sphere(Vector3 pos, float r, Vector3 color) {
		Gameobject* object = nullptr;
		object = Gameobject_manager::createSphere(GO_tag::Sphere);
		//Vector4 C = Vector4(color.x, color.y, color.z, 1);
		Vector4 C = Vector4(1, 0, 1, 1);
		object->material->color = C;

		//object->addComponent<object_fall>();
		object->transform->local_pos = pos;
		object->transform->local_scale = Vector3(r, r, r);


		Collider* coll = object->addComponent<Sphere>();
		coll->tag = Collider_tags::Sphere;
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
		M->tag = Collider_tags::Box;

		GOs.emplace_back(object);
		boxes.emplace_back(M);
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

		Capsule* M = object->addComponent<Capsule>();
		M->r = r;
		M->length = length;
		//Meshcoll* M = object->addComponent<Meshcoll>();
		//M->load_mesh("./DefaultModel/cube.fbx");
		//M->inertial_mass = 1;
		M->tag = Collider_tags::Cylinder;
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