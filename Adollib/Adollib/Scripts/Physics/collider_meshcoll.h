#pragma once

#include "collider__base.h"
#include "meshcoll_resorce_manager.h"
#include "ALP__meshcoll_data.h"

namespace Adollib {

	//Mesh colliderクラス
	class Meshcoll : public Collider {
	public:
		Vector3 center;
		Vector3 rotate;
		Vector3 size;

	private:
		std::vector<Physics_function::Meshcollider_mesh>* collider_mesh = nullptr;
		std::string FBX_pass;

	public:
		//不動オブジェクトとして生成
		Meshcoll() :center(Vector3(0)), rotate(Vector3(0)), size(1) { name = std::string("Meshcoll"); }

		void load_mesh(const char* filename) {
			Physics_function::Collider_ResourceManager::CreateMCFromFBX(filename, &collider_mesh);
			FBX_pass = filename;

			ALPcollider_itr->collider_meshes.clear();

			int coll_mesn_size = collider_mesh->size();

			Physics_function::ALP_Collider_mesh coll_mesh;
			coll_mesh.ALPcollider = ALPcollider_itr;
			auto itr = collider_mesh->begin();
			for(int i = 0;i < coll_mesn_size;i++){
				coll_mesh.mesh = itr;
				ALPcollider_itr->collider_meshes.emplace_back(coll_mesh);
				itr++;
			}

		}

		Physics_function::Collider_data get_Colliderdata() const override {
			Physics_function::Collider_data ret;

			ret.local_position = center;
			ret.local_orientation = quaternion_from_euler(rotate);
			ret.local_scale = size;

			ret.shape = Physics_function::ALP_Collider_shape::Mesh;

			Physics_function::Meshcoll_data data;
			//data.collider_mesh = collider_mesh;

			ret.meshcoll_data = data;
			return ret;
		};

		void Update_hierarchy() override {
			float ave = (transform->local_scale.x + transform->local_scale.y + transform->local_scale.z) * 0.333333333f;
			//position
			{
				float vec3[3] = { center.x,center.y,center.z };
				ImGui::DragFloat3("center", vec3, ave * 0.001f, 0, 0, "%.2f");
				center = Vector3(vec3[0], vec3[1], vec3[2]);
			}
			//rotate
			{
				float vec3[3] = { rotate.x, rotate.y, rotate.z };
				ImGui::DragFloat3("rotate", vec3, 0.1f, 0, 0, "%.2f");
				rotate = Vector3(vec3[0], vec3[1], vec3[2]);
			}
			//scale
			{
				float vec3[3] = { size.x,size.y, size.z };
				ImGui::DragFloat3("size", vec3, ave * 0.001f, 0, 0, "%.2f");
				size = Vector3(vec3[0], vec3[1], vec3[2]);
			}

			if (ImGui::BeginTabBar("Physics")) {
				ImGui::Checkbox("moveable", &physics_data.is_moveable);
				ImGui::Checkbox("fallable", &physics_data.is_fallable);
				ImGui::Checkbox("kinematic", &physics_data.is_kinematic);
				ImGui::Checkbox("hitable", &physics_data.is_hitable);

				ImGui::DragFloat("mass", &physics_data.inertial_mass, 0.001f, 0, 0);

				ImGui::DragFloat("restitution", &physics_data.restitution, 0.001f, 0, 0);

				ImGui::DragFloat("friction", &physics_data.dynamic_friction, 0.001f, 0, 100000000.f);

				ImGui::EndTabBar();
			}
		};

	};
}