#pragma once

#include "joint_base.h"

namespace Adollib {
	class Collider;

	namespace Physics_function {
		class ALP_Joint;
	}

	// �{�[���W���C���g
	class BallJoint : public Joint_base {
	public:
		BallJoint(Collider* l_colliderA_comp, Collider* l_colliderB_comp, Physics_function::ALP_Joint* l_ALPjoint)
			: Joint_base(l_colliderA_comp, l_colliderB_comp, l_ALPjoint) {}

		Anchor anchor;

	public:
		void adapt_anchor() override {
			anchor_count = 1;

			anchors[0] = anchor;
		}

		Matrix33 tensor_effect() const override {
			return matrix33_zero();
		}

		void velocity_effect() const override {};

		bool limit_effect(Vector3& contactP0, Vector3& contactP1, float& penetrate) const override {
			return false;
		}


	};

};
