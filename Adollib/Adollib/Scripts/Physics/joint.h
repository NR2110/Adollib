#pragma once

#include "../Math/math.h"
#include "collider.h"
#include "ALP_joint_base.h"

namespace Adollib {

	namespace Physics_function {
		class ALP_Collider;

		class Joint_data {
		public:
			Joint_data(ALP_Joint_base* base) {

				anchorA = &base->anchorA;
				anchorB = &base->anchorB;
				bias = &base->bias;

				colliderA = base->colliderA;
				colliderB = base->colliderB;
			}

		public:
			//���̂̃f�[�^�ւ̃|�C���^
			float* bias = 0; // �S���̋����̒����l
			Vector3* anchorA = nullptr; // ����A�̃��[�J�����W�n�ɂ�����ڑ��_
			Vector3* anchorB = nullptr; // ����B�̃��[�J�����W�n�ɂ�����ڑ��_

		private:
			Physics_function::ALP_Collider* colliderA = nullptr; // ����A�ւ̃C���f�b�N�X
			Physics_function::ALP_Collider* colliderB = nullptr; // ����B�ւ̃C���f�b�N�X

		public:
			Physics_function::ALP_Collider* get_colliderA() { return colliderA; };
			Physics_function::ALP_Collider* get_colliderB() { return colliderB; };

			void destroy();
		};

		class Joint {
			static Joint_data add_balljoint(
				Collider* colliderA, Vector3 anchorA,
				Collider* colliderB, Vector3 anchorB,
				float bias = 1
				);
		};



	}
}