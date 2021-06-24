#include "ALP_midphase.h"
#include <list>
#include "../Imgui/work_meter.h"

#include "ALP_physics.h"

#include "ALP__physics_manager.h"
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
		const float dis = vector3_dot(DOP_14_axis[i], meshA->get_DOP().pos - meshB->get_DOP().pos);

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

	bool is_generate_contact = true;
	// sleep同士の場合は衝突検知のみ
	if (collA->get_ALPphysics()->is_sleep == true && collB->get_ALPphysics()->is_sleep == true)is_generate_contact = false;

	// タグによる衝突の是非
	if (collA->get_ALPphysics()->is_hitable == false || (meshA->get_tag() & meshB->get_ignore_tags())) is_generate_contact = false;
	if (collB->get_ALPphysics()->is_hitable == false || (meshB->get_tag() & meshA->get_ignore_tags())) is_generate_contact = false;


	bool check_oncoll_only = false;
	if (is_generate_contact == false) {
		if (collA->get_oncoll_check_bits() & meshB->get_tag())check_oncoll_only = true;
		if (collB->get_oncoll_check_bits() & meshA->get_tag())check_oncoll_only = true;
	}
	if (is_generate_contact == false && check_oncoll_only == false)return;

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


void Physics_function::Midphase(std::vector<Contacts::Collider_2>& in_pair, std::vector<Contacts::Contact_pair>& old_pairs, std::vector<Contacts::Contact_pair>& out_pairs) {

	std::vector<Contacts::Contact_pair> new_pairs;
	new_pairs.clear();

	Work_meter::start("Mid_Dop14");
	//DOP_14による大雑把な当たり判定
	for (auto& pair : in_pair) {

		for (auto& meshB : pair.bodylists) {
			if (pair.body->get_ALPcollider() == meshB->get_ALPcollider())continue;

			Midphase_DOP_14(new_pairs, pair.body, meshB);
		}
	}
	Work_meter::stop("Mid_Dop14");


	const int old_pair_size = old_pairs.size();
	const int new_pair_size = new_pairs.size();

	Work_meter::start("Contact_pair_quick_sort");
	{
		if (new_pair_size != 0) {
			std::vector<Contacts::Contact_pair> pair_buff;
			pair_buff.resize(new_pair_size);
			Contact_pair_quick_sort(&new_pairs[0], &pair_buff[0], new_pair_size);
		}
	}
	Work_meter::stop("Contact_pair_quick_sort");

	//前フレームから存在していたかのチェック
	out_pairs.clear();
	out_pairs.reserve(sizeof(Contacts::Contact_pair) * new_pair_size);
	Work_meter::start("Mid_check_alive");
	{
		int oldId = 0, newId = 0;
		while (oldId < old_pair_size && newId < new_pair_size)
		{
			if (new_pairs[newId].key > old_pairs[oldId].key) {
				//oldのkeyがnewより小さい -> そのoldは衝突生成されていない

				if (old_pairs[oldId].body[0]->get_ALPcollider()->get_ALPphysics()->is_sleep == true &&
					old_pairs[oldId].body[1]->get_ALPcollider()->get_ALPphysics()->is_sleep == true) {
					//お互いにfleeze状態ならoutに追加
					old_pairs[oldId].type = Contacts::Pairtype::keep_pair;
					out_pairs.emplace_back(old_pairs[oldId]);
				}

				oldId++;
			}
			else if (new_pairs[newId].key == old_pairs[oldId].key) {
				//oldのkeyとnewが同じ -> 前から存在している
				// keep

				// accumeimpulseの大きさからsleepをflseにする
				for (int i = 0; i < old_pairs[oldId].contacts.contact_num; i++) {
					if (old_pairs[oldId].contacts.contactpoints[i].constraint->accuminpulse > Phyisics_manager::physicsParams.accumeimpulse_sleep_threrhold) {
						old_pairs[oldId].body[0]->get_ALPcollider()->get_ALPphysics()->is_sleep = false;
						old_pairs[oldId].body[1]->get_ALPcollider()->get_ALPphysics()->is_sleep = false;
					};
				}

				old_pairs[oldId].type = Contacts::Pairtype::keep_pair;
				out_pairs.emplace_back(old_pairs[oldId]);

				oldId++;
				newId++;
			}
			else {
				//oldのkeyがnewより大きい -> 新しいnew
				// new
				new_pairs[newId].type = Contacts::Pairtype::keep_pair;
				new_pairs[newId].contacts.reset();
				out_pairs.emplace_back(new_pairs[newId]);

				newId++;
			}

		}

		if (newId < new_pair_size) {
			// 残りは全部new
			for (; newId < new_pair_size; newId++) {
				new_pairs[newId].type = Contacts::Pairtype::keep_pair;
				new_pairs[newId].contacts.reset();
				out_pairs.emplace_back(new_pairs[newId]);
			}
		}
		else if (oldId < old_pair_size) {
			// 残りは全部old
			if (old_pairs[oldId].body[0]->get_ALPcollider()->get_ALPphysics()->is_sleep == true &&
				old_pairs[oldId].body[1]->get_ALPcollider()->get_ALPphysics()->is_sleep == true) {
				//お互いにfleeze状態ならoutに追加
				old_pairs[oldId].type = Contacts::Pairtype::keep_pair;
				out_pairs.emplace_back(old_pairs[oldId]);
			}

			oldId++;
		}

	}

	Work_meter::stop("Mid_check_alive");

	Work_meter::start("Mid_remove_contact_point");
	//現在使用していない衝突点を削除
	for (auto& new_p : out_pairs) {
		new_p.contacts.chack_remove_contact_point(
			new_p.body[0],
			new_p.body[1]
		);
	}

	Work_meter::stop("Mid_remove_contact_point");
}
#pragma endregion
//:::::::::::::::::::::::::::