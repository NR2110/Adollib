#pragma once

#include <vector>
#include <assert.h>
#include <string>
#include <unordered_map>

#include "ALP__tags.h"
#include "ALP_Collider.h"
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
}

namespace Adollib {
	class Joint_base;

	namespace Physics_function {
		class ALP_Joint;
		class ALP_Collider;
		class ALP_Physics;
		namespace Contacts {
			struct Contact_pair;
		}
	}


	class Collider {

	public:
		//::: tag ::::::::
		Collider_tagbit tag = 0; //���g��tag(bit)
		Collider_tagbit ignore_tags = 0; //�Փ˂��Ȃ�tags(bit)

		//::: unity��physics���� ������K�v�Ȃ�ĂȂ���� ::::
		Physics_data physics_data;

		//::: ���g�̊ւ��contact_pair�̏��������o�ɕۑ����邩�ǂ��� :::
		bool is_save_contacted_colls = false;

		//::: transform(����������world�����Ȃ̂� ����Collider�̐e����inverse���g��)
		DirectX::XMFLOAT3 Wposition;
		DirectX::XMFLOAT4 Worientation;
		DirectX::XMFLOAT3 Wscale;
		DirectX::XMFLOAT4 pearent_Worientation_inverse;

	private:
		Physics_function::ALP_Physics* ALPphysics_ptr = nullptr;
		Physics_function::ALP_Collider* ALPcollider_ptr = nullptr;

	public:
		//::: ��ŕύX���� :::
		const DirectX::XMFLOAT3 linear_velocity() const;
		const DirectX::XMFLOAT3 angula_velocity() const;
		const void linear_velocity(DirectX::XMFLOAT3 v);
		const void angula_velocity(DirectX::XMFLOAT3 v);
		// �w�肵����_�ł̑��x
		const DirectX::XMFLOAT3 get_point_velocity(const DirectX::XMFLOAT3& pos, bool is_local = false);

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
		void add_force(const DirectX::XMFLOAT3& force, const float& delta_time, const bool& is_force_local = false);

		// ���i�ړ��ɗ͂�������
		void add_force(const DirectX::XMFLOAT3& force, const DirectX::XMFLOAT3& position, const float& delta_time, const bool& is_position_local = false, const bool& is_force_local = false);

		// �p��]�ɗ͂�������
		void add_torque(const DirectX::XMFLOAT3& force, const float& delta_time, const bool& is_local = false);

		// ���i�����ɒl��������
		void add_linear_acc(const DirectX::XMFLOAT3& force, const float& delta_time);

		// �p�����ɒl��������
		void add_angula_acc(const DirectX::XMFLOAT3& force, const float& delta_time);

		// ���݂������Ă��鑬�x�A�����x�A�͂�0�ɂ���
		void reset_force();

		// ���x�������s��
		void set_max_linear_velocity(const float& max_scalar);
		void set_max_angula_velocity(const float& max_scalar);

		// shape�̃A�^�b�`
		template<typename T>
		T* add_shape() { return ALPcollider_ptr->add_shape<T>(); };

		// meshcollider�̃A�^�b�`
		void add_mesh_shape(const char* filepass, bool is_right_rtiangle = true, bool is_permit_edge_have_many_facet = false);

		// �������[�����g�����[�U�[��`�Őݒ肷��
		void set_tensor(const Physics_function::Matrix33& tensor);

		// �d�S�����[�U�[��`�Őݒ肷��
		void set_barycenter(const DirectX::XMFLOAT3& cent);

		// ���݂̊������[�����g�̒l
		const Physics_function::Matrix33 get_tensor();

		// �d�S��local���W��Ԃ�
		const DirectX::XMFLOAT3 get_barycenter() const;

		// �Փ˂���collider�̔z���Ԃ� is_save_contacted_colls��true�ɂȂ��Ă��Ȃ��ƏՓ˂���collider�̏��͕ۑ�����Ȃ�
		std::vector<Contacted_data> get_Contacted_data() const;

	public:

		//:::::::::
		// Goptr : ���j�[�N�Ȓl�Ȃ�Ȃ�ł�OK �����l�������̂Ƃ͏Փ˂��Ȃ�
		// Wpos, Worient, Wscale : world transform �̏����l
		// pearent_Worient_inv : ����������World���W�n�ł̗��������Ȃ̂� �ω��ʂ̎Z�o���ɐe�̉�]��inv��������
		//:::::::::
		void awake(const void* Goptr, const DirectX::XMFLOAT3& Wpos, const DirectX::XMFLOAT4& Worient, const DirectX::XMFLOAT3& Wscale, const DirectX::XMFLOAT4& pearent_Worient_inv);

		void update();

		void update_get_amount_of_change(DirectX::XMFLOAT3& pos_amount_of_change, DirectX::XMFLOAT4& orient_amount_of_change);

		void update_set_Wtransform(const DirectX::XMFLOAT3& Wpos, const DirectX::XMFLOAT4& Worient, const DirectX::XMFLOAT3& Wscale, const DirectX::XMFLOAT4& pearent_Worient_inv = DirectX::XMFLOAT4(0,0,0,0));

		void Update_hierarchy();

		void finalize();

	};
}

#include "shape_box.h"
#include "shape_plane.h"
#include "shape_sphere.h"
#include "shape_capsule.h"