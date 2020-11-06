#include "ALP_midphase.h"
#include <list>
#include "../Imgui/work_meter.h"
using namespace Adollib;
using namespace physics_function;
using namespace Contacts;
using namespace DOP;

//:::::::::::::::::::::::::::
#pragma region Midphase
//:::::::::::::::::::::::::::
//DOP6による大雑把な当たり判定
bool Check_insert_DOP14(const std::list<ALP_Collider>::iterator collA, const std::list<ALP_Collider>::iterator collB) {
	//無限PlaneはDOPが作れないためnarrowに投げる?
	if (collA->shape == ALP_Collider_shape::Plane || collB->shape == ALP_Collider_shape::Plane) return true;

	Vector3 dis = collA->dop14.pos - collB->dop14.pos;

	for (int i = 0; i < DOP_size; i++) {
		if (
			vector3_dot(+DOP_14_axis[i], collA->dop14.pos - collB->dop14.pos) < collB->dop14.min[i] - collA->dop14.max[i] ||
			vector3_dot(-DOP_14_axis[i], collA->dop14.pos - collB->dop14.pos) < collA->dop14.min[i] - collB->dop14.max[i]
			) {
			float AA = vector3_dot(+DOP_14_axis[i], collA->dop14.pos - collB->dop14.pos);
			float AB = collB->dop14.min[i] - collA->dop14.max[i];
			float BA = vector3_dot(-DOP_14_axis[i], collA->dop14.pos - collB->dop14.pos);
			float BB = collA->dop14.min[i] - collB->dop14.max[i];
			return false;
		}
	}

	return true;
}

bool Check_insert_Plane(const std::list<ALP_Collider>::iterator plane, const std::list<ALP_Collider>::iterator coll) {

	Vector3 V;
	float plane_dis = 0, coll_dis = FLT_MAX;

	V = vector3_quatrotate(Vector3(0, 1, 0), plane->world_orientation);
	plane_dis = vector3_dot(V, plane->world_position);

	for (int i = 0; i < DOP_size; i++) {
		float coll_len = vector3_dot(V, coll->world_position + DOP_14_axis[i] * coll->dop14.max[i]);
		if (plane_dis > coll_len)return true;
	}

	return false;
}

void add_pair(std::vector<Contacts::Contact_pair>& pairs, Contacts::Contact_pair pair) {
	pairs.emplace_back(pair);
}

void Midphase_DOP_14(std::vector<Contacts::Contact_pair>& new_pairs, std::list<ALP_Collider>::iterator collA, std::list<ALP_Collider>::iterator collB) {
	Contact_pair new_pair;

	// タグによる衝突の是非
	bool hit = true;
	if (collA->tag & collB->not_hitable_tags) hit = false;
	if (collB->tag & collA->not_hitable_tags) hit = false;
	bool check_oncoll_only = false;
	if (hit == false) {
		if (collA->oncoll_check_bits & collB->tag)check_oncoll_only = true;
		if (collB->oncoll_check_bits & collA->tag)check_oncoll_only = true;
	}
	if (hit == false && check_oncoll_only == false)return;

	//DOPによる大雑把な当たり判定
	if (collA->shape != ALP_Collider_shape::Plane && collB->shape != ALP_Collider_shape::Plane) {
		if (Check_insert_DOP14(collA, collB) == false)return;
	}
	else if (collA->shape == ALP_Collider_shape::Plane && collB->shape != ALP_Collider_shape::Plane) {
		if (Check_insert_Plane(collA, collB) == false)return;
	}
	else if (collA->shape != ALP_Collider_shape::Plane && collB->shape == ALP_Collider_shape::Plane) {
		if (Check_insert_Plane(collB, collA) == false)return;
	}
	else return;

	//new_pair.body[0]にアドレスの大きいほうをしまう
	if (&*collA > &*collB) {
		new_pair.body[0] = collA;
		new_pair.body[1] = collB;
	}
	else {
		new_pair.body[0] = collB;
		new_pair.body[1] = collA;
	}
	new_pair.type = Pairtype::new_pair;
	new_pair.check_oncoll_only = check_oncoll_only;

	add_pair(new_pairs, new_pair);
}


void physics_function::Midphase(std::vector<Contacts::Collider_2>& in_pair, std::vector<Contacts::Contact_pair>& pairs) {

	Work_meter::start("Mid_Dop14");
	//DOP_14による大雑把な当たり判定
	std::vector<Contacts::Contact_pair> new_pairs;
	for (int i = 0; i < in_pair.size(); i++) {

		int c_size = in_pair[i].bodylists.size();
		for (int o = 0; o < c_size; o++) {

			Midphase_DOP_14(new_pairs, in_pair[i].body, in_pair[i].bodylists[o]);
		}
	}
	Work_meter::stop("Mid_Dop14");

	Work_meter::start("Mid_check_alive");
	//生成したpairが前のpairから存在しているかの確認
	for (int old_num = 0; old_num < pairs.size(); old_num++) {
		for (int new_num = 0; new_num < new_pairs.size(); new_num++) {
			if (new_pairs[new_num].type == Pairtype::keep_pair) continue;

			if (
				new_pairs[new_num].body[0] == pairs[old_num].body[0] &&
				new_pairs[new_num].body[1] == pairs[old_num].body[1]
				) {
				//前から存在していたらデータを引き継ぐ
				new_pairs[new_num] = pairs[old_num];

				new_pairs[new_num].type = Pairtype::keep_pair;
			}
			else {
				new_pairs[new_num].type = Pairtype::new_pair;
			}
		}
	}
	Work_meter::stop("Mid_check_alive");

	Work_meter::start("Mid_remove_contact_point");
	//現在使用していない衝突点を削除
	for (int i = 0; i < new_pairs.size(); i++) {
		new_pairs[i].contacts.chack_remove_contact_point(
			new_pairs[i].body[0]->world_position,
			new_pairs[i].body[0]->world_orientation,
			new_pairs[i].body[1]->world_position,
			new_pairs[i].body[1]->world_orientation
		);
	}
	Work_meter::stop("Mid_remove_contact_point");
	pairs.clear();
	pairs = new_pairs;
}
#pragma endregion
//:::::::::::::::::::::::::::