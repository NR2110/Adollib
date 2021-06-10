#pragma once
#include "collider_shape.h"

namespace Adollib {
	//平面用クラス
	class Plane : public Collider_shape {
	public:
		Vector3 rotate; //平面の回転
		float distance; //平面の距離

		//不動オブジェクトとして生成
		Plane(Physics_function::ALP_Collider* l_ALPcollider_ptr) :rotate(Vector3(0, 0, 0)), distance(0)
		{
			shape_tag = Physics_function::ALPCollider_shape_type::Plane;
			tensor_type = Physics_function::Tensor_type::Box;
			ALPcollider_ptr = l_ALPcollider_ptr;
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

		void update_inertial_tensor(Matrix& inertial_tensor, const float& inertial_mass) override {
			const Vector3& Wsize = world_scale();

			inertial_tensor = matrix_identity();
			inertial_tensor._11 = 0.3333333f * inertial_mass * ((Wsize.y * Wsize.y) + (Wsize.z * Wsize.z));
			inertial_tensor._22 = 0.3333333f * inertial_mass * ((Wsize.z * Wsize.z) + (Wsize.x * Wsize.x));
			inertial_tensor._33 = 0.3333333f * inertial_mass * ((Wsize.x * Wsize.x) + (Wsize.y * Wsize.y));
		};

	};

}