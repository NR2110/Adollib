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

		Gameobject* gameobject = nullptr;	//�e���

		Collider_shape shape = Collider_shape::shape_size;	//�`���
		DOP_14 dop14; //DOP_7�f�[�^
		physics_function::Solverbody* solve;

		std::string tag; //���g�̃^�O
		std::vector<std::string> No_hit_tag; //�Փ˂��Ȃ��^�O

		vector3 world_position = vector3();		     //���[���h��Ԃł̍��W
		quaternion world_orientation = vector3();    //���[���h��Ԃł̎p��
		vector3 world_size = vector3();              //���[���h��Ԃł̑傫��

		vector3 local_position = vector3();             //go����̑��΍��W
		quaternion local_orientation = quaternion_identity();       //go����̑��Ύp��
		vector3 local_scale = vector3();				//go�Ƃ̑���scale

		float density = 1;						//���x
		float restitution = 0.1; 						//�����W��
		float friction = 0.4;					//���C��

		vector3 linear_velocity = vector3();    //���i���x
		vector3 angula_velocity = vector3();   //�p���x

		vector3 liner_acceleration = vector3();  //����
		vector3 angular_acceleration = vector3();//�p����

		float inertial_mass = 0;           //��������

		vector3 accumulated_force = vector3();  //���i�ړ��ɉ������

		matrix inertial_tensor = matrix();      //�������[�����g

		vector3 accumulated_torque = vector3(); //�p��]�ɉ������

		Collider() :
			local_position(0, 0, 0), local_orientation(0, 0, 0, 1),
			linear_velocity(0, 0, 0), angula_velocity(0, 0, 0),
			inertial_mass(1), accumulated_force(0, 0, 0),
			accumulated_torque(0, 0, 0), solve(nullptr)
		{
		}

		//�O�͂̍X�V
		void apply_external_force(float duration = 1);

		//���W,�p���̍X�V
		void integrate(float duration = 1);

		//gameobject�֕ω��ʂ�n��
		void resolve_gameobject();

		//gameobject��transform����collider��world��Ԃł̏����X�V
		virtual void update_world_trans() = 0;

		//���i�ړ��ɗ͂�������
		void add_force(const vector3& force);

		//�p��]�ɗ͂�������
		void add_torque(const vector3& force);

		//���I�u�W�F�N�g���ǂ���
		bool is_movable() const;

		//���ʂ̋t����Ԃ�(�s�ғ��I�u�W�F�N�g��0��Ԃ�)
		float inverse_mass() const;

		//�������[�����g�̋t�s���Ԃ�
		matrix inverse_inertial_tensor() const;

		//�T�C�Y�����֐�
		virtual quaternion get_dimension() const = 0;

		//�T�C�Y�ύX�ȂǂɑΉ����邽�ߖ��t���[�������e���\���Ȃǂ��X�V
		virtual void update_inertial(const vector3& size, float density = 1) = 0;

		//�I�u�W�F�N�g���������тɌĂяo���@�̂������悢���K���ɖ��t���[���Ă�
		//DOP_14�f�[�^�̍X�V
		virtual void update_dop14() = 0;

	};
}