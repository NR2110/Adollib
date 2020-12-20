#pragma once
#include "collider__base.h"

namespace Adollib {
	//Boxクラス
	class Box : public Collider {
	public:
		Vector3	center;//中心座標
		Vector3	rotate;//回転
		Vector3	size;//大きさ

		Box() : center(Vector3(0)), rotate(Vector3(0)), size(Vector3(1)) { name = std::string("Box"); };

		Physics_function::Collider_data get_Colliderdata() const override {
			Physics_function::Collider_data ret;

			ret.local_position = center;
			ret.local_orientation = quaternion_from_euler(rotate);
			ret.local_scale = size;

			ret.half_size = Vector3(1, 1, 1);

			ret.shape = Physics_function::ALP_Collider_shape::BOX;

			return ret;
		};

		void Update_hierarchy() override {
			float ave = (size.x + size.y + size.z) * 0.333333333f;
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