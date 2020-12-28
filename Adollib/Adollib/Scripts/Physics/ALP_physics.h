#pragma once

#include "../Math/math.h"
#include "ALP_collider.h"

namespace Adollib {
	namespace Physics_function {

		struct ALP_Solverbody {
#if 1 || _DEBUG
			Vector3 delta_LinearVelocity; // ���i���x����
			Vector3 delta_AngulaVelocity; // ��]���x����
			Quaternion orientation; // �p��
			Matrix inv_inertia; // �����e���\���̋t�s��
#else
			DirectX::XMVECTOR delta_LinearVelocity; // ���i���x����
			DirectX::XMVECTOR delta_AngulaVelocity; // ��]���x����
			DirectX::XMVECTOR orientation; // �p��
			DirectX::XMMATRIX inv_inertia; // �����e���\���̋t�s��
#endif
			float  inv_mass = 0; // ���ʂ̋t��
			int num = 0;
		};

		class ALP_Physics {
		public:
			ALP_Physics() {};

			ALP_Physics(
				float inertial_mass, //����
				float linear_drag, //��C��R
				float anglar_drag, //��C��R
				float dynamic_friction,//�����C
				float static_friction, //�Ö��C
				float restitution,	 //�����W��

				bool is_fallable, //�����Ȃ�
				bool is_kinematic, //�e�������Ȃ�(fall�͂���)
				bool is_kinmatic_anglar,
				bool is_kinmatic_linear,
				bool is_moveable,//�����Ȃ�
				bool is_hitable //�Փ˂��Ȃ�
			) :
				inertial_mass(inertial_mass), //����
				linear_drag(linear_drag), //��C��R
				anglar_drag(anglar_drag), //��C��R
				dynamic_friction(dynamic_friction),//�����C
				static_friction(static_friction), //�Ö��C
				restitution(restitution),	 //�����W��

				is_fallable(is_fallable), //�����Ȃ�
				is_kinematic(is_kinematic), //�e�������Ȃ�(fall�͂���)
				is_kinmatic_anglar(is_kinmatic_anglar), //�ق��̕��̂���̉e���ŉ�]���x���ω����Ȃ�
				is_kinmatic_linear(is_kinmatic_linear), //�ق��̕��̂���̉e���ŕ��i���x���ω����Ȃ�
				is_moveable(is_moveable),//�����Ȃ�
				is_hitable(is_hitable)
			{};

			//::: �ύX�� :::::::::::::::::::::::::::::
			float inertial_mass; //����
			float linear_drag; //��C��R
			float anglar_drag; //��C��R
			float dynamic_friction;//�����C
			float static_friction; //�Ö��C
			float restitution;	 //�����W��

			bool is_fallable; //�����Ȃ�
			bool is_kinematic; //�e�������Ȃ�(fall�͂���)
			bool is_kinmatic_anglar; //�ق��̕��̂���̉e���ŉ�]���x���ω����Ȃ�
			bool is_kinmatic_linear; //�ق��̕��̂���̉e���ŕ��i���x���ω����Ȃ�
			bool is_moveable;//�����Ȃ�
			bool is_hitable;  //�Փ˂��Ȃ�

			//::: �����邾�� :::::::::::::::::::::::::
			float speed = 0;//debug���₷���悤��
			Vector3 linear_velocity;//���i���x
			Vector3 anglar_velocity; //��]���x

			Matrix inertial_tensor;//�����e���\��

			bool sleep_state = false; //sleep��Ԃ���flag

			//::: �����Ȃ� :::::::::::::::::::::::::::
			Vector3 accumulated_force;//���i���x
			Vector3 accumulated_torque; //��]���x

			Vector3 linear_acceleration;//���i�����x
			Vector3 angula_acceleration; //��]�����x

			std::list<ALP_Collider>::iterator ALP_coll;

			ALP_Solverbody* solve = nullptr; //�Փˉ����p

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

			//::: collider:Component �� mass�Ȃǂ��ύX���ꂽ�Ƃ��ɌĂԂ��� :::
			// Collider������̊l��
			void refresh_ALP_from_data();

		};
	}
}
