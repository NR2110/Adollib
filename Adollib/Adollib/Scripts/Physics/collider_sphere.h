#pragma once
#include "collider__base.h"

namespace Adollib {

	//球体用クラス
	class Sphere : public ALP_shape {
	public:
		Vector3 center; //中心座標
		float r = 1; //半径

		Sphere() : center(Vector3(0)), r(1) { shape_tag = Physics_function::ALP_Collider_shape_tag::Sphere; };

		void adapt_Colliderdata() override {
			local_position = center;
			local_orientation = quaternion_identity();
			local_scale = Vector3(r);
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
		};


		void update_dop14() override {
			dop14.pos = world_position();
			for (int i = 0; i < DOP::DOP14_size; i++) {
				dop14.max[i] = +world_scale().x * 1.0000001f;
				dop14.min[i] = -world_scale().x * 1.0000001f;
			}

		};


	};

}