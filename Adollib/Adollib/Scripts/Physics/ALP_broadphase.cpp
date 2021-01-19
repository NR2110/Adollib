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
	std::list<ALP_Collider>& ALP_collider, std::vector<Contacts::Collider_2>& out_pair,
	std::vector<std::list<Physics_function::ALP_Collider>::iterator>& moved_collider, //動いたもの
	std::vector<std::list<Physics_function::ALP_Collider>::iterator>& added_collider //追加されたもの
) {

	//適当に20コ点を取って適当に?
	int SAP_axis = 0;

	//DOPの更新
	Work_meter::start("update_dop14");

	for (auto& itr : ALP_collider) {
		itr.update_dop14();
	}

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
			auto itr = added->collider_meshes.begin();
			u_int index_count = 0;
			axis_list_edge_pS[added->scene][added->index].clear();
			for (const auto& mesh : added->collider_meshes) {
				ed.value = mesh.dop14.pos[0] + mesh.dop14.max[0];
				ed.mesh = itr;
				ed.stgo = false;
				ed.mesh_index = index_count;
				axis_list_pS[added->scene].push_back(ed);
				auto a = axis_list_pS[added->scene].end();
				a--;
				axis_list_edge_pS[added->scene][added->index].push_back(a);
				index_count++;

				ed.value = mesh.dop14.pos[0] + mesh.dop14.min[0];
				ed.mesh = itr;
				ed.stgo = true;
				ed.mesh_index = index_count;
				axis_list_pS[added->scene].push_back(ed);
				auto b = axis_list_pS[added->scene].end();
				b--;
				axis_list_edge_pS[added->scene][added->index].push_back(b);
				index_count++;

				itr++;
			}
		}
	}

	Work_meter::stop("make_axis_list");

	//::: 動いたもののvalueを更新
	{
		Work_meter::start("update_Value");
		for (auto& moved : moved_collider) {
			u_int index_count = 0;

			for (auto& edge : axis_list_edge_pS[moved->scene][moved->index]) {
				if (edge->stgo == false) {
					edge->value = edge->mesh->dop14.pos[SAP_axis] + edge->mesh->dop14.max[SAP_axis];
				}
				else {
					edge->value = edge->mesh->dop14.pos[SAP_axis] + edge->mesh->dop14.min[SAP_axis];
				}
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

						axis_list_edge[itr_insert->mesh->ALPcollider->index][itr_insert->mesh_index] = itr_insert;
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
	std::list<std::vector<ALP_Collider_mesh>::iterator> actives;
	std::list<std::vector<ALP_Collider_mesh>::iterator> static_actives;
	Contacts::Collider_2 pair;
	out_pair.clear();
	std::list<std::vector<ALP_Collider_mesh>::iterator>::iterator ac_itr;
	{
		for (auto& insert_edge : axis_list) {
			//colliderの始点ならactivelistにあるものと衝突の可能性あり
			if (insert_edge.stgo == true) {

				if ((*insert_edge.mesh->ALPcollider->coll_itr)->is_static == true) {

					pair.body = insert_edge.mesh;
					pair.bodylists = actives;
					out_pair.emplace_back(pair);

					static_actives.emplace_back(insert_edge.mesh);
				}
				else {

					pair.body = insert_edge.mesh;
					pair.bodylists = actives;
					out_pair.emplace_back(pair);
					pair.bodylists = static_actives;
					out_pair.emplace_back(pair);

					actives.emplace_back(insert_edge.mesh);
				}

			}
			else {
				if ((*insert_edge.mesh->ALPcollider->coll_itr)->is_static == true) {
					ac_itr = static_actives.begin();
					for (auto& ac : static_actives) {
						if (ac._Ptr == insert_edge.mesh._Ptr) {
							static_actives.erase(ac_itr);
							break;

						}
						++ac_itr;
					}
				}
				else {
					ac_itr = actives.begin();
					for (auto& ac : actives) {
						if (ac._Ptr == insert_edge.mesh._Ptr) {
							actives.erase(ac_itr);
							break;

						}
						++ac_itr;
					}
				}


			}
		}

	}

	moved_collider.clear();
	added_collider.clear();

	Work_meter::stop("Sweep&Prune");
}

void Physics_function::remove_collider_broad_phase(std::list<Physics_function::ALP_Collider>::iterator removed) {
	//::: 削除されたものをaxis_listから削除する :::
	{
		std::vector<std::list<Insert_edge>::iterator> itrs;
		std::list<Insert_edge>::iterator itr;

		if (Broadphase_static::axis_list_edge_pS.count(removed->scene) == 0)return;
		if (Broadphase_static::axis_list_edge_pS[removed->scene].count(removed->index) == 0)return;

		auto remove_edge_itrs = Broadphase_static::axis_list_edge_pS[removed->scene][removed->index];

		for (auto& edge : remove_edge_itrs) {
			Broadphase_static::axis_list_pS[removed->scene].erase(edge);

		}
	}
}