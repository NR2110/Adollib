#pragma once

#include "../Math/math.h"


namespace Adollib {
	namespace Physics_function {

		//::: �v�Z�ȂǂɎg����� :::
		//struct Collider_data {
		//	Vector3 local_position = Vector3(0, 0, 0);
		//	Quaternion local_orientation = quaternion_identity();
		//	Vector3 local_scale = Vector3(1, 1, 1);

		//	Vector3 half_size;

		//	u_int tag = 0;
		//	u_int ignore_tags = 0;

		//	Physics_function::ALP_Collider_shape_tag shape = Physics_function::ALP_Collider_shape_tag::None; //�`���

		//};

		enum class ALP_Collider_shape_tag {
			None,
			BOX,
			Sphere,
			Plane,
			Capsule,
			Mesh,
			Cylinder,

			shapes_size
		};

		class Meshcollider_data;
		class ALP_Collider;
	}
	class ALP_shape {
	public:

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
		const Vector3	world_position()const { return world_position_; };
		const Quaternion	world_orientation()const { return world_orientation_; };
		const Vector3	world_scale()const { return world_scale_; };

		const Vector3	old_world_position()const { return old_world_position_; };
		const Quaternion	old_world_orientation()const { return old_world_orientation_; };
		const Vector3	old_world_scale()const { return old_world_scale_; };

	protected:
		//����shape���ǂ̂悤�Ȍ`�Ȃ̂�
		Physics_function::ALP_Collider_shape_tag shape_tag;

		//14-DOP
		DOP::DOP_14	dop14;	//14DOP �f�[�^

		//meshcollider�p vertexes,Edges,Facets�Ȃǂ̏�� resorce_manager�̒��ɂ�����ւ̃|�C���^
		Physics_function::Meshcollider_data* mesh_data;

		//����shape���A�^�b�`���ꂽcollider�ւ̃C�e���[�^
		Physics_function::ALP_Collider* ALPcollider_ptr;

	public:

		const Physics_function::ALP_Collider_shape_tag get_shape_tag() const { return shape_tag; };

		const DOP::DOP_14 get_DOP() const { return dop14; };

		const Physics_function::Meshcollider_data* get_mesh_data() const { return mesh_data; };

		//TODO : ALPCollider::oncoll_tag��generate_contact�ŕύX����K�v�����邽��const���O���Ă��� �p�C��
		Physics_function::ALP_Collider* get_ALPcollider() const { return ALPcollider_ptr; };

	public:
		u_int tag = 0; //���g��tag(bit)
		u_int ignore_tags = 0; //�Փ˂��Ȃ�tags(bit)



	public:

		//world���̍X�V
		void update_world_trans(const Vector3& GO_Wposiiton, const Quaternion& GO_Worientation, const Vector3& GO_Wscale);

	public:
		//�eshape�̃��[�U�[�p�̏��(box:center,size)����v�Z�p�̏��(position,scale)�Ɏ���
		virtual void adapt_Colliderdata() = 0;

		//�q�G�����L�[�ւ̕\��
		virtual void Update_hierarchy() = 0;

		//DOP�̍X�V
		virtual void update_dop14() = 0;

	};



}