#pragma once

#include <vector>
#include <assert.h>
#include <string>
#include <unordered_map>
#include <map>
#include "../Math/math.h"

#include "ALP__tags.h"
//#include "ALP_collider.h"
#include "ALP_physics.h"

#include "collider.h"

namespace Adollib {

	////�\���p��physics_data ���[�U�[���ȒP�ɕύX�ł���悤��
	//struct Physics_data {
	//	float inertial_mass = 0; //����
	//	float drag = 0; //��C��R
	//	float anglar_drag = 0; //��C��R
	//	float dynamic_friction = 0; //�����C
	//	float static_friction = 0; //�Ö��C
	//	float restitution = 0;	 //�����W��

	//	bool is_fallable = false; // �����Ȃ�
	//	bool is_kinematic = false;// �e�������Ȃ�(fall�͂���)
	//	bool is_kinmatic_anglar = false; // �ق��̕��̂���̉e���ŉ�]���x���ω����Ȃ�
	//	bool is_kinmatic_linear = false; // �ق��̕��̂���̉e���ŕ��i���x���ω����Ȃ�
	//	bool is_moveable = false; // �����Ȃ�
	//	bool is_hitable = false;  // �Փ˂��Ȃ�
	//	bool is_static = false;  // static���m��oncoll_enter���g���Ȃ� ���ǌy���Ȃ�
	//};
	////:::::::::::::::::::::::::


	namespace Physics_function {
		class ALP_Joint;
		class ALP_Collider;
		class ALP_Physics;
		struct Meshcollider_data;
		namespace Contacts {
			struct Contact_pair;
		}
	}

	class Collider_Croth  {

	public:
		//::: tag ::::::::
		Collider_tagbit tag = 0; //���g��tag(bit)
		Collider_tagbit ignore_tags = 0; //�Փ˂��Ȃ�tags(bit)

		//::: �A�^�b�`����collider��physics�̏����l ::::
		Physics_data default_physics_data;

		//::: �A�^�b�`����sphere�̔��a�̏����l ::::
		float default_sphere_r = 0.01f;

		//::: static���m�ŏՓ˔�����s��Ȃ� :::
		bool is_static = false;

		//::: ���g�̊ւ��contact_pair�̏��������o�ɕۑ����邩�ǂ��� :::
		bool is_save_contacted_colls = false;

	private:
		// �K����struct
		enum class croth_constraint_type {
			sructural_spring, //�\���o�l
			shear_spring, //����f�o�l
			bending_spring //�Ȃ��o�l
		};
		struct Croth_constraint {

			int vertId0 = 0; //���_
			int vertId1 = 0; //���_
			Physics_function::Meshcollider_data* mesh = nullptr;

			float natural_length = 0; //�΂˂̎��R��
			croth_constraint_type constraint_type = croth_constraint_type::bending_spring;
		};

	private:
		std::map<Physics_function::Meshcollider_data*, std::vector<Collider*>> colliders; //���_���ɃA�^�b�`����collider�̔z��

		std::shared_ptr<std::vector<std::vector<std::pair<Vector3, Vector3>>>> vertex_offset; //model���_����ǂꂭ�炢����Ă��邩 renderer��position,normal��std::pair�Ȃ̂ō��킹��

		std::vector<Croth_constraint> croth_constraints; //�\���΂�

		std::vector<Physics_function::Meshcollider_data>* meshcoll_data = nullptr;

	public:
		// �A�^�b�`���ꂽjoint�̐�
		const int get_joint_count();
		// �w�肵���ԍ��ɃA�^�b�`����Ă���joint�̏��𓾂�
		Joint_base* get_joint(const int num);

		std::shared_ptr<std::vector<std::vector<std::pair<Vector3, Vector3>>>> get_vertex_offset() { return vertex_offset; };

	public:
		// �������Ă�����true��Ԃ�
		const bool concoll_enter(const Collider_tagbit tag_name);

		// ���i�ړ��ɗ͂�������
		void add_force(const Vector3& force, const float& delta_time, const bool& is_force_local = false);

		// ���i�ړ��ɗ͂�������
		void add_force(const Vector3& force, const Vector3& position, const float& delta_time, const bool& is_position_local = false, const bool& is_force_local = false);

		// �p��]�ɗ͂�������
		void add_torque(const Vector3& force, const float& delta_time, const bool& is_local = false);

		// ���i�����ɒl��������
		void add_linear_acc(const Vector3& force, const float& delta_time);

		// �p�����ɒl��������
		void add_angula_acc(const Vector3& force, const float& delta_time);

		// ���݂������Ă��鑬�x�A�����x�A�͂�0�ɂ���
		void reset_force() { for (auto& map : colliders) for (auto ptr : map.second) ptr->reset_force(); };

		// ���x�������s��
		void set_max_linear_velocity(const float& max_scalar) {for(auto& map : colliders) for (auto ptr : map.second) ptr->set_max_linear_velocity(max_scalar); };
		void set_max_angula_velocity(const float& max_scalar) {for(auto& map : colliders) for (auto ptr : map.second) ptr->set_max_angula_velocity(max_scalar); };

		// �������[�����g�����[�U�[��`�Őݒ肷��
		void set_tensor(const Matrix33& tensor) { for (auto& map : colliders) for (auto ptr : map.second) ptr->set_tensor(tensor); };

		// �d�S�����[�U�[��`�Őݒ肷��
		void set_barycenter(const Vector3& cent) { for (auto& map : colliders) for (auto ptr : map.second) ptr->set_barycenter(cent); };

		// �d�S��local���W��Ԃ�
		const Vector3 get_barycenter() const;

		// �w��̒��_��physics_data��ύX����
		void set_vertex_data(const int& mesh_num, const int& vertex_num, const Physics_data& physics_data);
		// �w��̒��_��physics_data�𓾂�
		const Physics_data& get_vertex_data(const int& mesh_num, const int& vertex_num) const;

	public:
		void load_file(const std::string& filename, bool is_right_rtiangle, bool is_permit_edge_have_many_facet);

		void Update_hierarchy();

		void update();

		void awake();

		void finalize();

	};
}
