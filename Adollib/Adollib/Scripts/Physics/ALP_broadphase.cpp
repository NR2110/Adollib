#include "ALP_broadphase.h"
#include <list>

#include "../Imgui/work_meter.h"
#include "../Imgui/debug.h"

#include "ALP__physics_manager.h"


using namespace Adollib;
using namespace Physics_function;
using namespace Contacts;

using namespace Broadphase_static;

//std::unordered_map <Scenelist, std::list<edge>> Broadphase_static::axis_list_pS;
//std::unordered_map <Scenelist, std::unordered_map<u_int, std::list<std::list<edge>::iterator>>> Broadphase_static::axis_list_edge_pS;

void Physics_function::Broadphase(Scenelist Sce,
	const std::list<ALP_Collider*>& ALP_collider,
	std::vector<Contacts::Collider_2>& out_pair,
	std::vector<Physics_function::ALP_Collider*>& moved_collider, //動いたもの
	std::vector<Physics_function::ALP_Collider*>& added_collider //追加されたもの
) {

	//適当に20コ点を取って適当に?
	int SAP_axis = 0;

	//DOPの更新
	Work_meter::start("update_dop14");

	Work_meter::stop("update_dop14");

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

	auto& axis_list = axis_list_pS[Sce];
	auto& axis_list_edge = axis_list_edge_pS[Sce];

	Work_meter::start("make_axis_list");

	//::: 追加されたものをaxis_listに追加 :::
	{
		Insert_edge ed;
		for (const auto& added : added_collider) {
			const Scenelist scene = added->get_scene();
			const u_int index = added->get_index();

			//auto itr = added->collider_meshes.begin();
			u_int index_count = 0;
			axis_list_edge_pS[added->get_scene()][index].clear();
			for (const auto& shape : added->get_shapes()) {
				ed.value = shape->get_DOP().pos[0] + shape->get_DOP().max[0];
				ed.shape = shape;
				ed.edge_start = false;
				ed.mesh_index = index_count;
				axis_list_pS[scene].push_back(ed);
				index_count++;

				ed.value = shape->get_DOP().pos[0] + shape->get_DOP().min[0];
				ed.shape = shape;
				ed.edge_start = true;
				ed.mesh_index = index_count;
				axis_list_pS[scene].push_back(ed);
				index_count++;

				auto itr_G = axis_list_pS[scene].end();
				--itr_G;
				auto itr_S = itr_G;
				--itr_S;

				itr_S->axis_list_pair_itr = itr_G;
				itr_G->axis_list_pair_itr = itr_S;

				axis_list_edge_pS[scene][index].push_back(itr_S);
				axis_list_edge_pS[scene][index].push_back(itr_G);
			}
		}
	}

	Work_meter::stop("make_axis_list");

	//::: 動いたもののvalueを更新
	{
		Work_meter::start("update_Value");
		for (auto& moved : moved_collider) {
			u_int index_count = 0;

			for (auto& edge : axis_list_edge_pS[moved->get_scene()][moved->get_index()]) {
				if (edge->edge_start == false)
					edge->value = edge->shape->get_DOP().pos[SAP_axis] + edge->shape->get_DOP().max[SAP_axis];
				else
					edge->value = edge->shape->get_DOP().pos[SAP_axis] + edge->shape->get_DOP().min[SAP_axis];
			}

		}

		for (auto& moved : added_collider) {
			u_int index_count = 0;

			for (auto& edge : axis_list_edge_pS[moved->get_scene()][moved->get_index()]) {
				if (edge->edge_start == false)
					edge->value = edge->shape->get_DOP().pos[SAP_axis] + edge->shape->get_DOP().max[SAP_axis];
				else
					edge->value = edge->shape->get_DOP().pos[SAP_axis] + edge->shape->get_DOP().min[SAP_axis];

			}
		}

		Work_meter::stop("update_Value");
	}

	//挿入ソート
	//small ----> big
	{
		Work_meter::start("insert_sort");
		Insert_edge ed;

		std::list<Insert_edge>::iterator itr = axis_list.begin();
		std::list<Insert_edge>::iterator itr_next;
		std::list<Insert_edge>::iterator itr_insert;
		std::list<Insert_edge>::iterator itr_end = axis_list.end();


		while (true) {
			itr_next = itr;
			itr_next++;
			if (itr_next == itr_end)break;

			if (itr_next->value < itr->value) {
				itr_insert = axis_list.begin();

				while (true) {
					if (itr_next->value < itr_insert->value) {

						axis_list.insert(itr_insert, *itr_next);
						axis_list.erase(itr_next);

						itr_insert--;
						itr_insert->axis_list_pair_itr->axis_list_pair_itr = itr_insert;
						axis_list_edge[itr_insert->shape->get_ALPcollider()->get_index()][itr_insert->mesh_index] = itr_insert;
						break;
					}

					itr_insert++;
				}

			}
			else itr++;
		}

		Work_meter::stop("insert_sort");
	}

	Work_meter::start("Sweep&Prune");
	//Sweep&Prune
	std::list<Collider_shape*> actives;
	std::list<Collider_shape*> static_actives;
	Contacts::Collider_2 pair;
	out_pair.clear();
	std::list<Collider_shape*>::iterator ac_itr;
	{
		for (auto& insert_edge : axis_list) {
			//colliderの始点ならactivelistにあるものと衝突の可能性あり
			if (insert_edge.edge_start == true) {

				if ((*insert_edge.shape->get_ALPcollider()->get_collitr())->is_static == true) {

					pair.body = insert_edge.shape;
					pair.bodylists = actives;
					out_pair.emplace_back(pair);

					static_actives.emplace_back(insert_edge.shape);

					auto pair_itr = static_actives.end();
					--pair_itr;
					insert_edge.axis_list_pair_itr->active_list_pair_itr = pair_itr;
				}
				else {

					pair.body = insert_edge.shape;
					pair.bodylists = actives;
					out_pair.emplace_back(pair);
					pair.bodylists = static_actives;
					out_pair.emplace_back(pair);
					actives.emplace_back(insert_edge.shape);

					auto pair_itr = actives.end();
					--pair_itr;
					insert_edge.axis_list_pair_itr->active_list_pair_itr = pair_itr;
				}

			}
			else {
				if ((*insert_edge.shape->get_ALPcollider()->get_collitr())->is_static == true)
					static_actives.erase(insert_edge.active_list_pair_itr);
				else
					actives.erase(insert_edge.active_list_pair_itr);
			}
		}

	}

	moved_collider.clear();
	added_collider.clear();

	Work_meter::stop("Sweep&Prune");
}

void Physics_function::remove_collider_broad_phase(Physics_function::ALP_Collider* removed) {
	//::: 削除されたものをaxis_listから削除する :::
	{
		const Scenelist scene = removed->get_scene();
		const u_int index = removed->get_index();

		if (Broadphase_static::axis_list_edge_pS.count(scene) == 0)return;
		if (Broadphase_static::axis_list_edge_pS[scene].count(removed->get_index()) == 0)return;

		auto remove_edge_itrs = Broadphase_static::axis_list_edge_pS[scene][index];

		for (auto& edge : remove_edge_itrs) {
			Broadphase_static::axis_list_pS[scene].erase(edge);

		}
	}
}