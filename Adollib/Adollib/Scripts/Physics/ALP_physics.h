#pragma once

#include "../Math/math.h"
#include "ALP_collider.h"

#include "collider_types.h"

//SIMD���g�����ǂ��� �o�O���Ă��邽�ߎg�p�s��
//#define PHYICSE_USED_SIMD


namespace Adollib {
	namespace Physics_function {

		struct ALP_Solverbody {
#ifndef PHYICSE_USED_SIMD
			Vector3 delta_LinearVelocity; // ���i���x����
			Vector3 delta_AngulaVelocity; // ��]���x����
			//Quaternion orientation; // �p��
			Matrix inv_inertia; // �����e���\���̋t�s��
#else
			DirectX::XMVECTOR delta_LinearVelocity; // ���i���x����
			DirectX::XMVECTOR delta_AngulaVelocity; // ��]���x����
			//DirectX::XMVECTOR orientation; // �p��
			DirectX::XMMATRIX inv_inertia; // �����e���\���̋t�s��
#endif
			float  inv_mass = 0; // ���ʂ̋t��
			//int num = 0;
		};

		class ALP_Physics {
		public:
			//�R���X�g���N�^
			ALP_Physics(
				Gameobject* l_go, std::list<ALP_Physics*>::iterator l_itr, ALP_Collider* _ALPcollider ,Scenelist l_scene, u_int l_index
			) :
				gameobject(l_go), this_itr(l_itr), ALPcollider(_ALPcollider), scene(l_scene), index(l_index) {};

		private:
			//::: ���g�ւ̃C�e���[�^(remove�p) :::
			std::list<ALP_Physics*>::iterator this_itr;

			u_int index = 0; //����collider�̔ԍ�
			Scenelist scene = Scenelist::scene_null; //����colldier�����݂���scene

			//::: �������[�����g��type :::
			Physics_function::Tensor_type tensor_type = Physics_function::Tensor_type::None;

			//::: Collider�ւ̃|�C���^ :::
			ALP_Collider* ALPcollider = nullptr;

			//::: �A�^�b�`���ꂽGO�ւ̃|�C���^ :::
			Gameobject* gameobject = nullptr;
		public:
			ALP_Solverbody* solve = nullptr; //�Փˉ����p

			void set_default();

			//::: �ύX�� :::::::::::::::::::::::::::::
			float inertial_mass = 0; //����
			float linear_drag = 0; //��C��R
			float anglar_drag = 0; //��C��R
			float dynamic_friction = 0;//�����C
			float static_friction = 0; //�Ö��C
			float restitution = 0;	 //�����W��

			bool is_fallable = false; //�����Ȃ�
			bool is_kinmatic_anglar = false; //�ق��̕��̂���̉e���ŉ�]���x���ω����Ȃ�
			bool is_kinmatic_linear = false; //�ق��̕��̂���̉e���ŕ��i���x���ω����Ȃ�
			bool is_moveable = false;//�����Ȃ�
			bool is_hitable = false;  //�Փ˂��Ȃ�

			//::: �����邾�� :::::::::::::::::::::::::
			float speed = 0;//debug���₷���悤��
			Vector3 linear_velocity;//���i���x
			Vector3 anglar_velocity; //��]���x

			Matrix inertial_tensor; //�����e���\��

			bool sleep_state = false; //sleep��Ԃ���flag

			//::: �����Ȃ� :::::::::::::::::::::::::::
			Vector3 accumulated_force;//���i���x
			Vector3 accumulated_torque; //��]���x

			Vector3 linear_acceleration;//���i�����x
			Vector3 angula_acceleration; //��]�����x

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

			//�ǉ����ꂽ�`�ɂ�芵�����[�����g�̃^�C�v��ύX����
			void add_tensor_type(Tensor_type type);

			//���݃A�^�b�`����Ă���shape���犵�����[�����g�̃^�C�v���Đݒ肷��(shape��remove���ꂽ�Ƃ��ȂǂɎg�p)
			void refresh_tensor_type(const std::vector<Collider_shape*>& shapes);

			//�������[�����g�����[�U�[�w��̂��̂ɌŒ肷��
			void set_tensor(const Matrix& tensor) {
				tensor_type = Tensor_type::User;
				inertial_tensor = tensor;
			}


			//::: ���t���[���ĂԂ��� ::::::::::::
			//���x�A�����x��0�ɂ���
			void reset_force();

			//�O�͂̍X�V
			void apply_external_force(float duration = 1);

			//���W,�p���̍X�V
			void integrate(float duration = 1);

			//�T�C�Y�ύX�ȂǂɑΉ����邽�ߖ��t���[�������e���\���Ȃǂ��X�V �����e���\���̍X�V��ALP_collider::scale��ALP_Physics::mass���K�v�Ȃ��߂����ɋL�q
			void update_tensor(const std::vector<Collider_shape*>& shapes);

			//::: collider:Component �� mass�Ȃǂ��ύX���ꂽ�Ƃ��ɌĂԂ��� :::
			// Collider������̊l��
			void update_physics_data();

			//�}�l�[�W���[���炱�̃N���X�̍폜
			void destroy();

		};
	}
}
