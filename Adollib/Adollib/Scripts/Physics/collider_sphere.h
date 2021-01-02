#pragma once
#include "collider__base.h"

namespace Adollib {

	//球体用クラス
	class Sphere : public Collider {
	public:
		Vector3 center; //中心座標
		float r = 1; //半径

		Sphere() : center(Vector3(0)), r(1) { name = std::string("Sphere"); };

		Physics_function::Collider_data get_Colliderdata() const override {
			Physics_function::Collider_data ret;

			ret.local_position = center;
			ret.local_orientation = quaternion_identity();
			ret.local_scale = Vector3(r);

			ret.half_size = Vector3(1, 1, 1);

			ret.shape = Physics_function::ALP_Collider_shape::Sphere;

			return ret;
		};

		void Update_hierarchy() override {
			float ave = (r);
			//position
			{
				float vec3[3] = { center.x,center.y,center.z };
				ImGui::DragFloat3("center", vec3, ave * 0.001f, 0, 0, "%.2f");
				center = Vector3(vec3[0], vec3[1], vec3[2]);
			}
			//scale
			{
				ImGui::DragFloat3("radias", &r, ave * 0.001f, 0, 0, "%.2f");
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
		};

	};

}