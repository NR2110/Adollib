#include "ALP_Collider.h"

#include "../Object/gameobject.h"
#include "ALP__physics_manager.h"

#include "collider_shape.h"
#include "ALP_physics.h"
#include "ALP_joint.h"
#include "ALP__meshcoll_data.h"

using namespace Adollib;
using namespace Physics_function;


//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
#pragma region Collider
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
const bool ALP_Collider::concoll_enter(const unsigned int tag_name) {
	oncoll_check_bits |= tag_name;
	return (oncoll_bits & tag_name);
}

void ALP_Collider::update_world_trans() {
	bool is_changed_Size = false;

	bool is_changPos = false;
	bool is_changRot = false;
	bool is_changSiz = false;

	for (auto& shape : shapes) {

		// ���[�U�[�ɓ��͂��ꂽcollider�f�[�^(center,size�Ȃ�)���v�Z�p�̃f�[�^�Ɏ���
		shape->update_Colliderdata();

		// world���̍X�V
		shape->update_world_trans(gameobject->world_position(), gameobject->world_orientate(), gameobject->world_scale(),
			is_changPos, is_changRot, is_changSiz
		);

		is_changed_Size |= is_changSiz;

		// DOP�̍X�V
		shape->update_dop14();

	}


	// ���[�U�[�ɓ��͂��ꂽphysics�f�[�^(mass�Ȃ�)���v�Z�p�̃f�[�^�Ɏ���
	ALPphysics->update_physics_data();

	// �������[�����g�̍X�V
	ALPphysics->update_tensor_and_barycenter(shapes, joints);




}

#pragma endregion
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

void ALP_Collider::integrate(float duration, Vector3 linear_velocity, Vector3 angula_velocity, Vector3 old_linear_velocity, Vector3 old_angula_velocity) {
	if (linear_velocity.norm() == 0 && angula_velocity.norm() == 0)return;

	//�e��orienattion�̋t���Ƃ�
	Quaternion pearent_orientate_inv = Quaternion(1, 0, 0, 0);
	if (gameobject->pearent() != nullptr) {
		pearent_orientate_inv = gameobject->pearent()->world_orientate();
		pearent_orientate_inv = pearent_orientate_inv.inverse();
	}

	//���݂̑��x�� �O�̑��x��������x���ŕω������Ƃ��Ĉړ����������߂�
	const Vector3 linear_move = old_linear_velocity * duration + 0.5f * (linear_velocity - old_linear_velocity) * duration;
	const Vector3 angula_move = old_angula_velocity * duration + 0.5f * (angula_velocity - old_angula_velocity) * duration;

	//�A�^�b�`����Ă���GO�̐e�q�֌W�ɑΉ� �e����]���Ă��Ă�������"��"������
	const Vector3 local_linear_move = vector3_quatrotate(linear_move, pearent_orientate_inv);
	const Vector3 local_anglar_move = vector3_quatrotate(angula_move, pearent_orientate_inv);

	//�ǂ������v�Z���ʂ͈ꏏ ��̂ق��������I
	//gameobject->transform->local_orient *= quaternion_axis_radian(local_anglar_move.unit_vect(), local_anglar_move.norm_sqr());
	//gameobject->transform->local_orient = gameobject->transform->local_orient.unit_vect();

	Vector3 local_linear_velocity = vector3_quatrotate(linear_velocity, pearent_orientate_inv);
	gameobject->transform->local_pos += local_linear_velocity * duration;

	Vector3 local_anglar_velocity = vector3_quatrotate(angula_velocity, pearent_orientate_inv);
	gameobject->transform->local_orient *= quaternion_axis_radian(local_anglar_velocity.unit_vect(), local_anglar_velocity.norm_sqr() * duration);
	gameobject->transform->local_orient = gameobject->transform->local_orient.unit_vect();

	//Quaternion dAng = gameobject->transform->local_orient.unit_vect() * Quaternion(0, anglar_velocity.x, anglar_velocity.y, anglar_velocity.z) * 0.5f;
	//gameobject->transform->local_orient =( gameobject->transform->local_orient + dAng * duration).unit_vect();
}

void ALP_Collider::reset_data_per_frame() {
	oncoll_bits = 0;
	coll_ptr->contacted_colliders.clear();
};

void ALP_Collider::Update_hierarchy()
{
	for (auto& shape : shapes) {
		shape->Update_hierarchy();
	}
};

//:::::

//����collider��������joint��ǉ�
void ALP_Collider::add_joint(ALP_Joint* joint) {
#if _DEBUG
	//�G���[�\�� ����joint���A�^�b�`���Ă͂����Ȃ�
	for (auto& j : joints) {
		if (j == joint) assert(0 && " the joint is already been added");
	}
#endif
	joints.emplace_back(joint);
}

//joint����O���ꂽ
void ALP_Collider::remove_joint(ALP_Joint* joint) {
	joints.remove(joint);
}

void ALP_Collider::add_contacted_collider(Contacts::Contact_pair* pair, u_int num) {
	if (coll_ptr->is_save_contacted_colls == false)return; //�ۑ�����flag�������Ă��Ȃ���Εۑ����Ȃ�

	Contacted_data data;
	data.coll = pair->body[1 - num]->get_ALPcollider()->get_collptr();
	//���ׂĂ�pair����Փ˓_�A�ђʗʁA�Փ˖@����ۑ�
	for (int contact_num = 0; contact_num < pair->contacts.contact_num; ++contact_num) {
		const auto& contact_point = pair->contacts.contactpoints[contact_num];

		//shape���W�n����GO���W�n�ɒ����ĕۑ�(�c�����킹��������"0+"�����Ă���)
		data.contacted_pointA = vector3_quatrotate(contact_point.point[0 + num], pair->body[0 + num]->local_orientation) + pair->body[0 + num]->local_position;
		data.contacted_pointB = vector3_quatrotate(contact_point.point[1 - num], pair->body[1 - num]->local_orientation) + pair->body[1 - num]->local_position;
		data.normal = contact_point.normal; //���[���h���
		data.penetrate = contact_point.distance;

		coll_ptr->contacted_colliders.push_back(data);
	}

}

Meshcoll_part* ALP_Collider::add_mesh_shape(const char* filepass, Physics_function::Meshcollider_data* mesh_data) {
	Meshcoll_part* shape = newD Meshcoll_part(this, filepass, mesh_data);

	shapes.emplace_back(shape);
	return shape;
};

void ALP_Collider::destroy()
{
	//shape�̉��
	for (auto& shape : shapes) {
		delete shape;
	}

	for (auto& j : joints) {
		j->destroy(this, true);
		delete j;
	}

	Phyisics_manager::remove_ALPcollider(scene, this_itr);
};

const Collider_tagbit ALP_Collider::get_tag()const { return coll_ptr->tag; }; //���g��tag(bit)
const Collider_tagbit ALP_Collider::get_ignore_tags() const { return coll_ptr->ignore_tags; }; //�Փ˂��Ȃ�tags