#pragma once

#include "../Math/math.h"
#include "ALP_collider.h"

#include "collider_types.h"

#include <mutex>



namespace Adollib {
	namespace Physics_function {

		struct ALP_Solverbody {

			DirectX::XMVECTOR delta_LinearVelocity; // ���i���x����
			DirectX::XMVECTOR delta_AngulaVelocity; // ��]���x����

			DirectX::XMMATRIX inv_tensor; // �����e���\���̋t�s��

			DirectX::XMVECTOR Worient; //�A�^�b�`���Ă���GO��world_orientation
			DirectX::XMVECTOR Wposition; //�A�^�b�`���Ă���GO��world_position

			float  inv_mass = 0; // ���ʂ̋t��
			//int num = 0;
		};

		class ALP_Physics {
		public:
			//�R���X�g���N�^
			ALP_Physics(
				__int64 l_UUID, std::list<ALP_Physics*>::iterator l_itr, ALP_Collider* _ALPcollider, u_int l_index
			) :
				UUID(l_UUID), this_itr(l_itr), ALPcollider(_ALPcollider), index(l_index) {
			};

		private:
			//::: ���g�ւ̃C�e���[�^(remove�p) :::
			std::list<ALP_Physics*>::iterator this_itr;

			u_int index = 0; //����collider�̔ԍ�

			//::: �������[�����g�����[�U�[��`���ꂽ���̂� :::
			bool is_user_tensor = false;

			//::: �d�S�����[�U�[�ɒ�`���ꂽ���̂� :::
			bool is_user_barycnter = false;

			//::: Collider�ւ̃|�C���^ :::
			ALP_Collider* ALPcollider = nullptr;

			//::: �A�^�b�`���ꂽGO�ւ̃|�C���^ :::
			__int64 UUID = 0;

			//::: �}���`�X���b�h�p��collider�ɃR�s�[���ꂽtransform�̃|�C���^ physics����͓ǂݎ��̂� :::
			world_trans* transform = nullptr;

			std::mutex mtx;

		public:
			// �}���`�X���b�h������ɂ�����Aadd_collider�������_�ł̓��C����list�ɓ��ꂸbuffer��list�ɓ���邽�� ���g��itr���������Ɍ��܂�Ȃ����� set�֐�������
			void set_this_itr(std::list<ALP_Physics*>::iterator itr) { this_itr = itr; };

		public:
			ALP_Solverbody* solve = nullptr; //�Փˉ����p

			//::: added�̔z�񂩂烁�C���̔z���adapt���ꂽ��true�ɂ��� (itr��added���w���̂�main�̔z����h���̂��킩��Ȃ�����)
			bool is_added = false;

			//::: �ύX�� :::::::::::::::::::::::::::::
			float inertial_mass = 0; //����
			float linear_drag = 0; //��C��R
			float angula_drag = 0; //��C��R
			float dynamic_friction = 0;//�����C
			float static_friction = 0; //�Ö��C
			float restitution = 0;	 //�����W��
			float linear_sleep_threrhold = 0.4f; //freeze��臒l
			float angula_sleep_threrhold = 0.2f; //freeze��臒l

			bool is_fallable = false; //�����Ȃ�
			bool is_kinmatic_anglar = false; //�ق��̕��̂���̉e���ŉ�]���x���ω����Ȃ�
			bool is_kinmatic_linear = false; //�ق��̕��̂���̉e���ŕ��i���x���ω����Ȃ�
			bool is_moveable = false;//�����Ȃ�
			bool is_hitable = false;  //�Փ˂��Ȃ�
			bool is_static = false;  //static���m��oncoll_enter���g���Ȃ� ���ǌy���Ȃ�



		private:
			//::: �����邾�� :::::::::::::::::::::::::
			Vector3 linear_velocity_;//���i���x
			Vector3 angula_velocity_; //��]���x

			Vector3 old_linear_velocity_;//���i���x
			Vector3 old_angula_velocity_; //��]���x

			bool is_sleep_ = false; //sleep��Ԃ���flag
			float sleep_timer = 0; //

			Matrix33 inertial_tensor_; //�����e���\��

		public:
			const Vector3& linear_velocity() const { return linear_velocity_; };
			const Vector3& angula_velocity() const { return angula_velocity_; };
			const Vector3& old_linear_velocity() const { return old_linear_velocity_; };
			const Vector3& old_angula_velocity() const { return old_angula_velocity_; };
			const bool& is_sleep()const { return is_sleep_; };

		private:
			float max_linear_velocity = FLT_MAX; //���i���x�̐���
			float max_angula_velocity = FLT_MAX; //��]���x�̐���

