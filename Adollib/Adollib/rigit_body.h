#pragma once

#include <vector>
#include <assert.h>
#include <string>
#include "math.h"

//�ђʂ��Ȃ��Փ˔����p���邩(�����d���Ȃ�?)
//�����p����ꍇintegral�O�ɏՓ˔�����s��
//������
//����Ȃ�
#define USE_CHECK_CONTACT 0

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

		shape_null

	};

	class Collider {
	public:
		bool move = true; //���I�u�W�F�N�g�J�ǂ����̃t���O

		Gameobject* gameobject = nullptr;	//�e���

		Collider_shape shape = Collider_shape::shape_null;	//�`���
		DOP_7 dop7; //DOP_7�f�[�^
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
		vector3 angular_velocity = vector3();   //�p���x

		vector3 liner_acceleration = vector3();  //����
		vector3 angular_acceleration = vector3();//�p����

		float inertial_mass = 0;           //��������

		vector3 accumulated_force = vector3();  //���i�ړ��ɉ������

		matrix inertial_tensor = matrix();      //�������[�����g

		vector3 accumulated_torque = vector3(); //�p��]�ɉ������

		Collider() :
			local_position(0, 0, 0), local_orientation(0, 0, 0, 1),
			linear_velocity(0, 0, 0), angular_velocity(0, 0, 0),
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
		//DOP_6�f�[�^�̍X�V
		virtual void update_dop7() = 0;

	};

	//���̗p�N���X
	class Sphere : public Collider {
	public:
		float r = 1; //���a

		Sphere(float r, float density, vector3 pos = vector3(0, 0, 0)) : r(r) {
			//shape�̐ݒ�
			shape = Collider_shape::shape_sphere;

			//���x�̕ۑ�
			this->density = density;

			//���W
			local_position = pos;

			local_orientation = quaternion_identity();

			//���ʂ̌v�Z
			inertial_mass = 4.0f / 3.0f * r * r * r * DirectX::XM_PI * density;

			//�������[�����g�̌v�Z
			inertial_tensor = matrix_identity();
			inertial_tensor._11 = 0.4f * inertial_mass * r * r;
			inertial_tensor._22 = 0.4f * inertial_mass * r * r;
			inertial_tensor._33 = 0.4f * inertial_mass * r * r;


		}

		//�T�C�Y�̏����֐��̃I�[�o�[���C�h
		quaternion get_dimension() const {
			return quaternion(r, r, r);
		}
		//world�ϊ��֐��̃I�[�o�[���C�h
		void update_world_trans();
		//size�▧�x���ύX�����Ǝ��ʂ⊮�����[�����g�̕ύX���K�v�ɂȂ邩�炻�̂��߂ɗp��(���̔��a = size.x)
		void update_inertial(const vector3& size, float density = 1) {
			this->density = density;

			float r = size.x;
			//���ʂ̌v�Z
			inertial_mass = 4.0f / 3.0f * r * r * r * DirectX::XM_PI * density;

			//�������[�����g�̌v�Z
			inertial_tensor = matrix_identity();
			inertial_tensor._11 = 0.4f * inertial_mass * r * r;
			inertial_tensor._22 = 0.4f * inertial_mass * r * r;
			inertial_tensor._33 = 0.4f * inertial_mass * r * r;
		}
		//dop6�̍X�V
		void update_dop7();

	};

	//���ʗp�N���X
	class Plane : public Collider {
	public:
		//�s���I�u�W�F�N�g�Ƃ��Đ���
		Plane(vector3 n, float d) {
			//shape�̐ݒ�
			shape = Collider_shape::shape_plane;

			//���ʂ̌v�Z
			inertial_mass = FLT_MAX;

			//�������[�����g�̌v�Z
			inertial_tensor = matrix_identity();
			inertial_tensor._11 = FLT_MAX;
			inertial_tensor._22 = FLT_MAX;
			inertial_tensor._33 = FLT_MAX;

			//�����Ȃ�
			move = false;

			n = n.unit_vect();
			local_position = n * d;

			vector3 Y(0, 1, 0);
			float angle = acosf(vector3_dot(Y, n));
			vector3 axis;
			axis = vector3_cross(Y, n);
			axis = axis.unit_vect();
			local_orientation = quaternion_angle_axis(angle, axis);
			local_orientation = local_orientation.unit_vect();
		}

		//�T�C�Y�̏����֐��̃I�[�o�[���C�h
		quaternion get_dimension() const {
			return quaternion(1, 0, 1);
		}
		//world�ϊ��֐��̃I�[�o�[���C�h
		void update_world_trans();
		//size�▧�x���ύX�����Ǝ��ʂ⊮�����[�����g�̕ύX���K�v�ɂȂ邩�炻�̂��߂ɗp��(plane�͏����Ȃ�)
		void update_inertial(const vector3& size, float density = 1) {
			//float r = size.x;
			////���ʂ̌v�Z
			//inertial_mass = 4.0f / 3.0f * r * r * r * DirectX::XM_PI * density;

			////�������[�����g�̌v�Z
			//inertial_tensor = matrix_identity();
			//inertial_tensor._11 = 0.4f * inertial_mass * r * r;
			//inertial_tensor._22 = 0.4f * inertial_mass * r * r;
			//inertial_tensor._33 = 0.4f * inertial_mass * r * r;
		}
		//dop6�̍X�V
		void update_dop7();

	};

	//Box�N���X
	class Box : public Collider {
	public:
		vector3 half_size = vector3();

		//�s���I�u�W�F�N�g�Ƃ��Đ���
		Box(vector3 half_size, float density, vector3 pos = vector3(0,0,0)) : half_size(half_size) {
			//shape�̐ݒ�
			shape = Collider_shape::shape_box;

			//���x�̕ۑ�
			this->density = density;

			//���W
			local_position = pos;

			//���ʂ̌v�Z
			inertial_mass = (half_size.x * half_size.y * half_size.z) * 8.0f * density;

			//�������[�����g�̌v�Z
			inertial_tensor = matrix_identity();
			inertial_tensor._11 = 0.3333333f * inertial_mass * ((half_size.y * half_size.y) + (half_size.z * half_size.z));
			inertial_tensor._22 = 0.3333333f * inertial_mass * ((half_size.z * half_size.z) + (half_size.x * half_size.x));
			inertial_tensor._33 = 0.3333333f * inertial_mass * ((half_size.x * half_size.x) + (half_size.y * half_size.y));
		}

		//�T�C�Y�̏����֐��̃I�[�o�[���C�h
		quaternion get_dimension() const {
			return half_size;
		}
		//world�ϊ��֐��̃I�[�o�[���C�h
		void update_world_trans();

		//size�▧�x���ύX�����Ǝ��ʂ⊮�����[�����g�̕ύX���K�v�ɂȂ邩�炻�̂��߂ɗp��
		void update_inertial(const vector3& half_size, float density = 1) {
			this->density = density;

			//���ʂ̌v�Z
			inertial_mass = (half_size.x * half_size.y * half_size.z) * 8.0f * density;

			//�������[�����g�̌v�Z
			inertial_tensor = matrix_identity();
			inertial_tensor._11 = 0.3333333f * inertial_mass * ((half_size.y * half_size.y) + (half_size.z * half_size.z));
			inertial_tensor._22 = 0.3333333f * inertial_mass * ((half_size.z * half_size.z) + (half_size.x * half_size.x));
			inertial_tensor._33 = 0.3333333f * inertial_mass * ((half_size.x * half_size.x) + (half_size.y * half_size.y));
		}
		//dop6�̍X�V
		void update_dop7();

	};

}