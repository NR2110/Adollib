#pragma once

#include "joint_base.h"
#include "collider.h"

namespace Adollib {
	class Collider;

	namespace Physics_function {
		class ALP_Joint;
	}

	// �Ђ˂�S��(�W���C���g?)
	// collider0�����
	// collider1�̉�]�ʂɐ�����������
	class TwistJoint : public Joint_base {
	public:
		TwistJoint(Collider* l_colliderA_comp, Collider* l_colliderB_comp, Physics_function::ALP_Joint* l_ALPjoint)
			: Joint_base(l_colliderA_comp, l_colliderB_comp, l_ALPjoint) {}

		Physics_function::Vector3 vec0 = Physics_function::Vector3(1, 0, 0); //collider_comp[0]�̎�
		Physics_function::Vector3 vec1 = Physics_function::Vector3(1, 0, 0); //collider_comp[1]�̎�

		float max_radian_pow = 10; //�p�x�̍����傫������ƒ��˂Ă������̂� limit_effect�ŗ͂������鎞�ɍl������p�x�̍ő�l

		Physics_function::Vector2 limit = Physics_function::Vector2(0, 360);

	public:
		void adapt_anchor() override {
			anchor_count = 0;
		}

		Physics_function::Matrix33 tensor_effect() const override {
			return Physics_function::matrix33_zero();
		}

		void velocity_effect() const override {};

		bool limit_effect(Physics_function::Vector3& contactP0, Physics_function::Vector3& contactP1, float& penetrate) const override;



	};

};
