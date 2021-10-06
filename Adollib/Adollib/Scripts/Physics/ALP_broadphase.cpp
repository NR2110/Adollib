#include "ALP_broadphase.h"
#include <list>

#include "../Imgui/work_meter.h"
#include "../Imgui/debug.h"

#include "ALP__physics_manager.h"


using namespace Adollib;
using namespace Physics_function;
using namespace Contacts;
using namespace DOP;

using namespace Broadphase_static;

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


void Midphase_DOP_14(std::vector<Contacts::Contact_pair*>& new_pairs, Collider_shape* meshA, Collider_shape* meshB) {

	if (meshA->get_ALPcollider() == meshB->get_ALPcollider())return; //同じGOにアタッチされたshape同士は衝突しない


	const ALP_Collider* collA = meshA->get_ALPcollider();
	const ALP_Collider* collB = meshB->get_ALPcollider();

	bool is_generate_contact = true;
	//sleep同士の場合は衝突検知のみ
	//if (collA->get_ALPphysics()->is_sleep == true && collB->get_ALPphysics()->is_sleep == true)is_generate_contact = false;

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

	Contact_pair* new_pair = newD Contact_pair;
	//new_pair.body[0]にアドレスの大きいほうをしまう
	if (&*meshA > & *meshB) {
		new_pair->body[0] = meshA;
		new_pair->body[1] = meshB;
	}
	else {
		new_pair->body[0] = meshB;
		new_pair->body[1] = meshA;
	}
	new_pair->type = Pairtype::new_pair;
	new_pair->check_oncoll_only = check_oncoll_only;

	new_pairs.emplace_back(new_pair);
}




