#pragma once

#include "joint_base.h"
#include "collider.h"

namespace Adollib {
	class Collider;

	namespace Physics_function {
		class ALP_Joint;
	}

	// ヒンジジョイント
	class HingeJoint : public Joint_base {
	public:
		HingeJoint(std::shared_ptr<Collider> l_colliderA_comp, std::shared_ptr<Collider> l_colliderB_comp, Physics_function::ALP_Joint* l_ALPjoint)
			: Joint_base(l_colliderA_comp, l_colliderB_comp, l_ALPjoint) {}

		Anchor anchor_s; //Hinge 始点
		Anchor anchor_g; //Hinge 終点

		float hinge_pow = 0; //ジョイントの強さ 大きすぎると軸が動かなくなる?

		Physics_function::Vector2 limit = Physics_function::Vector2(0, 360);

	public:
		void adapt_anchor() override {
			anchor_count = 2;

			anchors[0] = anchor_s;
			anchors[1] = anchor_g;
		}

		void velocity_effect() const override;

		Physics_function::Matrix33 tensor_effect() const override {
			// hinge方向以外には曲がりにくくする
			Physics_function::Vector3 axis;
			//if (coll == collider_comp[0])axis = anchor_s.posA - anchor_g.posA;
			//else axis = anchor_s.posB - anchor_g.posB;
			axis = anchor_s.posA - anchor_g.posA;

			Physics_function::Matrix33 mat = Physics_function::matrix33_zero();

			axis.x = fabsf(axis.x);
			axis.y = fabsf(axis.y);
			axis.z = fabsf(axis.z);

			mat._11 = axis.y * axis.y + axis.z * axis.z;
			mat._22 = axis.z * axis.z + axis.x * axis.x;
			mat._33 = axis.x * axis.x + axis.y * axis.y;

			mat *= hinge_pow;

			return mat;
		}

		// 回転の角度制限
		bool limit_effect(Physics_function::Vector3& contactP0, Physics_function::Vector3& contactP1, float& penetrate) const override;



	};

};