			Vector3 accumulated_force;  //���i���x
			Vector3 accumulated_torque; //��]���x

			Vector3 linear_acceleration; //���i�����x
			Vector3 angula_acceleration; //��]�����x

			Vector3 barycenter; //GO��local��Ԃ̏d�S���W

		public:
			//::: user�̃^�C�~���O�ŌĂԂ���(��������mutex��lock���K�v) :::::::

			// ���i�ړ��ɗ͂�������
			void add_force(const Vector3& force, const bool& is_force_local = false);
			void add_force(const Vector3& force, const Vector3& position, const bool& is_position_local = false,const bool& is_force_local = false);
			// �p��]�ɗ͂�������
			void add_torque(const Vector3& force, const bool& is_local = false);

			// ���i�����ɒl��������
			void add_linear_acc(const Vector3& acc);
			//�p�����ɒl��������
			void add_angula_acc(const Vector3& acc);

			// ���i���x�̎w��
			void set_linear_velocity(const Vector3& vec) { std::lock_guard <std::mutex> lock(mtx); linear_velocity_ = vec;};
			// �p���x�̎w��
			void set_angula_velocity(const Vector3& vec) { std::lock_guard <std::mutex> lock(mtx); angula_velocity_ = vec; };
			// old���i���x�̎w��
			void set_old_linear_velocity(const Vector3& old_vec) { std::lock_guard <std::mutex> lock(mtx); old_linear_velocity_ += old_vec; };
			// old�p���x�̎w��
			void set_old_angula_velocity(const Vector3& old_vec) { std::lock_guard <std::mutex> lock(mtx); old_angula_velocity_ += old_vec; };

			// ���x�������s��
			void set_max_linear_velocity(const float& max_scalar) {/*std::lock_guard <std::mutex> lock(mtx);*/ max_linear_velocity = max_scalar; }; //max_linear_velocity��phsics���l�����Ȃ�����mutex��lock���Ȃ�
			// ���x�������s��
			void set_max_angula_velocity(const float& max_scalar) {/*std::lock_guard <std::mutex> lock(mtx);*/ max_angula_velocity = max_scalar; };//max_angula_velocity��phtsics���l�����Ȃ�����mutex��lock���Ȃ�

			Matrix33 get_tensor();
			Matrix33 get_tensor_contain_added(); // �������[�����g�𓾂邽�߂�collider,spahe��adapt�Ȃǂ��낢�낵�Ă���
			void set_tensor(const Matrix33& tensor); //�������[�����g�����[�U�[�w��̂��̂ɌŒ肷��

			// �d�S��Ԃ�
			const Vector3 get_barycenter(bool is_calculate = true) const; //shape�݂̂̏d�S
			const Vector3 get_barycenter_contain_added(); //added_shape���܂߂��d�S(mutex��lock���邽��const�ɂł��Ȃ�)
			void set_barycenter(const Vector3& cent); //�d�S�����[�U�[�w��̂��̂ɌŒ肷��

			// ���x�A�����x��0�ɂ���
			void reset_force();

			//::: ���t���[��physics_manager���ĂԂ���(mutex��lock���K�v�Ȃ�) ::::::::::::

			// transform�̍X�V(transform�̒l���R�s�[���� �ŏI�I�ɂ͑��x�̉e���ʂ�transform�ɗ^���邽��mutex��lock�͕K�v�Ȃ��Ɣ��f)
			void copy_transform_ptr();

			// �O�͂̍X�V
			void apply_external_force(float duration = 1,float timeratio_60 = 1);

			// ���W,�p���̍X�V
			void integrate(float duration = 1);

			// �A�^�b�`���ꂽshapes���犵�����[�����g�Ǝ��ʁA���łɏd�S�̍X�V
			void update_tensor_and_barycenter(const std::list<Collider_shape*>& shapes, const std::list<ALP_Joint*>& joints);

			// Collider������̊l��
			void adapt_collider_component_data(); //component::collider�̏����R�s�[���� mutex��lock�͕K�v�Ȃ��Ɣ��f


			//::: �K�v�Ȏ��ɌĂԂ���(mutex��lock���K�v�Ȃ�) :::::::::

			// ���I�u�W�F�N�g���ǂ���
			bool is_movable() const;

			// ���ʂ̋t����Ԃ�(�s�ғ��I�u�W�F�N�g��0��Ԃ�)
			float inverse_mass() const;

			// �������[�����g�̋t�s���Ԃ�
			Matrix33 inverse_inertial_tensor() const;

			// �}�l�[�W���[���炱�̃N���X��remove, itr��private�Ȃ��߃����o�֐���remove����
			void destroy();

		};
	}
}
