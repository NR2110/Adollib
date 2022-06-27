#pragma once

#include "joint_base.h"

#include "collider.h"


namespace Adollib {
	class Collider;

	namespace Physics_function {
		class ALP_Joint;
	}

	// Conetwist ���ȂǂɎg�p
	class ConeJoint : public Joint_base {
	public:
		ConeJoint(std::shared_ptr<Collider> l_colliderA_comp, std::shared_ptr<Collider> l_colliderB_comp, Physics_function::ALP_Joint* l_ALPjoint)
			: Joint_base(l_colliderA_comp, l_colliderB_comp, l_ALPjoint) {}

		Anchor anchor; //�ڑ��_
		Physics_function::Vector3 limit_axis[2]; //�� axis��axis�̊p�x��limit���Ƃ�

		float limit = 90;

	public:
		void adapt_anchor() override {
			anchor_count = 1;

			anchors[0] = anchor;
		}

		void velocity_effect() const override {};

		Physics_function::Matrix33 tensor_effect() const override {
			return Physics_function::matrix33_zero();
		}

		bool limit_effect(Physics_function::Vector3& contactP0, Physics_function::Vector3& contactP1, float& penetrate) const override;



	};

};
