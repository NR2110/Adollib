#pragma once
#include "collider__base.h"

namespace Adollib {
	//平面用クラス
	class Plane : public ALP_shape {
	public:
		Vector3 rotate; //平面の回転
		float distance; //平面の距離

		//不動オブジェクトとして生成
		Plane(Physics_function::ALP_Collider* l_ALPcollider_ptr) :rotate(Vector3(0, 0, 0)), distance(0)
		{
			shape_tag = Physics_function::ALP_Collider_shape_tag::Plane;
			ALPcollider_ptr = l_ALPcollider_ptr;
			//name = std::string("Plane");
			//physics_data.is_moveable = false;
			//physics_data.inertial_mass = FLT_MAX;
		};

		void adapt_Colliderdata() override {

			Quaternion orient = quaternion_from_euler(rotate);
			local_orientation = orient;
			local_position = vector3_quatrotate(Vector3(0,1,0), orient) * distance;
			local_scale = Vector3(FLT_MAX, 0, FLT_MAX);
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
		}

		void update_dop14() override {
				dop14.pos = world_position();
				for (int i = 0; i < DOP::DOP14_size; i++) {
					dop14.max[i] = FLT_MAX;
				}

		}
	};

}