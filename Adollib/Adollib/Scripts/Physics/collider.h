#pragma once

#include <vector>
#include <assert.h>
#include <string>
#include <unordered_map>
#include "../Math/math.h"
#include "../Object/component.h"

#include "ALP__tags.h"
//#include "ALP_collider.h"
#include "ALP_physics.h"

#include "../Imgui/imgui_all.h"

#include "ALP_struct_contacted_data.h"

namespace Adollib {

	//�\���p��physics_data ���[�U�[���ȒP�ɕύX�ł���悤��
	struct Physics_data {
		float inertial_mass = 0; //����
		float drag = 0; //��C��R
		float anglar_drag = 0; //��C��R
		float dynamic_friction = 0; //�����C
		float static_friction = 0; //�Ö��C
		float restitution = 0;	 //�����W��
		float linear_sleep_threrhold = 0; //freeze��臒l
		float angula_sleep_threrhold = 0; //freeze��臒l

		bool is_fallable = false; // �����Ȃ�
		bool is_kinematic = false;// �e�������Ȃ�(fall�͂���)
		bool is_kinmatic_anglar = false; // �ق��̕��̂���̉e���ŉ�]���x���ω����Ȃ�
		bool is_kinmatic_linear = false; // �ق��̕��̂���̉e���ŕ��i���x���ω����Ȃ�
		bool is_moveable = false; // �����Ȃ�
		bool is_hitable = false;  // �Փ˂��Ȃ�
		bool is_static = false;  // static���m��oncoll_enter���g���Ȃ� ���ǌy���Ȃ�
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

	private:
		Physics_function::ALP_Physics* ALPphysics_ptr = nullptr;
		Physics_function::ALP_Collider* ALPcollider_ptr = nullptr;

	public:
		//::: ��ŕύX���� :::
		const Vector3 linear_velocity() const { return ALPphysics_ptr->linear_velocity(); }
		const Vector3 angula_velocity() const { return ALPphysics_ptr->angula_velocity(); }
		const void linear_velocity(Vector3 v) {
			ALPphysics_ptr->set_linear_velocity(v);
			ALPphysics_ptr->set_old_linear_velocity(v);
		}
		const void angula_velocity(Vector3 v) {
			ALPphysics_ptr->set_angula_velocity(v);
			ALPphysics_ptr->set_old_angula_velocity(v);
		}

		// �w�肵����_�ł̑��x
		const Vector3 get_point_velocity(const Vector3& pos, bool is_local = false);

		// �A�^�b�`���ꂽjoint�̐�
		const int get_joint_count();
		// �w�肵���ԍ��ɃA�^�b�`����Ă���joint�̏��𓾂�
		Joint_base* get_joint(const int num);

	public:
		// joint�Ɏ��g�̕ێ�����ALPCollider�̏�������
		void set_ptr_to_joint(Physics_function::ALP_Joint* joint_base);

	public:
		// �������Ă�����true��Ԃ�
		const bool concoll_enter(const Collider_tagbit tag_name);

		// ���i�ړ��ɗ͂�������
		void add_force(const Vector3& force, const bool& is_force_local = false);

		// ���i�ړ��ɗ͂�������
		void add_force(const Vector3& force, const Vector3& position, const bool& is_position_local = false, const bool& is_force_local = false);

		// �p��]�ɗ͂�������
		void add_torque(const Vector3& force, const bool& is_local = false);

		// ���i�����ɒl��������
		void add_linear_acc(const Vector3& force);

		// �p�����ɒl��������
		void add_angula_acc(const Vector3& force);

		// ���݂������Ă��鑬�x�A�����x�A�͂�0�ɂ���
		void reset_force() { ALPphysics_ptr->reset_force(); };

		// ���x�������s��
		void set_max_linear_velocity(const float& max_scalar) { ALPphysics_ptr->set_max_linear_velocity(max_scalar); };
		void set_max_angula_velocity(const float& max_scalar) { ALPphysics_ptr->set_max_angula_velocity(max_scalar); };

		// shape�̃A�^�b�`
		template<typename T>
		T* add_shape() { return ALPcollider_ptr->add_shape<T>(); };

		// meshcollider�̃A�^�b�`
		void add_mesh_shape(const char* filepass, bool is_right_rtiangle = true, bool is_permit_edge_have_many_facet = false);

		// �������[�����g�����[�U�[��`�Őݒ肷��
		void set_tensor(const Matrix33& tensor) { ALPphysics_ptr->set_tensor(tensor); };

		// �d�S�����[�U�[��`�Őݒ肷��
		void set_barycenter(const Vector3& cent) { ALPphysics_ptr->set_barycenter(cent); };

		// ���݂̊������[�����g�̒l
		const Matrix33 get_tensor();

		// �d�S��local���W��Ԃ�
		const Vector3 get_barycenter() const;

		// �Փ˂���collider�̔z���Ԃ� is_save_contacted_colls��true�ɂȂ��Ă��Ȃ��ƏՓ˂���collider�̏��͕ۑ�����Ȃ�
		std::vector<Contacted_data> get_Contacted_data() const;

	public:
		void Update_hierarchy();

		void awake() override;

		// user���Ă΂Ȃ��悤��!!
		void finalize() override;

	};
}

#include "shape_box.h"
#include "shape_plane.h"
#include "shape_sphere.h"
#include "shape_capsule.h"