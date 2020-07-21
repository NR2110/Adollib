#include "ALP_boardphase.h"

using namespace Adollib;
using namespace Contacts;

//:::::::::::::::::::::::::::
#pragma region Boardphase
//:::::::::::::::::::::::::::
//DOP6�ɂ���G�c�ȓ����蔻��
bool Check_insert_DOP7(Collider* collA, Collider* collB) {
	//����Plane��DOP�����Ȃ�����narrow�ɓ�����?
	if (collA->shape == Collider_shape::shape_plane || collB->shape == Collider_shape::shape_plane) return true;

	vector3 dis = collA->dop7.pos - collB->dop7.pos;

	for (int i = 0; i < DOP_size; i++) {
		if (
			+vector3_dot(DOP_7_axis[i], collA->dop7.pos - collB->dop7.pos) < collB->dop7.min[i] - collA->dop7.max[i] ||
			-vector3_dot(DOP_7_axis[i], collA->dop7.pos - collB->dop7.pos) < collA->dop7.min[i] - collB->dop7.max[i]
			)
			return false;
	}

	return true;
}

bool Check_insert_Plane(Collider* plane, Collider* coll) {
	vector3 axis[6]{
	{1,0,0},
	{0,1,0},
	{0,0,1},
	{1,1,0},
	{0,1,1},
	{1,0,1}
	};
	vector3 V;
	float plane_dis = 0, coll_dis = FLT_MAX;

	V = vector3_be_rotated_by_quaternion(vector3(0, 1, 0), plane->world_orientation);
	plane_dis = vector3_dot(V, plane->world_position);

	for (int i = 0; i < DOP_size; i++) {
		float coll_len = vector3_dot(V, coll->world_position + axis[i] * coll->dop7.max[i]);
		if (plane_dis > coll_len)return true;
	}

	return false;
}

void physics_function::Boardphase(const std::vector<Collider*>& coll, std::vector<Contacts::Contact_pair>& pairs) {


	//DOP�̍X�V
	for (int i = 0; i < coll.size(); i++) {
		coll[i]->update_dop7();
	}

	//DOP_6�ɂ���G�c�ȓ����蔻��
	std::vector<Contacts::Contact_pair> new_pairs;
	Contact_pair new_pair;
	for (int i = 0; i < coll.size(); i++) {
		for (int o = i + 1; o < coll.size(); o++) {

			// �^�O�ɂ��Փ˂̐���
			bool hit = true;
			for (int q = 0; q < coll[q]->No_hit_tag.size(); q++) {
				if (coll[i]->No_hit_tag[q] == std::string("all")) hit = false;
				if (coll[i]->No_hit_tag[q] == coll[o]->tag) hit = false;
				if (hit == false)break;
			}
			if (hit == false)continue;
			for (int q = 0; q < coll[q]->No_hit_tag.size(); q++) {
				if (coll[o]->No_hit_tag[q] == std::string("all")) hit = false;
				if (coll[o]->No_hit_tag[q] == coll[i]->tag) hit = false;
				if (hit == false)break;
			}
			if (hit == false)continue;

			//DOP�ɂ���G�c�ȓ����蔻��
			if (coll[i]->shape != Collider_shape::shape_plane && coll[o]->shape != Collider_shape::shape_plane) {
				if (Check_insert_DOP7(coll[i], coll[o]) == false)continue;
			}
			else if (coll[i]->shape == Collider_shape::shape_plane && coll[o]->shape != Collider_shape::shape_plane) {
				if (Check_insert_Plane(coll[i], coll[o]) == false)continue;
			}
			else if (coll[i]->shape != Collider_shape::shape_plane && coll[o]->shape == Collider_shape::shape_plane) {
				if (Check_insert_Plane(coll[o], coll[i]) == false)continue;
			}
			else continue;

			//new_pair.body[0]�ɃA�h���X�̑傫���ق������܂�
			if (coll[i] > coll[o]) {
				new_pair.body[0] = coll[i];
				new_pair.body[1] = coll[o];
			}
			else {
				new_pair.body[0] = coll[o];
				new_pair.body[1] = coll[i];
			}
			new_pair.type = Pairtype::new_pair;

			new_pairs.emplace_back(new_pair);
		}
	}

	//��������pair���O��pair���瑶�݂��Ă��邩�̊m�F
	for (int old_num = 0; old_num < pairs.size(); old_num++) {
		for (int new_num = 0; new_num < new_pairs.size(); new_num++) {
			if (new_pairs[new_num].type == Pairtype::keep_pair) continue;

			if (
				new_pairs[new_num].body[0] == pairs[old_num].body[0] &&
				new_pairs[new_num].body[1] == pairs[old_num].body[1]
				) {
				//�O���瑶�݂��Ă�����f�[�^�������p��
				new_pairs[new_num] = pairs[old_num];
				new_pairs[new_num].type = Pairtype::keep_pair;
			}
			else {
				new_pairs[new_num].type = Pairtype::new_pair;
			}
		}
	}

	//���ݎg�p���Ă��Ȃ��Փ˓_���폜
	for (int i = 0; i < new_pairs.size(); i++) {
		new_pairs[i].contacts.chack_remove_contact_point(
			new_pairs[i].body[0]->world_position,
			new_pairs[i].body[0]->world_orientation,
			new_pairs[i].body[1]->world_position,
			new_pairs[i].body[1]->world_orientation
		);
	}

	pairs.clear();
	pairs = new_pairs;

}
#pragma endregion