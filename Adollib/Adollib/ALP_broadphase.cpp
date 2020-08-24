#include "ALP_broadphase.h"
#include <list>
using namespace Adollib;
using namespace Contacts;

struct edge{
	Collider* coll;
	float value;
	bool st_go; //start == true, goal == false
};

//
//void swap(int* a, int* b)
//{
//	int t = *a;
//	*a = *b;
//	*b = t;
//}
//
//int partition(Collider* array, int l, int r) {
//	float pivot = array[r].world_position.x;
//	int i = (l - 1);
//
//	for (int j = l; j <= r - 1; j++) {
//		if (array[j].world_position.x <= pivot) {
//			i++;
//			swap(&array[i], &array[j]);
//		}
//	}
//	swap(&array[i + 1], &array[r]);
//	return (i + 1);
//}
//
//void quickSort(Collider* array, int l, int r) {
//	if (l < r) {
//		int pivot = partition(array, l, r);
//		quickSort(array, l, pivot - 1);
//		quickSort(array, pivot + 1, r);
//	}
//}


void physics_function::Broadphase(const std::vector<Collider*>& coll, std::vector<Contacts::Contact_pair>& pairs) {
	//Sweep&Pruneを挿入法で実装
	static std::list<edge>axis_list[2]; //x,y軸の分だけ用意 

	//DOPの更新	
	int coll_size = coll.size();
	for (int i = 0; i < coll_size; i++) {
		coll[i]->update_dop7();
	}

	{
		pairs.clear();
		Contacts::Contact_pair pair;
		for (int C0 = 0; C0 < coll_size; C0++) {
			for (int C1 = C0 + 1; C1 < coll_size; C1++) {
				pair.body[0] = coll[C0];
				pair.body[1] = coll[C1];

				pairs.emplace_back(pair);
			}
		}

		return;
	}
	

	if (coll.size() * 2 != axis_list[0].size()) {
		//TODO : 新しく追加されたもの,なくなったものがわかればさらに最適化できる
		edge ed;

		for (int xy = 0; xy < 2; xy++) {
			axis_list[xy].clear();

			for (int coll_num = 0; coll_num < coll_size; coll_num++) {

				ed.value = coll[coll_num]->world_position[xy];
				ed.coll = coll[coll_num];

				axis_list[xy].emplace_back(ed);
				ed.value += coll[coll_num]->world_size[xy];
				axis_list[xy].emplace_back(ed);
			}
		}

	}

	//挿入ソート
	//[0]small ---- big
	{
		edge ed;

		for (int xy = 0; xy < 2; xy++) {

			std::list<edge>::iterator itr = axis_list[xy].begin();
			std::list<edge>::iterator itr_old;
			std::list<edge>::iterator itr_insert;
			int axis_size = axis_list[xy].size();

			for (int list_num = 0; list_num < axis_size - 1; list_num++) {
				itr_old = itr;
				itr++;

				if (itr_old->value > itr->value) {

					itr_insert = axis_list[xy].begin();
	
					for (int ins_num = 0; ins_num < axis_size; ins_num++) {
						if (itr_old->value < itr_insert->value) {

							axis_list[xy].insert(itr_insert, *itr_old);
							axis_list[xy].erase(itr_old);
							break;
						}

						itr_insert++;
					}

				}
			}

		}
	}


	//Sweep&Prune
	std::list < Collider*> actives;
	Contacts::Contact_pair pair;
	pairs.clear();
	for (int xy = 0; xy < 1; xy++) {
		int axis_size = axis_list[xy].size();

		std::list<edge>::iterator itr = axis_list[xy].begin();
		for (int list_num = 0; list_num < axis_size; list_num++) {

			pair.body[0] = itr->coll;

			bool add_active = true;
			std::list<Collider*>::iterator ac = actives.begin();
			std::list<Collider*>::iterator ac_end = actives.end();

			for (;ac != ac_end; ac++) {
				if (*ac == itr->coll) {
					actives.erase(ac);
					add_active = false;
					break;
				}
				pair.body[1] = *ac;
				pairs.emplace_back(pair);
			}

			if (add_active) {
				actives.emplace_front(itr->coll);
			}

			itr++;
		}
	}





}





