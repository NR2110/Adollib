#pragma once

#include <vector>
#include <assert.h>
#include <string>
#include <unordered_map>
#include "../Math/math.h"
#include "../Object/component.h"

namespace Adollib {

	namespace  physics_function {
		class Solverbody;
	}
	namespace Contacts {
		class Contact;
	}


	class Collider : public Component {
	public:
		Vector3 local_position;
		Vector3 local_orientation;
		Vector3 local_scale;

		float mass; //����
		float drag; //��C��R
		float anglar_drag; //��C��R
		float dynamic_friction; //�����C
		float static_friction; //�Ö��C
		float restitution;	 //�����W��

		bool is_fallable; //�����Ȃ�
		bool is_kinematic; //�e�������Ȃ�(fall�͂���)
		bool is_moveable; //�����Ȃ�


	private:
		//itr
		
	public:
		bool concoll_enter(std::string tag_name);

		////�O�͂̍X�V
		//void apply_external_force(float duration = 1);

		////���x�A�����x��0�ɂ���
		//void reset_force();

		////���W,�p���̍X�V
		//void integrate(float duration = 1);

		////gameobject�ւ̕ω��ʂ����߂�
		//void solv_resolve();

		////gameobject�֕ω��ʂ�n��
		//void resolve_gameobject();

		////gameobject��transform����collider��world��Ԃł̏����X�V
		//virtual void update_world_trans() = 0;

		////���i�ړ��ɗ͂�������
		//void add_force(const Vector3& force);

		////�p��]�ɗ͂�������
		//void add_torque(const Vector3& force);

		////���I�u�W�F�N�g���ǂ���
		//bool is_movable() const;

		////�����I�u�W�F�N�g���ǂ���
		//bool is_fallable() const;

		////���ʂ̋t����Ԃ�(�s�ғ��I�u�W�F�N�g��0��Ԃ�)
		//float inverse_mass() const;

		////�������[�����g�̋t�s���Ԃ�
		//matrix inverse_inertial_tensor() const;

		////�T�C�Y�����֐�
		//virtual Quaternion get_dimension() const = 0;

		////�T�C�Y�ύX�ȂǂɑΉ����邽�ߖ��t���[�������e���\���Ȃǂ��X�V
		//virtual void update_inertial(const Vector3& size, float density = 1) = 0;

		////�I�u�W�F�N�g���������тɌĂяo���@�̂������悢���K���ɖ��t���[���Ă�
		////DOP_14�f�[�^�̍X�V
		//virtual void update_dop14() = 0;

		void awake() override;

		void finalize() override;

	};
}