#pragma once
#include "collider_shape.h"

namespace Adollib {

	//球体用クラス
	class Sphere : public Collider_shape {
	public:
		Vector3 center; //中心座標
		float r = 1; //半径

		Sphere(Physics_function::ALP_Collider* l_ALPcollider_ptr) : center(Vector3(0)), r(1) {
			shape_tag = Physics_function::ALPCollider_shape_type::Sphere;
			ALPcollider_ptr = l_ALPcollider_ptr;
		};

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

		const Matrix33 tensor_base() const override {
			const Vector3& Wsize = world_scale();
			Matrix33 ret;

			ret = matrix33_identity();
			ret._11 = 0.4f * Wsize.x * Wsize.x;
			ret._22 = 0.4f * Wsize.x * Wsize.x;
			ret._33 = 0.4f * Wsize.x * Wsize.x;
			return ret;
		};


	};

}