void Physics_function::BroadMidphase(Scenelist Sce,
	const std::list<ALP_Collider*>& ALP_collider,
	std::vector<Contacts::Contact_pair*>& out_pair,
	std::vector<Physics_function::ALP_Collider*>& moved_collider, //動いたもの
	std::vector<Physics_function::ALP_Collider*>& added_collider //追加されたもの
) {

	const std::string work_meter_tag = std::string("Broadphase");

	//適当に20コ点を取って適当に?
	int SAP_axis = 0;

	//DOPの更新
	Work_meter::start("update_dop14", work_meter_tag);

	Work_meter::stop("update_dop14", work_meter_tag);

	//Work_meter::start("delete_colldier_from_axislist");
	////::: 削除されたものをaxis_listから削除する :::
	//{
	//	std::vector<std::list<edge>::iterator> itrs;
	//	std::list<edge>::iterator itr;
	//	for (const auto& removed : removed_collider) {
	//		if (axis_list_edge_pS.count(removed->scene) == 0)continue;
	//		if (axis_list_edge_pS[removed->scene].count(removed->index) == 0)continue;

	//		auto remove_edge_itrs = axis_list_edge_pS[removed->scene][removed->index];

	//		for (auto& edge : remove_edge_itrs) {
	//			axis_list_pS[removed->scene].erase(edge);
	//		}
	//	}
	//}
	//Work_meter::stop("delete_colldier_from_axislist");

	//auto& axis_list = axis_list;
	//auto& axis_list_edge = access_axislist_itr;

	Work_meter::start("make_axis_list", work_meter_tag);

	//::: 追加されたものをaxis_listに追加 :::
	{
		for (auto& added : added_collider) {
			const u_int collider_index = added->get_index(); //colliderのuniqueなID

			access_axislist_itr[collider_index].clear(); // 初期化

			u_int shape_count = 0;

			for (auto& shape : added->get_shapes()) {

				{
					Insert_edge* ed = newD Insert_edge;
					ed->value = shape->get_DOP().pos[0] + shape->get_DOP().max[0];
					ed->shape = shape;
					ed->edge_start = false;
					ed->shape_index = shape_count;
					axis_list.emplace_back(ed);

					shape_count++;
				}
				{
					Insert_edge* ed = newD Insert_edge;
					ed->value = shape->get_DOP().pos[0] + shape->get_DOP().min[0];
					ed->shape = shape;
					ed->edge_start = true;
					ed->shape_index = shape_count;
					axis_list.emplace_back(ed);

					shape_count++;
				}


				auto itr_G = axis_list.end();
				--itr_G;
				auto itr_S = itr_G;
				--itr_S;

				//互いにitrを保存
				(*itr_S)->axis_list_pair_itr = itr_G;
				(*itr_G)->axis_list_pair_itr = itr_S;

				//colliderからアクセスできるようにaccess_axislist_itrに保存
				access_axislist_itr[collider_index].push_back(itr_S);
				access_axislist_itr[collider_index].push_back(itr_G);
			}
		}
	}

	auto a = axis_list;


	Work_meter::stop("make_axis_list", work_meter_tag);

	//::: 動いたもののvalueを更新
	{
		Work_meter::start("update_Value", work_meter_tag);
		for (auto& moved : moved_collider) {

			for (auto& edge_itr : access_axislist_itr[moved->get_index()]) {
				auto& edge = *edge_itr;

				if (edge->edge_start == false)
					edge->value = edge->shape->get_DOP().pos[SAP_axis] + edge->shape->get_DOP().max[SAP_axis];
				else
					edge->value = edge->shape->get_DOP().pos[SAP_axis] + edge->shape->get_DOP().min[SAP_axis];
			}

		}

		for (auto& moved : added_collider) {
			u_int index_count = 0;

			for (auto& edge_itr : access_axislist_itr[moved->get_index()]) {
				auto& edge = *edge_itr;

				if (edge->edge_start == false)
					edge->value = edge->shape->get_DOP().pos[SAP_axis] + edge->shape->get_DOP().max[SAP_axis];
				else
					edge->value = edge->shape->get_DOP().pos[SAP_axis] + edge->shape->get_DOP().min[SAP_axis];

			}
		}

		Work_meter::stop("update_Value", work_meter_tag);
	}

	//::: 挿入ソート ::::::::::::::
	//small ----> big
	{
		Work_meter::start("insert_sort", work_meter_tag);
		auto a = axis_list;

		std::list<Insert_edge*>::iterator itr[2] = { axis_list.begin(),axis_list.begin() };
		u_int next_num = 0;

		std::list<Insert_edge*>::iterator itr_insert;
		std::list<Insert_edge*>::iterator itr_end = axis_list.end();


		if(itr[next_num] != itr_end)itr[next_num]++;

		while (true) {
			if (itr[next_num] == itr_end)break;

			const u_int now_num = 1 - next_num;
			const Insert_edge* next_edge = *itr[next_num];
			const Insert_edge* now_edge = *itr[now_num];


			if (next_edge->value < now_edge->value) {

				itr_insert = axis_list.begin();

				for (; itr_insert != itr_end; ++itr_insert) {
					if (next_edge->value < (*itr_insert)->value) {

						axis_list.emplace(itr_insert, *itr[next_num]);
						axis_list.erase(itr[next_num]);

						itr_insert--;
						(*(*itr_insert)->axis_list_pair_itr)->axis_list_pair_itr = itr_insert; //相方に保存されている自身へのitrを更新
						access_axislist_itr[(*itr_insert)->shape->get_ALPcollider()->get_index()][(*itr_insert)->shape_index] = itr_insert; //axis_list_edgeに保存したitrの更新

						itr[next_num] = itr_insert;
						break;
					}
				}


			}

			//次のnextに今のnextの次を保存し next_numを更新
			itr[now_num] = itr[next_num];
			++itr[now_num];
			next_num = now_num;
		}

		Work_meter::stop("insert_sort", work_meter_tag);
	}

	Work_meter::start("Sweep&Prune", work_meter_tag);
	//Sweep&Prune
	std::list<Collider_shape*> actives;
	std::list<Collider_shape*> static_actives;
	out_pair.clear();
	std::list<Collider_shape*>::iterator ac_itr;
	{
		for (auto& insert_edge_itr : axis_list) {
			auto& insert_edge = *insert_edge_itr;

			//colliderの始点ならactivelistにあるものと衝突の可能性あり
			if (insert_edge.edge_start == true) {

				if (insert_edge.shape->get_ALPcollider()->get_collptr()->is_static == true) {

					//staticなオブジェクトはstatic_activesとは衝突しない
					for (auto& shape : actives) {
						Midphase_DOP_14(out_pair, shape, insert_edge.shape);
					}
					static_actives.emplace_back(insert_edge.shape);

					auto pair_itr = static_actives.end();
					--pair_itr;
					(*insert_edge.axis_list_pair_itr)->active_list_pair_itr = pair_itr;
				}
				else {

					//dynamicなオブジェクトはすべてに可能性がある
					for (auto& shape : static_actives) {
						Midphase_DOP_14(out_pair, shape, insert_edge.shape);
					}
					for (auto& shape : actives) {
						Midphase_DOP_14(out_pair, shape, insert_edge.shape);
					}
					actives.emplace_back(insert_edge.shape);

					auto pair_itr = actives.end();
					--pair_itr;
					(*insert_edge.axis_list_pair_itr)->active_list_pair_itr = pair_itr;
				}

			}

			else {

				if (insert_edge.shape->get_ALPcollider()->get_collptr()->is_static == true)
					static_actives.erase(insert_edge.active_list_pair_itr);
				else
					actives.erase(insert_edge.active_list_pair_itr);
			}
		}

	}

	moved_collider.clear();
	added_collider.clear();

	Work_meter::stop("Sweep&Prune", work_meter_tag);
}

void Physics_function::remove_collider_broad_phase(Physics_function::ALP_Collider* removed) {
	//::: 削除されたものをaxis_listから削除する :::
	{
		const Scenelist scene = removed->get_scene();
		const u_int index = removed->get_index();

		if (Broadphase_static::access_axislist_itr.count(removed->get_index()) == 0)return;

		auto remove_edge_itrs = Broadphase_static::access_axislist_itr[index];

		for (auto& edge : remove_edge_itrs) {
			delete *edge;
			Broadphase_static::axis_list.erase(edge);

		}
	}
}