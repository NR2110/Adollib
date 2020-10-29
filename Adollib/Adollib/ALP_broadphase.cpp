#include "ALP_broadphase.h"
#include <list>

#include "work_meter.h"
#include "debug.h"
using namespace Adollib;
using namespace Contacts;

struct edge{
	Collider* coll;
	float value;
	bool stgo; //true = st, false = go 
};

void physics_function::Broadphase(std::list<Adollib::Collider*>& coll, std::vector<Contacts::Collider_2>& out_pair, std::vector<Contacts::Contact_pair>& pairs) {

#if 1
	//Sweep&Pruneを挿入法で実装
	static std::list<edge>axis_list[3];

	//DOPの更新	

	Work_meter::start("update_dop14");
	int coll_size = coll.size();
	std::list<Adollib::Collider*>::iterator collitr;
	std::list<Adollib::Collider*>::iterator collitr_end = coll.end();

	for (collitr = coll.begin(); collitr != collitr_end; collitr++) {
		if ((*collitr)->move == true) {
			int dafsdbf = 0;
		}
		(*collitr)->update_dop14();
	}
	Work_meter::stop("update_dop14");

	//適当に20コ点を取って適当に
	int SAP_axis = 0;

	Debug::set(std::string("SAP_axis"), Vector3(SAP_axis).get_XM3());


	if (coll.size() * 2 != axis_list[0].size()) {
		//TODO : 新しく追加されたもの,なくなったものがわかればさらに最適化できる
		edge ed;

		for (int xz = 0; xz < 3; xz++) {
			axis_list[xz].clear();

			for (collitr = coll.begin(); collitr != collitr_end; collitr++) {

				ed.value = (*collitr)->dop14.pos[xz] + (*collitr)->dop14.max[xz];
				ed.coll =  (*collitr);
				ed.stgo = false;

				axis_list[xz].emplace_back(ed);
				ed.value = (*collitr)->dop14.pos[xz] + (*collitr)->dop14.min[xz];
				ed.stgo = true;
				axis_list[xz].emplace_back(ed);


			}
		}

	}
	//valueの更新
	else {
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
	std::vector< Collider*> actives;
	Contacts::Collider_2 pair;
	out_pair.clear();
	std::vector<Collider*>::iterator ac = actives.begin();
	std::vector<Collider*>::iterator ac_end = actives.end();
	{

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

	}


	Work_meter::stop("Sweep&Prune");

#else
	//Sweep&Pruneを挿入法で実装
	static std::list<edge>axis_list[3];

	//DOPの更新	

	Work_meter::start("update_dop14");
	int coll_size = coll.size();
	for (int i = 0; i < coll_size; i++) {
		coll[i]->update_dop14();
	}
	Work_meter::stop("update_dop14");

	if (coll.size() * 2 != axis_list[0].size()) {
		//TODO : 新しく追加されたもの,なくなったものがわかればさらに最適化できる
		edge ed;

		for (int xz = 0; xz < 3; xz += 2) {
			axis_list[xz].clear();

			for (int coll_num = 0; coll_num < coll_size; coll_num++) {

				ed.value = coll[coll_num]->dop14.pos[xz] + coll[coll_num]->dop14.max[xz];
				ed.coll = coll[coll_num];
				ed.stgo = false;

				axis_list[xz].emplace_back(ed);
				ed.value = coll[coll_num]->dop14.pos[xz] + coll[coll_num]->dop14.min[xz];
				ed.stgo = true;
				axis_list[xz].emplace_back(ed);


			}
		}

	}
	//valueの更新
	else {

		Work_meter::start("update_Value");
		for (int xz = 0; xz < 3; xz += 2) {
			std::list<edge>::iterator itr = axis_list[xz].begin();
			std::list<edge>::iterator itr_end = axis_list[xz].end();
			for (; itr != itr_end; itr++) {
				if (itr->stgo == false) {
					itr->value = itr->coll->dop14.pos[xz] + itr->coll->dop14.max[xz];
				}
				else {
					itr->value = itr->coll->dop14.pos[xz] + itr->coll->dop14.min[xz];
				}

			}
		}
		Work_meter::stop("update_Value");
	}

	//挿入ソート
	//[0]small ---- big
	{
		Work_meter::start("insert_sort");
		edge ed;

		std::list<edge>::iterator itr;
		std::list<edge>::iterator itr_next;
		std::list<edge>::iterator itr_insert;
		int axis_size;
		for (int xz = 0; xz < 3; xz += 2) {
			itr = axis_list[xz].begin();
			axis_size = axis_list[xz].size();

			for (int list_num = 0; list_num < axis_size - 1; list_num++) {
				itr_next = itr;
				itr_next++;

				if (itr_next->value < itr->value) {

					itr_insert = axis_list[xz].begin();

					for (int ins_num = 0; ins_num < axis_size; ins_num++) {
						if (itr_next->value < itr_insert->value) {

							axis_list[xz].insert(itr_insert, *itr_next);
							axis_list[xz].erase(itr_next);
							break;
						}

						itr_insert++;
					}

				}
				else itr++;

			}
		}

		Work_meter::stop("insert_sort");
	}


	//Sweep&Prune
	std::unordered_map<Collider*, Collider*> next_pair;
	std::list<Collider*> actives;
	std::list<Collider*>::iterator ac;
	std::list<Collider*>::iterator ac_end;
	out_pair.clear();
	int axis;
	// x軸	
	{
		axis = 0;
		int axis_size = axis_list[axis].size();
		actives.clear();

		std::list<edge>::iterator itr = axis_list[axis].begin();
		for (int list_num = 0; list_num < axis_size; list_num++) {
			//colliderの始点ならactivelistにあるものと衝突の可能性あり
			if (itr->stgo == true) {

				ac = actives.begin();
				ac_end = actives.end();
				for (; ac != ac_end; ac++) {
					if (itr->coll < *ac)
						next_pair[itr->coll] = *ac;
					else
						next_pair[*ac] = itr->coll;
				}

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

	// z軸
	{

		axis = 2;
		int axis_size = axis_list[axis].size();
		actives.clear();
		Contacts::Collider_2 pair;

		std::list<edge>::iterator itr = axis_list[axis].begin();
		for (int list_num = 0; list_num < axis_size; list_num++) {
			//colliderの始点ならactivelistにあるものと衝突の可能性あり
			if (itr->stgo == true) {

				ac = actives.begin();
				ac_end = actives.end();
				for (; ac != ac_end; ac++) {
					if (itr->coll < *ac) {
						pair.body[0] = itr->coll;
						pair.body[1] = *ac;
						if (next_pair[itr->coll] == *ac) 
							out_pair.emplace_back(pair);
					}
					else {
						pair.body[1] = itr->coll;
						pair.body[0] = *ac;
						if (next_pair[*ac] == itr->coll) 
							out_pair.emplace_back(pair);
					}
				}

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

#endif

}
