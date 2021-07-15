#pragma once

#include "joint_base.h"

namespace Adollib {
	class Collider;

	namespace Physics_function {
		class ALP_Joint;

		// Conetwist ���ȂǂɎg�p
		class ConetwistJoint : public Joint_base {
		public:
			ConetwistJoint(Collider* l_colliderA_comp, Collider* l_colliderB_comp, ALP_Joint* l_ALPjoint)
				: Joint_base(l_colliderA_comp, l_colliderB_comp, l_ALPjoint) {}

			Anchor anchor; //�ڑ��_
			Vector3 limit_axis[2]; //�� axis��axis�̊p�x��limit���Ƃ�

			float limit = 0;

		public:
			void adapt_anchor() override {
				anchor_count = 1;

				anchors[0] = anchor;
			}

			Matrix33 tensor_effect(const Collider* coll) const override {
				return matrix33_zero();
			}

			bool limit_effect(Vector3& contactP0, Vector3& contactP1, float& penetrate) const override {
				return false;
			}



		};

	}

};
