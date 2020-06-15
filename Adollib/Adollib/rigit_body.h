#pragma once

#include <vector>
#include <assert.h>
#include "quaternion.h"
#include "matrix.h"

//�ђʂ��Ȃ��Փ˔����p���邩(�����d���Ȃ�?)
//�����p����ꍇintegral�O�ɏՓ˔�����s��
//������
//����Ȃ�
#define USE_CHECK_CONTACT 0

namespace Adollib {

	class Gameobject;

	enum class Rigitbody_shape {
		shape_box,
		shape_sphere,
		shape_plane,

		shape_null

	};

	class Rigitbody {
	public:
		bool move = true; //���I�u�W�F�N�g�J�ǂ����̃t���O

		Gameobject* gameobject = nullptr;	//�e���

		Rigitbody_shape shape = Rigitbody_shape::shape_null;	//�`���

		vector3 world_position = vector3();		     //���[���h��Ԃł̍��W
		quaternion world_orientation = vector3();    //���[���h��Ԃł̎p��
		vector3 world_size = vector3();              //���[���h��Ԃł̑傫��

		vector3 local_position = vector3();             //go����̑��΍��W
		quaternion local_orientation = quaternion_identity();       //go����̑��Ύp��
		vector3 local_scale = vector3();				//go�Ƃ̑���scale
		float density = 1;						//���x

		vector3 linear_velocity = vector3();    //���i���x
		vector3 angular_velocity = vector3();   //�p���x

		vector3 liner_acceleration = vector3();  //����
		vector3 angular_acceleration = vector3();//�p����

		float inertial_mass = 0;           //��������

		vector3 accumulated_force = vector3();  //���i�ړ��ɉ������

		matrix inertial_tensor = matrix();      //�������[�����g

		vector3 accumulated_torque = vector3(); //�p��]�ɉ������

	//	std::vector<Contact*> contact;  //����collider���������Փ˂ւ̃|�C���^
		//vector3 extruction = vector3();				//����collider���󂯂鉟���o����

		Rigitbody::Rigitbody() :
			local_position(0, 0, 0), local_orientation(0, 0, 0, 1),
			linear_velocity(0, 0, 0), angular_velocity(0, 0, 0),
			inertial_mass(1), accumulated_force(0, 0, 0),
			accumulated_torque(0, 0, 0)
		{
			//inertial_tensor = ();
		}

		void integrate(float duration = 1);
		void late_integrate(float duration = 1);

		void resolve_gameobject(); //gameobject�֕ω��ʂ�n��

		virtual void update_world_trans() = 0; //gameobject��transform����collider��world��Ԃł̏����X�V

		void add_force(const vector3& force); //���i�ړ��ɗ͂�������

		void add_torque(const vector3& force); //�p��]�ɗ͂�������

		bool is_movable() const; //���I�u�W�F�N�g���ǂ���

		float inverse_mass() const; //���ʂ̋t����Ԃ�(�s�ғ��I�u�W�F�N�g��0��Ԃ�)

		matrix inverse_inertial_tensor() const; //�������[�����g�̋t�s���Ԃ�

		virtual quaternion get_dimension() const = 0; //�T�C�Y�����֐�

		virtual void update_inertial(const vector3& size, float density = 1) = 0;

	};

	//���̗p�N���X
	class Sphere : public Rigitbody {
	public:
		float r = 1; //���a

		Sphere(float r, float density, vector3 pos = vector3(0, 0, 0)) : r(r) {
			//shape�̐ݒ�
			shape = Rigitbody_shape::shape_sphere;

			//���x�̕ۑ�
			this->density = density;

			//���W
			local_position = pos;

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

	};

	//���ʗp�N���X
	class Plane : public Rigitbody {
	public:
		//�s���I�u�W�F�N�g�Ƃ��Đ���
		Plane(vector3 n, float d) {
			//shape�̐ݒ�
			shape = Rigitbody_shape::shape_plane;

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

	};

	//Box�N���X
	class Box : public Rigitbody {
	public:
		vector3 half_size = vector3();

		//�s���I�u�W�F�N�g�Ƃ��Đ���
		Box(vector3 half_size, float density, vector3 pos = vector3(0,0,0)) : half_size(half_size) {
			//shape�̐ݒ�
			shape = Rigitbody_shape::shape_box;

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

	};

	//�Փ˗p��namespace ���̐l�����܂�G��Ȃ��悤��
	namespace Contacts {

		//�Փˏ����p
		struct Contact {

			Rigitbody* body[2]; //�ڐG����object

			vector3 point; //�Փ˓_
			vector3 normal; //�Փ˖ʂ̖@��
			float penetration; //�ђʗ�
			float restitution; //���ˌW��

			Contact() : point(0, 0, 0), normal(0, 0, 0), penetration(0), restitution(0) {
				body[0] = body[1] = 0;
			}

			void resolve();
		};

		//�Փ˂̊m�F
		//bool check_contact_sphere_sphere(Sphere& S1, Sphere& S2);
		//bool check_contact_sphere_plane(Sphere& S1, Plane& S2);
		//bool check_contact_sphere_box(Sphere& S1, Box& S2);
		//bool check_contact_box_plane(Box& S1, Plane& S2);
		//bool check_contact_box_box(Box& S1, Box& S2);

		//�Փ˂̐���
		int generate_contact_sphere_sphere(Sphere& S1, Sphere& S2, std::vector<Contact>& contacts, float restitution);
		int generate_contact_sphere_plane(Sphere& S1, Plane& S2, std::vector<Contact>& contacts, float restitution, bool half_space = true);
		int generate_contact_sphere_box(Sphere& S1, Box& S2, std::vector<Contact>& contacts, float restitution);
		int generate_contact_box_plane(Box& S1, Plane& S2, std::vector<Contact>& contacts, float restitution);
		int generate_contact_box_box(Box& S1, Box& S2, std::vector<Contact>& contacts, float restitution);
	}












}