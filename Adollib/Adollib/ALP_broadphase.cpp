#include "ALP_broadphase.h"

#include <iostream>
#include <thread>
#include <mutex>

using namespace Adollib;
using namespace Contacts;

//:::::::::::::::::::::::::::
#pragma region Broadphase
//:::::::::::::::::::::::::::
//DOP6による大雑把な当たり判定
bool Check_insert_DOP7(const Collider* collA, const Collider* collB) {
	//無限PlaneはDOPが作れないためnarrowに投げる?
	if (collA->shape == Collider_shape::shape_plane || collB->shape == Collider_shape::shape_plane) return true;

	vector3 dis = collA->dop7.pos - collB->dop7.pos;

	for (int i = 0; i < DOP_size; i++) {
		if (
			vector3_dot(+DOP_7_axis[i], collA->dop7.pos - collB->dop7.pos) < collB->dop7.min[i] - collA->dop7.max[i] ||
			vector3_dot(-DOP_7_axis[i], collA->dop7.pos - collB->dop7.pos) < collA->dop7.min[i] - collB->dop7.max[i]
			) {
			float AA = vector3_dot(+DOP_7_axis[i], collA->dop7.pos - collB->dop7.pos);
			float AB = collB->dop7.min[i] - collA->dop7.max[i];
			float BA = vector3_dot(-DOP_7_axis[i], collA->dop7.pos - collB->dop7.pos);
			float BB = collA->dop7.min[i] - collB->dop7.max[i];
			return false;
		}
	}

	return true;
}

bool Check_insert_Plane(const Collider* plane, const Collider* coll) {
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

namespace noname {
	std::mutex mutex_physics;
}

void add_pair(std::vector<Contacts::Contact_pair>& pairs, Contacts::Contact_pair pair) {
	std::lock_guard<std::mutex> lock(noname::mutex_physics);
	pairs.emplace_back(pair);
}

void Broadphase_DOP_7(std::vector<Contacts::Contact_pair>& new_pairs, Collider* collA, Collider* collB) {
	Contact_pair new_pair;
	// タグによる衝突の是非
	bool hit = true;
	for (int q = 0; q < collB->No_hit_tag.size(); q++) {
		if (collA->No_hit_tag[q] == std::string("all")) hit = false;
		if (collA->No_hit_tag[q] == collB->tag) hit = false;
		if (hit == false)break;
	}
	if (hit == false)return;
	for (int q = 0; q < collB->No_hit_tag.size(); q++) {
		if (collB->No_hit_tag[q] == std::string("all")) hit = false;
		if (collB->No_hit_tag[q] == collA->tag) hit = false;
		if (hit == false)break;
	}
	if (hit == false)return;

	//DOPによる大雑把な当たり判定
	if (collA->shape != Collider_shape::shape_plane && collB->shape != Collider_shape::shape_plane) {
		if (Check_insert_DOP7(collA, collB) == false)return;
	}
	else if (collA->shape == Collider_shape::shape_plane && collB->shape != Collider_shape::shape_plane) {
		if (Check_insert_Plane(collA, collB) == false)return;
	}
	else if (collA->shape != Collider_shape::shape_plane && collB->shape == Collider_shape::shape_plane) {
		if (Check_insert_Plane(collB, collA) == false)return;
	}
	else return;

	//new_pair.body[0]にアドレスの大きいほうをしまう
	if (collA > collB) {
		new_pair.body[0] = collA;
		new_pair.body[1] = collB;
	}
	else {
		new_pair.body[0] = collB;
		new_pair.body[1] = collA;
	}
	new_pair.type = Pairtype::new_pair;

	add_pair(new_pairs, new_pair);
}


void physics_function::Broadphase(const std::vector<Collider*>& coll, std::vector<Contacts::Contact_pair>& pairs) {

	//DEBUG : 適当に調整
	const int thread_max = 7;
	std::thread threads[thread_max];

	//DOPの更新
	for (int i = 0; i < coll.size(); i++) {
		coll[i]->update_dop7();
	}

	//DOP_8による大雑把な当たり判定
	std::vector<Contacts::Contact_pair> new_pairs;
	Contact_pair new_pair;
	for (int i = 0; i < coll.size(); i++) {
		for (int o = i + 1; o < coll.size(); o++) {
			Broadphase_DOP_7(new_pairs, coll[i], coll[o]);
		}
	}

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

	//現在使用していない衝突点を削除
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