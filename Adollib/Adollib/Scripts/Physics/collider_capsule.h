#pragma once
#include "collider_shape.h"

namespace Adollib {
	//Boxクラス
	class Capsule : public Collider_shape {
	public:
		Vector3	center;	//中心座標
		Vector3	rotate;	//回転
		float r;		//半径
		float length;	//長さ

		Capsule(Physics_function::ALP_Collider* l_ALPcollider_ptr) : center(Vector3(0)), rotate(Vector3(0)), r(1), length(1) {
			shape_tag = Physics_function::ALPCollider_shape_type::Capsule;
			ALPcollider_ptr = l_ALPcollider_ptr;
		};

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
			//半径
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

		const Matrix33 tensor_base() const override {
			const Vector3& Wsize = world_scale();
			Matrix33 ret;

			ret = matrix33_identity();

			//Rは半径 Hは円柱の高さ
			//円柱
			//X : M(1/4 R^2 + 1/12 H^2)
			//Y : 1/2 M R^2
			//Z : M(1/4 R^2 + 1/12 H^2)
			//端の球
			//X : M(1/5 R^2 + 1/4 H^2) * 2
			//Y : 2/5 M R^2
			//Z : M(1/5 R^2 + 1/4 H^2) * 2

			//Wsize.yは円柱の高さ/2なので
			float H = Wsize.y * 2;

			ret._11 = 0.65f * Wsize.x * Wsize.x + 0.583333333f * H * H;
			ret._22 = 0.9f * Wsize.x * Wsize.x;
			ret._33 = 0.65f * Wsize.x * Wsize.x + 0.583333333f * H * H;

			//なぜか上の式でうまくいかないので とりあえず四角のテンソルを入れておく
			//ret._11 = 0.3333333f * ((Wsize.y * Wsize.y) + (Wsize.z * Wsize.z));
			//ret._22 = 0.3333333f * ((Wsize.z * Wsize.z) + (Wsize.x * Wsize.x));
			//ret._33 = 0.3333333f * ((Wsize.x * Wsize.x) + (Wsize.y * Wsize.y));



			return ret;
		};


	};

}