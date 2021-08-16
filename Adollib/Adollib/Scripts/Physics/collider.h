#pragma once

#include <vector>
#include <assert.h>
#include <string>
#include <unordered_map>
#include "../Math/math.h"
#include "../Object/component.h"

#include "ALP__tags.h"
#include "ALP_collider.h"
#include "ALP_physics.h"

#include "../Imgui/imgui_all.h"

namespace Adollib {
	class Collider;



	//�\���p��physics_data ���[�U�[���ȒP�ɕύX�ł���悤��
	struct Physics_data {
		float inertial_mass = 0; //����
		float drag = 0; //��C��R
		float anglar_drag = 0; //��C��R
		float dynamic_friction = 0; //�����C
		float static_friction = 0; //�Ö��C
		float restitution = 0;	 //�����W��

		bool is_fallable = 0; //�����Ȃ�
		bool is_kinematic = 0;//�e�������Ȃ�(fall�͂���)
		bool is_kinmatic_anglar = 0; //�ق��̕��̂���̉e���ŉ�]���x���ω����Ȃ�
		bool is_kinmatic_linear = 0; //�ق��̕��̂���̉e���ŕ��i���x���ω����Ȃ�
		bool is_moveable = 0; //�����Ȃ�
		bool is_hitable = 0;  //�Փ˂��Ȃ�
	};

	struct Contacted_data {
		Collider* coll; //�����collider
		float penetrate = 0; //�ђʗ�
		Vector3 normal; //�Փ˖@�� world���W�n
		Vector3 contacted_pointA; //���g��GO���W�n�̏Փ˓_
		Vector3 contacted_pointB; //�����GO���W�n�̏Փ˓_
	};
	//:::::::::::::::::::::::::



	namespace Physics_function {
		class ALP_Joint;
		class ALP_Collider;
		class ALP_Physics;
		namespace Contacts {
			struct Contact_pair;
		}
	}


	class Collider : public Component {

	public:
		//::: tag ::::::::
		Collider_tagbit tag = 0; //���g��tag(bit)
		Collider_tagbit ignore_tags = 0; //�Փ˂��Ȃ�tags(bit)

		//::: unity��physics���� ������K�v�Ȃ�ĂȂ���� ::::
		Physics_data physics_data;

		//::: static���m�ŏՓ˔�����s��Ȃ� :::
		bool is_static = false;

		//::: ���g�̊ւ��contact_pair�̏��������o�ɕۑ����邩�ǂ��� :::
		bool is_save_contacted_colls = false;

		//::: is_save_contacted_colls��true�̎� ���g�̊ւ��contact_pair�̏���ۑ����� :::
		std::vector<Contacted_data> contacted_colliders;

	private:
		Physics_function::ALP_Collider* ALPcollider_ptr = nullptr;
		Physics_function::ALP_Physics* ALPphysics_ptr = nullptr;

	public:
		//::: ��ŕύX���� :::
		const Vector3 linear_velocity() const { return ALPphysics_ptr->linear_velocity; }
		const Vector3 linear_velocity(Vector3 v) {
			ALPphysics_ptr->linear_velocity = v;
			return ALPphysics_ptr->linear_velocity;
		}
		const Vector3 angula_velocity() const { return ALPphysics_ptr->angula_velocity; }
		const Vector3 angula_velocity(Vector3 v) {
			ALPphysics_ptr->angula_velocity = v;
			return ALPphysics_ptr->angula_velocity;
		}

	public:
		//joint�Ɏ��g�̕ێ�����ALPCollider�̏�������
		void set_ptr_to_joint(Physics_function::ALP_Joint*& joint_base);

	public:
		//�������Ă�����true��Ԃ�
		const bool concoll_enter(const unsigned int tag_name) const;

		//���i�ړ��ɗ͂�������
		void add_force(const Vector3& force);

		//�p��]�ɗ͂�������
		void add_torque(const Vector3& force);

		//shape�̃A�^�b�`
		template<typename T>
		T* add_shape() { return ALPcollider_ptr->add_shape<T>(); };

		//meshcollider�̃A�^�b�`
		void add_shape(const char* filepass, bool is_right_rtiangle = true, bool is_permit_edge_have_many_facet = false);

		//�������[�����g�����[�U�[��`�Őݒ肷��
		void set_tensor(const Matrix33& tensor) { ALPphysics_ptr->set_tensor(tensor); };

		void set_barycenter(const Vector3& cent) { ALPphysics_ptr->set_barycenter(cent); };

		//�d�S��local���W��Ԃ�
		const Vector3 get_barycenter() const { return ALPphysics_ptr->get_barycenter(); };

	public:
		void Update_hierarchy();

		void awake() override;

		void finalize() override;

	};
}

#include "collider_box.h"
#include "collider_plane.h"
#include "collider_sphere.h"
#include "collider_capsule.h"
#include "collider_meshcoll.h"