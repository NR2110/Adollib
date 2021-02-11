#pragma once
#include "collider__base.h"

namespace Adollib {
	//平面用クラス
	class Plane : public Collider {
	public:
		Vector3 rotate; //平面の回転
		float distance; //平面の距離

		//不動オブジェクトとして生成
		Plane():rotate(Vector3(0,0,0)),distance(0)
		{
			name = std::string("Plane");
			physics_data.is_moveable = false;
			physics_data.inertial_mass = FLT_MAX;
		}

		Physics_function::Collider_data get_Colliderdata() const override {
			Physics_function::Collider_data ret;

			Quaternion orient = quaternion_from_euler(rotate);
			ret.local_orientation = orient;
			ret.local_position = vector3_quatrotate(Vector3(0,1,0), orient) * distance;
			ret.local_scale = Vector3(FLT_MAX, 0, FLT_MAX);

			ret.half_size = Vector3(1, 1, 1);

			ret.shape = Physics_function::ALP_Collider_shape::Plane;

			ret.tag = tag;
			ret.nohit_tag = nohit_tag;

			return ret;
		}

		void Update_hierarchy() override {
			//rotate
			{
				float vec3[3] = { rotate.x,rotate.y,rotate.z };
				ImGui::DragFloat3("rotate", vec3, 0.1f, 0, 0, "%.2f");
				rotate = Vector3(vec3[0], vec3[1], vec3[2]);
			}
			{
				ImGui::DragFloat3("distance", &distance, 0.1f, 0, 0, "%.2f");
			}

			if (ImGui::BeginTabBar("Physics")) {
				ImGui::Checkbox("moveable", &physics_data.is_moveable);
				ImGui::Checkbox("fallable", &physics_data.is_fallable);
				ImGui::Checkbox("kinematic", &physics_data.is_kinematic);
				ImGui::Checkbox("is_kinmatic_anglar", &physics_data.is_kinmatic_anglar);
				ImGui::Checkbox("is_kinmatic_linear", &physics_data.is_kinmatic_linear);
				ImGui::Checkbox("hitable", &physics_data.is_hitable);

				ImGui::DragFloat("mass", &physics_data.inertial_mass, 0.001f, 0, 0);

				ImGui::DragFloat("restitution", &physics_data.restitution, 0.001f, 0, 0);

				ImGui::DragFloat("friction", &physics_data.dynamic_friction, 0.001f, 0, 100000000.f);

				ImGui::EndTabBar();
			}
		}
	};

}