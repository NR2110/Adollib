#pragma once
#include <unordered_map>
#include <string>
#include "../Math/math.h"

namespace Adollib {
	namespace physics_function {

		class ALP_Physics {
		public:
			float mass; //����
			float drag; //��C��R
			float anglar_drag; //��C��R
			float dynamic_friction;//�����C
			float static_friction; //�Ö��C
			float restitution;	 //�����W��

			bool is_fallable; //�����Ȃ�
			bool is_kinematic; //�e�������Ȃ�(fall�͂���)
			bool is_moveable;//�����Ȃ�

			float speed;//debug���₷���悤��
			Vector3 linear_velocity;//���i���x
			Vector3 anglar_velocity; //��]���x
			Matrix inertia_tensor;//�����e���\��

			bool sleep_state; //sleep��Ԃ���flag

		public:
			//�O�͂̍X�V
			void apply_external_force(float duration = 1);

			//���x�A�����x��0�ɂ���
			void reset_force();

			//���i�ړ��ɗ͂�������
			void add_force(const Vector3& force);

			//�p��]�ɗ͂�������
			void add_torque(const Vector3& force);

			//���I�u�W�F�N�g���ǂ���
			bool is_movable() const;

			//�����I�u�W�F�N�g���ǂ���
			bool is_fallable() const;

			//���ʂ̋t����Ԃ�(�s�ғ��I�u�W�F�N�g��0��Ԃ�)
			float inverse_mass() const;

			//�������[�����g�̋t�s���Ԃ�
			Matrix inverse_inertial_tensor() const;

		};
	}
}
