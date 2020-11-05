#pragma once

#include "../Math/math.h"
#include "ALP_collider.h"

namespace Adollib {
	namespace physics_function {

		struct ALP_Solverbody {
			Vector3 delta_LinearVelocity; // ���i���x����
			Vector3 delta_AngulaVelocity; // ��]���x����
			Quaternion orientation; // �p��
			Matrix inv_inertia; // �����e���\���̋t�s��
			float  inv_mass; // ���ʂ̋t��
			int num;
		};

		class ALP_Physics {
		public:
			ALP_Physics() {};

			//::: �ύX�� :::::::::::::::::::::::::::::
			float inertial_mass; //����
			float drag; //��C��R
			float anglar_drag; //��C��R
			float dynamic_friction;//�����C
			float static_friction; //�Ö��C
			float restitution;	 //�����W��

			bool is_fallable; //�����Ȃ�
			bool is_kinematic; //�e�������Ȃ�(fall�͂���)
			bool is_moveable;//�����Ȃ�

			//::: �����邾�� :::::::::::::::::::::::::
			float speed;//debug���₷���悤��
			Vector3 linear_velocity;//���i���x
			Vector3 anglar_velocity; //��]���x

			Matrix inertial_tensor;//�����e���\��

			bool sleep_state; //sleep��Ԃ���flag

			//::: �����Ȃ� :::::::::::::::::::::::::::
			Vector3 accumulated_force;//���i���x
			Vector3 accumulated_torque; //��]���x

			Vector3 linear_acceleration;//���i�����x
			Vector3 angula_acceleration; //��]�����x

			ALP_Collider* ALP_coll;

			ALP_Solverbody* solve; //�Փˉ����p

		public:
			//���i�ړ��ɗ͂�������
			void add_force(const Vector3& force);

			//�p��]�ɗ͂�������
			void add_torque(const Vector3& force);

			//���I�u�W�F�N�g���ǂ���
			bool is_movable() const;

			//���ʂ̋t����Ԃ�(�s�ғ��I�u�W�F�N�g��0��Ԃ�)
			float inverse_mass() const;

			//�������[�����g�̋t�s���Ԃ�
			Matrix inverse_inertial_tensor() const;

			//::: ���t���[���ĂԂ��� :::::
			//���x�A�����x��0�ɂ���
			void reset_force();

			//�O�͂̍X�V
			void apply_external_force(float duration = 1);

			//���W,�p���̍X�V
			void integrate(float duration = 1);

			//�T�C�Y�ύX�ȂǂɑΉ����邽�ߖ��t���[�������e���\���Ȃǂ��X�V �����e���\���̍X�V��ALP_collider::scale��ALP_Physics::mass���K�v�Ȃ��߂����ɋL�q
			void update_inertial();
		};
	}
}
