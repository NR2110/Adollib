#pragma once
#include "collider__base.h"

namespace Adollib {
	//BoxNX
	class Capsule : public ALP_shape {
	public:
		Vector3	center;	//SÀW
		Vector3	rotate;	//ñ]
		float r;		//¼a
		float length;	//·³

		Capsule() : center(Vector3(0)), rotate(Vector3(0)), r(1), length(1) { shape_tag = Physics_function::ALP_Collider_shape_tag::Capsule; };

		void adapt_Colliderdata() override {
			local_position = center;
			local_orientation = quaternion_from_euler(rotate);
			local_scale = Vector3(r, length, r);

		};

		void Update_hierarchy() override {
			float ave = 1;
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
			//¼a
			{
				ImGui::DragFloat("r", &r, ave * 0.001f, 0, 0, "%.2f");
			}
			//length
			{
				ImGui::DragFloat("length", &length, ave * 0.001f, 0, 0, "%.2f");
			}

		};

		void update_dop14() override {
			Vector3 p = vector3_quatrotate(Vector3(0, world_scale().y, 0), world_orientation());
			dop14.pos = world_position();
			for (int i = 0; i < DOP::DOP14_size; i++) {
				float v0, v1, v2, v3;
				v0 = vector3_dot(+p, DOP::DOP_14_axis[i]) + +world_scale().x * 1.0000001f;
				v1 = vector3_dot(+p, DOP::DOP_14_axis[i]) + -world_scale().x * 1.0000001f;
				v2 = vector3_dot(-p, DOP::DOP_14_axis[i]) + +world_scale().x * 1.0000001f;
				v3 = vector3_dot(-p, DOP::DOP_14_axis[i]) + -world_scale().x * 1.0000001f;
				dop14.max[i] = ALmax(ALmax(v0, v1), ALmax(v2, v3));
				dop14.min[i] = ALmin(ALmin(v0, v1), ALmin(v2, v3));
			}
		};

	};

}