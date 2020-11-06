#include "ALP_broadphase.h"
#include <list>

#include "../Imgui/work_meter.h"
#include "../Imgui/debug.h"

#include "ALP__physics_manager.h"
using namespace Adollib;
using namespace physics_function;
using namespace Contacts;

//挿入法の
struct edge{
	std::list<ALP_Collider>::iterator coll;
	float value = 0;
	bool stgo = false; //true = st, false = go 
};

void physics_function::Broadphase(std::list<ALP_Physics>& ALP_physicses, std::vector<Contacts::Collider_2>& out_pair) {

	//Sweep&Pruneを挿入法で実装
	static std::list<edge>axis_list[1];

	//DOPの更新	

	Work_meter::start("update_dop14");

	for (auto& itr : ALP_physicses) {
		itr.ALP_coll->update_dop14();
	}

	Work_meter::stop("update_dop14");

	//適当に20コ点を取って適当に?
	int SAP_axis = 0;

	Work_meter::start("make_axis_list");
	if (Phyisics_manager::is_changed_colliders() == true) {
		//TODO : 新しく追加されたもの,なくなったものがわかればさらに最適化できる
		edge ed;

		for (int xz = 0; xz < 1; xz++) {
			axis_list[xz].clear();

			for (auto& itr : ALP_physicses) {

				ed.value = itr.ALP_coll->dop14.pos[xz] + itr.ALP_coll->dop14.max[xz];
				ed.coll = itr.ALP_coll;
				ed.stgo = false;

				axis_list[xz].push_back(ed);

				ed.value = itr.ALP_coll->dop14.pos[xz] + itr.ALP_coll->dop14.min[xz];
				ed.coll = itr.ALP_coll;
				ed.stgo = true;
				axis_list[xz].push_back(ed);


			}
		}

	}
	Work_meter::stop("make_axis_list");
	//valueの更新
	if(Phyisics_manager::is_changed_colliders() == false) {
		Work_meter::start("update_Value");
		std::list<edge>::iterator itr = axis_list[SAP_axis].begin();
		std::list<edge>::iterator itr_end = axis_list[SAP_axis].end();
		for (; itr != itr_end; itr++) {
			if (itr->stgo == false) {
				itr->value = itr->coll->dop14.pos[SAP_axis] + itr->coll->dop14.max[SAP_axis];
			}
			else {
				itr->value = itr->coll->dop14.pos[SAP_axis] + itr->coll->dop14.min[SAP_axis];
			}

		}

		Work_meter::stop("update_Value");
	}

	//挿入ソート
	//[0]small ---- big
	{
		Work_meter::start("insert_sort");
		edge ed;

		std::list<edge>::iterator itr = axis_list[SAP_axis].begin();
		std::list<edge>::iterator itr_next;
		std::list<edge>::iterator itr_insert;
		int axis_size = axis_list[SAP_axis].size();

		for (int list_num = 0; list_num < axis_size - 1; list_num++) {
			itr_next = itr;
			itr_next++;

			if (itr_next->value < itr->value) {

				itr_insert = axis_list[SAP_axis].begin();

				for (int ins_num = 0; ins_num < axis_size; ins_num++) {
					if (itr_next->value < itr_insert->value) {

						axis_list[SAP_axis].insert(itr_insert, *itr_next);
						axis_list[SAP_axis].erase(itr_next);
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
	std::vector<std::list<ALP_Collider>::iterator> actives;
	Contacts::Collider_2 pair;
	out_pair.clear();
	std::vector<std::list<ALP_Collider>::iterator>::iterator ac = actives.begin();
	std::vector<std::list<ALP_Collider>::iterator>::iterator ac_end = actives.end();
	{


		int axis_size = axis_list[SAP_axis].size();

		std::list<edge>::iterator itr = axis_list[SAP_axis].begin();
		for (int list_num = 0; list_num < axis_size; list_num++) {
			//colliderの始点ならactivelistにあるものと衝突の可能性あり
			if (itr->stgo == true) {

				pair.body = itr->coll;
				pair.bodylists = actives;
				out_pair.emplace_back(pair);

				actives.emplace_back(itr->coll);
			}
			else {
				ac = actives.begin();
				ac_end = actives.end();

				for (; ac != ac_end; ac++) {
					if (*ac == itr->coll) {
						actives.erase(ac);
						break;

					}
				}
			}

			itr++;
		}


	}


	Work_meter::stop("Sweep&Prune");
}
