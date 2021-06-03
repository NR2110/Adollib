#include "ALP_midphase.h"
#include <list>
#include "../Imgui/work_meter.h"

#include "ALP_physics.h"
using namespace Adollib;
using namespace Physics_function;
using namespace Contacts;
using namespace DOP;

//:::::::::::::::::::::::::::
#pragma region Midphase
//:::::::::::::::::::::::::::
//DOP14による大雑把な当たり判定
bool Check_insert_DOP14(const Collider_shape* meshA, const Collider_shape* meshB) {
	//無限PlaneはDOPが作れないためnarrowに投げる?
	//if (meshA->shape == ALP_Collider_shape::Plane || meshB->shape == ALP_Collider_shape::Plane) return true;

	for (int i = 0; i < DOP14_size; i++) {
		float dis = vector3_dot(DOP_14_axis[i], meshA->get_DOP().pos - meshB->get_DOP().pos);

		if (
			0 > +dis + meshA->get_DOP().max[i] - meshB->get_DOP().min[i] ||
			0 > -dis + meshB->get_DOP().max[i] - meshA->get_DOP().min[i]
			) {
			return false;
		}
	}

	return true;
}

bool Check_insert_Plane(const Collider_shape* plane, const Collider_shape* mesh) {

	Vector3 V;
	float plane_dis = 0, coll_dis = FLT_MAX;

	V = vector3_quatrotate(Vector3(0, 1, 0), plane->world_orientation());
	plane_dis = vector3_dot(V, plane->world_position());

	for (int i = 0; i < DOP14_size; i++) {
		float coll_len = vector3_dot(V, mesh->world_position() + DOP_14_axis[i] * mesh->get_DOP().max[i]);
		if (plane_dis > coll_len)return true;
	}

	return false;
}

void add_pair(std::vector<Contacts::Contact_pair>& pairs, Contacts::Contact_pair pair) {
	pairs.emplace_back(pair);
}

void Midphase_DOP_14(std::vector<Contacts::Contact_pair>& new_pairs, Collider_shape* meshA, Collider_shape* meshB) {
	Contact_pair new_pair;

	const ALP_Collider* collA = meshA->get_ALPcollider();
	const ALP_Collider* collB = meshB->get_ALPcollider();

	// タグによる衝突の是非
	//bool hit = true;
	//if (collA->ALPphysics->is_hitable == false|| (collA->get_tag() & collB->nohit_tag)) hit = false;
	//if (collB->ALPphysics->is_hitable == false|| (collB->get_tag() & collA->nohit_tag)) hit = false;
	//bool check_oncoll_only = false;
	//if (hit == false) {
	//	if (collA->get_oncoll_check_bits() & collB->get_tag())check_oncoll_only = true;
	//	if (collB->get_oncoll_check_bits() & collA->get_tag())check_oncoll_only = true;
	//}
	//if (hit == false && check_oncoll_only == false)return;
	bool hit = true;
	if (collA->get_ALPphysics()->is_hitable == false || (meshA->get_tag() & meshB->get_ignore_tags())) hit = false;
	if (collB->get_ALPphysics()->is_hitable == false || (meshB->get_tag() & meshA->get_ignore_tags())) hit = false;
	bool check_oncoll_only = false;
	if (hit == false) {
		if (collA->get_oncoll_check_bits() & meshB->get_tag())check_oncoll_only = true;
		if (collB->get_oncoll_check_bits() & meshA->get_tag())check_oncoll_only = true;
	}
	if (hit == false && check_oncoll_only == false)return;

	//DOPによる大雑把な当たり判定
	if (meshA->get_shape_tag() != ALPCollider_shape_type::Plane && meshB->get_shape_tag() != ALPCollider_shape_type::Plane) {
		if (Check_insert_DOP14(meshA, meshB) == false)return;
	}
	else if (meshA->get_shape_tag() == ALPCollider_shape_type::Plane && meshB->get_shape_tag() != ALPCollider_shape_type::Plane) {
		if (Check_insert_Plane(meshA, meshB) == false)return;
	}
	else if (meshA->get_shape_tag() != ALPCollider_shape_type::Plane && meshB->get_shape_tag() == ALPCollider_shape_type::Plane) {
		if (Check_insert_Plane(meshB, meshA) == false)return;
	}
	else return;

	//new_pair.body[0]にアドレスの大きいほうをしまう
	if (&*meshA > & *meshB) {
		new_pair.body[0] = meshA;
		new_pair.body[1] = meshB;
	}
	else {
		new_pair.body[0] = meshB;
		new_pair.body[1] = meshA;
	}
	new_pair.type = Pairtype::new_pair;
	new_pair.check_oncoll_only = check_oncoll_only;

	add_pair(new_pairs, new_pair);
}


void Physics_function::Midphase(std::vector<Contacts::Collider_2>& in_pair, std::vector<Contacts::Contact_pair>& pairs) {

	Work_meter::start("Mid_Dop14");
	//DOP_14による大雑把な当たり判定
	std::vector<Contacts::Contact_pair> new_pairs;

	for (auto& pair : in_pair) {

		for (auto& meshB : pair.bodylists) {
			if (pair.body->get_ALPcollider() == meshB->get_ALPcollider())continue;

			Midphase_DOP_14(new_pairs, pair.body, meshB);
		}
	}
	Work_meter::stop("Mid_Dop14");

	Work_meter::start("Mid_check_alive");
	//生成したpairが前のpairから存在しているかの確認
	for (auto& old_p : pairs) {
		for (auto& new_p : new_pairs) {
			if (new_p.type == Pairtype::keep_pair) continue;

			if (
				new_p.body[0] == old_p.body[0] &&
				new_p.body[1] == old_p.body[1]
				) {
				//前から存在していたらデータを引き継ぐ
				new_p = old_p;

				new_p.type = Pairtype::keep_pair;
			}
			//else if (
			//	new_p.body[0] == old_p.body[1] &&
			//	new_p.body[1] == old_p.body[0]
			//	) {
			//	//前から存在していたらデータを引き継ぐ
			//	new_p = old_p;

			//	new_p.type = Pairtype::keep_pair;
			//}
			else {
				new_p.type = Pairtype::new_pair;
			}
		}
	}

	Work_meter::stop("Mid_check_alive");

	Work_meter::start("Mid_remove_contact_point");
	//現在使用していない衝突点を削除
	for (auto& new_p : new_pairs) {
		new_p.contacts.chack_remove_contact_point(
			new_p.body[0]->world_position(),
			new_p.body[0]->world_orientation(),
			new_p.body[1]->world_position(),
			new_p.body[1]->world_orientation()
		);
	}

	Work_meter::stop("Mid_remove_contact_point");
	pairs.clear();
	pairs = new_pairs;
}
#pragma endregion
//:::::::::::::::::::::::::::