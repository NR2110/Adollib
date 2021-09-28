#pragma once

#include "../Math/math.h"
#include "collider_types.h"

namespace Adollib {
	namespace Physics_function {

		struct Meshcollider_data;
		class ALP_Collider;
	}

	class Collider_shape {

	public:
		Vector3 local_position;
		Quaternion local_orientation;
		Vector3 local_scale;

	private:
		Vector3	world_position_;
		Quaternion	world_orientation_;
		Vector3	world_scale_;

		Vector3 old_world_position_;
		Quaternion old_world_orientation_;
		Vector3 old_world_scale_;

	public:
		//bool is_update_world_trans = false;

	public:
		const Vector3	world_position()const { return world_position_; };
		const Quaternion	world_orientation()const { return world_orientation_; };
		const Vector3	world_scale()const { return world_scale_; };

		const Vector3	old_world_position()const { return old_world_position_; };
		const Quaternion	old_world_orientation()const { return old_world_orientation_; };
		const Vector3	old_world_scale()const { return old_world_scale_; };

	public:

		//���ʂ��l�����Ȃ��������[�����g  barycenter : ��]�̒��S
		Matrix33 get_tensor(const Vector3& barycenter = Vector3(0)) {
			Matrix33 tensor;
			//shape�̒��S����]���S�Ƃ����������[�����g
			tensor = local_tensor();

			tensor = local_orientation.get_rotate_matrix() * tensor * matrix_inverse(local_orientation.get_rotate_matrix());

			//barycenter����]���S�Ƃ��� shape��local���W�̕����l�� (���s���̒藝)
			Vector3 dis =  local_position - barycenter;
			tensor._11 += (dis.y * dis.y + dis.z * dis.z);
			tensor._22 += (dis.z * dis.z + dis.x * dis.x);
			tensor._33 += (dis.x * dis.x + dis.y * dis.y);

			return tensor;
		};

		//shape�̑̐�
		virtual const float get_volume() const = 0;

	protected:
		//����shape���ǂ̂悤�Ȍ`�Ȃ̂�
		Physics_function::ALPCollider_shape_type shape_tag = Physics_function::ALPCollider_shape_type::None;

		//DOP
		DOP::DOP_14	dop14;	//DOP �f�[�^

		//meshcollider�p vertexes,Edges,Facets�Ȃǂ̏�� resorce_manager�̒��ɂ�����ւ̃|�C���^
		Physics_function::Meshcollider_data* mesh_data = nullptr;

		//����shape���A�^�b�`���ꂽcollider�ւ̃C�e���[�^
		Physics_function::ALP_Collider* ALPcollider_ptr = nullptr;

	public:
		const Physics_function::ALPCollider_shape_type get_shape_tag() const { return shape_tag; };

		const DOP::DOP_14 get_DOP() const { return dop14; };

		const Physics_function::Meshcollider_data* get_mesh_data() const { return mesh_data; };

		//TODO : ALPCollider::oncoll_tag��generate_contact�ŕύX����K�v�����邽��const���O���Ă��� �p�C��
		Physics_function::ALP_Collider* get_ALPcollider() { return ALPcollider_ptr; };

		const u_int get_tag() const; //����spahe���A�^�b�`���ꂽ collider : public component ��tag��Ԃ�

		const u_int get_ignore_tags() const;//����spahe���A�^�b�`���ꂽ collider : public component ��ignore_tags��Ԃ�

	public:

		//world���̍X�V �����Ă����true��ς��� inertial_tensor�X�V�̐�����m�F���邽��
		void update_world_trans(const Vector3& GO_Wposiiton, const Quaternion& GO_Worientation, const Vector3& GO_Wscale,
			bool& is_chenged_position, bool& is_chenged_orientation, bool& is_chenged_scale
		);


	protected:
		//�eshape�̃��[�U�[�p�̏��(box:center,size)����v�Z�p�̏��(position,scale)�Ɏ���
		virtual void adapt_Colliderdata() = 0;

		//shape�̒��S����]���S�Ƃ����������[�����g
		virtual const Matrix33 local_tensor() const = 0;

	public:
		//
		void update_Colliderdata() {
			Vector3 old_pos = local_position;
			Quaternion old_orient = local_orientation;
			Vector3 old_scale = local_scale;

			adapt_Colliderdata();

			if (old_pos != local_position ||
				old_orient != local_orientation ||
				old_scale != local_scale
				) {

			}
		};

		//�q�G�����L�[�ւ̕\��
		virtual void Update_hierarchy() = 0;

		//DOP�̍X�V
		virtual void update_dop14() = 0;


	};



}