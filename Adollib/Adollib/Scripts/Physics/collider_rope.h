#pragma once

#include <vector>
#include <assert.h>
#include <string>
#include <unordered_map>
#include <map>
#include "../Math/math.h"
#include "../Object/component.h"

#include "ALP__tags.h"
//#include "ALP_collider.h"
#include "ALP_physics.h"

#include "collider.h"

#include "../Imgui/imgui_all.h"

namespace Adollib {

	namespace Physics_function {
		class ALP_Joint;
		class ALP_Collider;
		class ALP_Physics;
		struct Meshcollider_data;
		namespace Contacts {
			struct Contact_pair;
		}
	}

	class Collider_Rope : public Component {

	public:
		//::: tag ::::::::
		Collider_tagbit tag = 0; //���g��tag(bit)
		Collider_tagbit ignore_tags = 0; //�Փ˂��Ȃ�tags(bit)

		//::: �A�^�b�`����collider��physics�̏����l ::::
		Physics_data default_physics_data;

		//::: rope��sphere�̌� ::::
		int sphere_num_size = 5;

		//::: rope��sphere�̔��a ::::
		float sphere_size_r = 0.01f;

		//::: rope��sphree�̋��� ::::
		float sphree_offset_size = 0.08f;


		//::: static���m�ŏՓ˔�����s��Ȃ� :::
		bool is_static = false;

		//::: ���g�̊ւ��contact_pair�̏��������o�ɕۑ����邩�ǂ��� :::
		bool is_save_contacted_colls = false;

	private:
		// �K����struct
		enum class Rope_constraint_type {
			sructural_spring, //�\���o�l
			bending_spring //�Ȃ��o�l
		};

	private:
		std::vector<Collider*> colliders; //���_���ɃA�^�b�`����collider�̔z��

		std::unordered_map<Rope_constraint_type, std::vector<Joint_base*>> joints; //�z�u����collider�̔z��

		std::shared_ptr<std::vector<std::pair<Vector3, Vector3>>> vertex_offset; //model���_����ǂꂭ�炢����Ă��邩 renderer��position,normal��std::pair�Ȃ̂ō��킹��


	public:
		// �A�^�b�`���ꂽjoint�̐�
		const int get_joint_count();
		// �w�肵���ԍ��ɃA�^�b�`����Ă���joint�̏��𓾂�
		Joint_base* get_joint(const int num);

		std::shared_ptr<std::vector<std::pair<Vector3, Vector3>>> get_vertex_offset() { return vertex_offset; };

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
		void reset_force() {for (auto& ptr : colliders)  ptr->reset_force(); };

		// ���x�������s��
		void set_max_linear_velocity(const float& max_scalar) {for (auto& ptr : colliders)  ptr->set_max_linear_velocity(max_scalar); };
		void set_max_angula_velocity(const float& max_scalar) {for (auto& ptr : colliders)  ptr->set_max_angula_velocity(max_scalar); };

		// �������[�����g�����[�U�[��`�Őݒ肷��
		void set_tensor(const Matrix33& tensor) {for (auto& ptr : colliders)  ptr->set_tensor(tensor); };

		// �d�S�����[�U�[��`�Őݒ肷��
		void set_barycenter(const Vector3& cent) {for (auto& ptr : colliders)  ptr->set_barycenter(cent); };

		// �d�S��local���W��Ԃ�
		const Vector3 get_barycenter() const;

		// �w��̒��_��physics_data��ύX����
		void set_vertex_data(const int& vertex_num, const Physics_data& physics_data);
		// �w��̒��_��physics_data�𓾂�
		const Physics_data& get_vertex_data(const int& vertex_num) const;

	public:
		void create_rope();

		void Update_hierarchy() override;

		void awake() override;

		void update() override;

		// user���Ă΂Ȃ��悤��!!
		void finalize() override;

	};





}