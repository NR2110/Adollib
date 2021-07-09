#pragma once

#include "joint_base.h"

namespace Adollib {
	class Collider;

	namespace Physics_function {
		class ALP_Joint;

		// ヒンジジョイント
		class HingeJoint : public Joint_base {
		public:
			HingeJoint(Collider* l_colliderA_comp, Collider* l_colliderB_comp, ALP_Joint* l_ALPjoint)
				: Joint_base(l_colliderA_comp, l_colliderB_comp, l_ALPjoint) {}

			Anchor anchor_s; //Hinge 始点
			Anchor anchor_g; //Hinge 終点

			float hinge_pow = 0;

		public:
			void adapt_anchor() override {
				anchor_count = 2;

				anchors[0] = anchor_s;
				anchors[1] = anchor_g;
			}

			Matrix33 tensor_effect(const Collider* coll) const override {
				Vector3 axis;
				if (coll == collider_comp[0])axis = anchor_s.posA - anchor_g.posA;
				else axis = anchor_s.posB - anchor_g.posB;

				Matrix33 mat = matrix33_zero();

				axis.x = fabsf(axis.x);
				axis.y = fabsf(axis.y);
				axis.z = fabsf(axis.z);

				mat._11 = axis.y * axis.y + axis.z * axis.z;
				mat._22 = axis.z * axis.z + axis.x * axis.x;
				mat._33 = axis.x * axis.x + axis.y * axis.y;

				mat *= hinge_pow;

				return mat;
			}


		};

	}

};
