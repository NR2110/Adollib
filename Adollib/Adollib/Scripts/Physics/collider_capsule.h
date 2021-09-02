#pragma once
#include "collider_shape.h"
namespace Adollib {
	//BoxÉNÉâÉX
	class Capsule : public Collider_shape {
	public:
		Vector3	center;	//íÜêSç¿ïW
		Vector3	rotate;	//âÒì]
		float r;		//îºåa
		float length;	//í∑Ç≥

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
			//îºåa
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

		const Matrix33 local_tensor() const override {
			const Vector3& Wsize = world_scale();
			Matrix33 ret;

			ret = matrix33_identity();

			//RÇÕîºåa HÇÕâ~íåÇÃçÇÇ≥
			//â~íå
			//Mc = PI * r^2 * H
			//X : Mc(1/4 R^2 + 1/12 H^2)
			//Y : 1/2 Mc R^2
			//Z : Mc(1/4 R^2 + 1/12 H^2)
			//í[ÇÃãÖ
			//Ms = PI * r^3 * 4/3
			//X : Ms(1/5 R^2 + 1/4 H^2) * 2
			//Y : 2/5 Ms R^2
			//Z : Ms(1/5 R^2 + 1/4 H^2) * 2

			//Wsize.yÇÕâ~íåÇÃçÇÇ≥/2Ç»ÇÃÇ≈
			const float H = Wsize.y * 2;
			float Mc = PI * Wsize.x * Wsize.x * H;
			float Ms = PI * Wsize.x * Wsize.x * Wsize.x * 1.3333333f;
			Mc /= Mc + Ms; //ëÃêœî‰Ç≈ï\Ç∑
			Ms /= Mc + Ms;

			constexpr float _inv2 = 0.5f;
			constexpr float _inv4 = 0.25f;
			constexpr float _2inv4 = 0.5f;
			constexpr float _2inv5 = 0.4f;
			constexpr float _inv12 = 0.08333333f;

			ret._11 = Wsize.x * Wsize.x * (_inv4 * Mc + _2inv5 * Ms) + H * H * (_inv12 * Mc + _2inv4 * Ms);
			ret._22 = Wsize.x * Wsize.x * (_inv2 * Mc + _2inv5 * Ms);
			ret._33 = ret._11;

			return ret;
		};

		const float get_volume() const override {
			const float h = local_scale.x * local_scale.x * DirectX::XM_PI * local_scale.y * 2;
			const float s = 1.3333333f * DirectX::XM_PI * local_scale.x * local_scale.x * local_scale.x;
			return h + s;
		}


	};

}