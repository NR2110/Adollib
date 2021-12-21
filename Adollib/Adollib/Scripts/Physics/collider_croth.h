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

namespace Adollib {

	//�\���p��physics_data ���[�U�[���ȒP�ɕύX�ł���悤��
	struct Physics_data {
		float inertial_mass = 0; //����
		float drag = 0; //��C��R
		float anglar_drag = 0; //��C��R
		float dynamic_friction = 0; //�����C
		float static_friction = 0; //�Ö��C
		float restitution = 0;	 //�����W��

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

	enum class croth_constraint_type {
		sructural_spring, //�\���o�l
		shear_spring, //����f�o�l
		bending_spring //�Ȃ��o�l
	};
	struct Croth_constraint {

		int vertId0 = 0; //���_
		int vertId1 = 0; //���_

		float natural_length = 0; //�΂˂̎��R��
		croth_constraint_type constraint_type = croth_constraint_type::bending_spring;
	};

	class Collider_Croth : public Component {

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
		std::vector<Collider*> colliders; //���_���ɃA�^�b�`����collider�̔z��

		std::shared_ptr<std::vector<Vector3>> vertex_offset; //model���_����ǂꂭ�炢����Ă��邩

		std::vector<Croth_constraint> croth_constraints; //�\���΂�

	public:
		// �A�^�b�`���ꂽjoint�̐�
		const int get_joint_count();
		// �w�肵���ԍ��ɃA�^�b�`����Ă���joint�̏��𓾂�
		Joint_base* get_joint(const int num);

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
		void reset_force() { for(auto ptr : colliders) ptr->reset_force(); };

		// ���x�������s��
		void set_max_linear_velocity(const float& max_scalar) { for (auto ptr : colliders) ptr->set_max_linear_velocity(max_scalar); };
		void set_max_angula_velocity(const float& max_scalar) { for (auto ptr : colliders) ptr->set_max_angula_velocity(max_scalar); };

		// shape�̃A�^�b�`
		template<typename T>
		T* add_shape() { return ALPcollider_ptr->add_shape<T>(); };

		// �������[�����g�����[�U�[��`�Őݒ肷��
		void set_tensor(const Matrix33& tensor) { for (auto ptr : colliders) ptr->set_tensor(tensor); };

		// �d�S�����[�U�[��`�Őݒ肷��
		void set_barycenter(const Vector3& cent) { for (auto ptr : colliders) ptr->set_barycenter(cent); };

		// �d�S��local���W��Ԃ�
		const Vector3 get_barycenter() const;

	public:
		void load_file(const std::string& filename,bool is_right_rtiangle,bool is_permit_edge_have_many_facet);

		void Update_hierarchy() override;

		void update() override;

		// user���Ă΂Ȃ��悤��!!
		void finalize() override;

	};
}
