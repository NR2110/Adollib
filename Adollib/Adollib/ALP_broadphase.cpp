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

	if (coll.size()*2 != axis_list[0].size()) {
		//TODO : 新しく追加されたもの,なくなったものがわかればさらに最適化できる
		edge ed;

		for (int xy = 0; xy < 2; xy++) {
			axis_list[xy].clear();

			for (int coll_num = 0; coll_num > coll.size(); coll_num++) {

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
			for (int list_num = 0; list_num < axis_list[xy].size() - 1; list_num++) {
				itr_old = itr;
				itr++;

				if (itr_old->value > itr->value) {

					itr_insert = axis_list[xy].begin();
					for (int ins_num = 0; ins_num < axis_list[xy].size(); ins_num++) {
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

	std::list<Collider*> active;





}





