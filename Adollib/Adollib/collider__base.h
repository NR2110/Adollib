#pragma once

#include <vector>
#include <assert.h>
#include <string>
#include "math.h"

namespace Adollib {

	class Gameobject;
	namespace  physics_function {
		class Solverbody;
	}
	namespace Contacts {
		class Contact;
	}

	enum class Collider_shape {
		shape_box,
		shape_sphere,
		shape_plane,
		shape_mesh,

		shape_size

	};

	class Collider {
	public:
		bool move = true; //���I�u�W�F�N�g�J�ǂ����̃t���O
		bool fall = true;

		Gameobject* gameobject = nullptr;	//�e���

		Collider_shape shape = Collider_shape::shape_size;	//�`���
		DOP::DOP_14 dop14; //DOP_7�f�[�^
		physics_function::Solverbody* solve;

		std::string tag; //���g�̃^�O
		std::vector<std::string> No_hit_tag; //�Փ˂��Ȃ��^�O

		Vector3 world_position = Vector3();		     //���[���h��Ԃł̍��W
		Quaternion world_orientation = Vector3();    //���[���h��Ԃł̎p��
		Vector3 world_scale = Vector3();              //���[���h��Ԃł̑傫��

		Vector3 local_position = Vector3();             //go����̑��΍��W
		Quaternion local_orientation = quaternion_identity();       //go����̑��Ύp��
		Vector3 local_scale = Vector3();				//go�Ƃ̑���scale

		float density = 1;						//���x
		float restitution = 0.1; 						//�����W��
		float friction = 0.4;					//���C��

		Quaternion offset_CollGO_quat;
		Vector3 offset_CollGO_pos;

		Vector3 linear_velocity = Vector3();    //���i���x
		Vector3 angula_velocity = Vector3();   //�p���x

		Vector3 linear_acceleration = Vector3();  //����
		Vector3 angula_acceleration = Vector3();//�p����

		float inertial_mass = 0;           //��������

		Vector3 accumulated_force = Vector3();  //���i�ړ��ɉ������

		matrix inertial_tensor = matrix();      //�������[�����g

		Vector3 accumulated_torque = Vector3(); //�p��]�ɉ������

		Collider() :
			local_position(0, 0, 0), local_orientation(1, 0, 0, 0), local_scale(1,1,1),
			linear_velocity(0, 0, 0), angula_velocity(0, 0, 0),
			inertial_mass(1), accumulated_force(0, 0, 0),
			accumulated_torque(0, 0, 0), solve(nullptr)
		{
		}

		//�O�͂̍X�V
		void apply_external_force(float duration = 1);

		//���x�A�����x��0�ɂ���
		void reset_force();

		//���W,�p���̍X�V
		void integrate(float duration = 1);

		//gameobject�ւ̕ω��ʂ����߂�
		void solv_resolve();

		//gameobject�֕ω��ʂ�n��
		void resolve_gameobject();

		//gameobject��transform����collider��world��Ԃł̏����X�V
		virtual void update_world_trans() = 0;

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
		matrix inverse_inertial_tensor() const;

		//�T�C�Y�����֐�
		virtual Quaternion get_dimension() const = 0;

		//�T�C�Y�ύX�ȂǂɑΉ����邽�ߖ��t���[�������e���\���Ȃǂ��X�V
		virtual void update_inertial(const Vector3& size, float density = 1) = 0;

		//�I�u�W�F�N�g���������тɌĂяo���@�̂������悢���K���ɖ��t���[���Ă�
		//DOP_14�f�[�^�̍X�V
		virtual void update_dop14() = 0;

	};
}