#include "ALP_broadphase.h"
#include <list>

#include "work_meter.h"
#include "debug.h"
using namespace Adollib;
using namespace Contacts;

#if 0
//DOP6による大雑把な当たり判定
bool Check_insert_DOP7(Collider* collA, Collider* collB) {
	//無限PlaneはDOPが作れないためnarrowに投げる?
	if (collA->shape == Collider_shape::shape_plane || collB->shape == Collider_shape::shape_plane) return true;

	vector3 dis = collA->dop7.pos - collB->dop7.pos;

	for (int i = 0; i < DOP_size; i++) {
		if (
			+vector3_dot(DOP_7_axis[i], collA->dop7.pos - collB->dop7.pos) < collB->dop7.min[i] - collA->dop7.max[i] ||
			-vector3_dot(DOP_7_axis[i], collA->dop7.pos - collB->dop7.pos) < collA->dop7.min[i] - collB->dop7.max[i]
			)
			return false;
	}

	return true;
}
bool Check_insert_Plane(Collider* plane, Collider* coll) {
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

void physics_function::Broadphase(const std::vector<Collider*>& coll, std::vector<Contacts::Contact_pair>& pairs) {


	//DOPの更新
	for (int i = 0; i < coll.size(); i++) {
		coll[i]->update_dop7();
	}

	//DOP_6による大雑把な当たり判定
	std::vector<Contacts::Contact_pair> new_pairs;
	Contact_pair new_pair;
	for (int i = 0; i < coll.size(); i++) {
		for (int o = i + 1; o < coll.size(); o++) {

			// タグによる衝突の是非
			bool hit = true;
			for (int q = 0; q < coll[q]->No_hit_tag.size(); q++) {
				if (coll[i]->No_hit_tag[q] == std::string("all")) hit = false;
				if (coll[i]->No_hit_tag[q] == coll[o]->tag) hit = false;
				if (hit == false)break;
			}
			if (hit == false)continue;
			for (int q = 0; q < coll[q]->No_hit_tag.size(); q++) {
				if (coll[o]->No_hit_tag[q] == std::string("all")) hit = false;
				if (coll[o]->No_hit_tag[q] == coll[i]->tag) hit = false;
				if (hit == false)break;
			}
			if (hit == false)continue;

			//DOPによる大雑把な当たり判定
			if (coll[i]->shape != Collider_shape::shape_plane && coll[o]->shape != Collider_shape::shape_plane) {
				if (Check_insert_DOP7(coll[i], coll[o]) == false)continue;
			}
			else if (coll[i]->shape == Collider_shape::shape_plane && coll[o]->shape != Collider_shape::shape_plane) {
				if (Check_insert_Plane(coll[i], coll[o]) == false)continue;
			}
			else if (coll[i]->shape != Collider_shape::shape_plane && coll[o]->shape == Collider_shape::shape_plane) {
				if (Check_insert_Plane(coll[o], coll[i]) == false)continue;
			}
			else continue;

			//new_pair.body[0]にアドレスの大きいほうをしまう
			if (coll[i] > coll[o]) {
				new_pair.body[0] = coll[i];
				new_pair.body[1] = coll[o];
			}
			else {
				new_pair.body[0] = coll[o];
				new_pair.body[1] = coll[i];
			}
			new_pair.type = Pairtype::new_pair;

			new_pairs.emplace_back(new_pair);
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


#else
struct edge{
	Collider* coll;
	float value;
	bool stgo; //true = st, false = go 
};

void physics_function::Broadphase(const std::vector<Collider*>& coll, std::vector<Contacts::Collider_2>& out_pair, std::vector<Contacts::Contact_pair>& pairs) {
	//Sweep&Pruneを挿入法で実装
	//現在x軸のみでのSweep&Pruneになっている。
	//用調整
	static std::list<edge>axis_list[3];

	//DOPの更新	

	Work_meter::start("update_dop7");
	int coll_size = coll.size();
	for (int i = 0; i < coll_size; i++) {
		coll[i]->update_dop7();
	}
	Work_meter::stop("update_dop7");

	//{
	//	Contacts::Collider_2 pair;
	//	out_pair.clear();
	//	for (int cnum_0 = 0; cnum_0 < coll.size(); cnum_0++) {
	//		pair.body[0] = coll[cnum_0];
	//		for (int cnum_1 = cnum_0 + 1; cnum_1 < coll.size(); cnum_1++) {
	//			pair.body[1] = coll[cnum_1];
	//			out_pair.emplace_back(pair);
	//		}
	//	}
	//	return;
	//}

	//適当に20コ点を取って適当に
	int SAP_axis = 0;
	{
		vector3 max;
		vector3 min;
		for (int num = 0; num < 20; num++) {

			if (num == 0) {
				max = min = coll[rand() % coll_size]->dop7.pos;
			}
			else {
				vector3 pos = coll[rand() % coll_size]->dop7.pos;
				for (int ax = 0; ax < 3; ax++) {
					if(max[ax] < pos[ax])max[ax] = pos[ax];
					if (min[ax] > pos[ax])min[ax] = pos[ax];
				}
			}
		}

		max -= min;
		int max_num = 0,max_axis = 0;
		for (int i = 0; i < 3; i++) {
			if (max[i] > max_num) {
				max_num = max[i];
				max_axis = i;
			}
		}
		SAP_axis = max_axis;
	}
	Debug::set(std::string("SAP_axis"), vector3(SAP_axis).get_XM3());


	if (coll.size() * 2 != axis_list[0].size()) {
		//TODO : 新しく追加されたもの,なくなったものがわかればさらに最適化できる
		edge ed;

		for (int xz = 0; xz < 3; xz++) {
			axis_list[xz].clear();

			for (int coll_num = 0; coll_num < coll_size; coll_num++) {

				ed.value = coll[coll_num]->dop7.pos[xz] + coll[coll_num]->dop7.max[xz];
				ed.coll = coll[coll_num];
				ed.stgo = false;

				axis_list[xz].emplace_back(ed);
				ed.value = coll[coll_num]->dop7.pos[xz] + coll[coll_num]->dop7.min[xz];
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
				itr->value = itr->coll->dop7.pos[SAP_axis] + itr->coll->dop7.max[SAP_axis];
			}
			else {
				itr->value = itr->coll->dop7.pos[SAP_axis] + itr->coll->dop7.min[SAP_axis];
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
	std::list < Collider*> actives;
	Contacts::Collider_2 pair;
	out_pair.clear();
	std::list<Collider*>::iterator ac = actives.begin();
	std::list<Collider*>::iterator ac_end = actives.end();
	{

#if 0
		std::unordered_map<Collider*, std::vector<Collider*>> pair_mapx;
		std::unordered_map<Collider*, std::vector<Collider*>> pair_mapxy;
		// 明日、x,y,zで別々に配列生成 -> 何が重いのかの確認　やる
		int xy = 0;
		{
			int axis_size = axis_list[xy].size();

			std::list<edge>::iterator itr = axis_list[xy].begin();
			for (int list_num = 0; list_num < axis_size; list_num++) {
				//colliderの始点ならactivelistにあるものと衝突の可能性あり
				if (itr->stgo == true) {

					pair.body[0] = itr->coll;

					ac = actives.begin();
					ac_end = actives.end();
					for (; ac != ac_end; ac++) {
						pair.body[1] = *ac;

						if (pair.body[0] < pair.body[1]) {
							pair_mapx[pair.body[0]].emplace_back(pair.body[1]);
						}
						else {
							pair_mapx[pair.body[1]].emplace_back(pair.body[0]);
						}


					}
					actives.emplace_front(itr->coll);
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
		xy = 1;
		{
			int axis_size = axis_list[xy].size();

			std::list<edge>::iterator itr = axis_list[xy].begin();
			for (int list_num = 0; list_num < axis_size; list_num++) {
				//colliderの始点ならactivelistにあるものと衝突の可能性あり
				if (itr->stgo == true) {

					pair.body[0] = itr->coll;

					ac = actives.begin();
					ac_end = actives.end();
					for (; ac != ac_end; ac++) {
						pair.body[1] = *ac;

						if (pair.body[0] < pair.body[1])
							for (int mapx_num = 0; mapx_num < pair_mapx[pair.body[0]].size(); mapx_num++) {
								if (pair_mapx[pair.body[0]][mapx_num] == pair.body[1]) {
									pair_mapxy[pair.body[0]].emplace_back(pair.body[1]);
									break;
								}

							}

						else
							for (int mapx_num = 0; mapx_num < pair_mapx[pair.body[1]].size(); mapx_num++) {
								int aaa = pair_mapx[pair.body[1]].size();
								if (pair_mapx[pair.body[1]][mapx_num] == pair.body[0]) {
									pair_mapxy[pair.body[1]].emplace_back(pair.body[0]);
									break;
								}

							}

					}
					actives.emplace_front(itr->coll);
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
		xy = 2;
		{
			int axis_size = axis_list[xy].size();

			std::list<edge>::iterator itr = axis_list[xy].begin();
			for (int list_num = 0; list_num < axis_size; list_num++) {
				//colliderの始点ならactivelistにあるものと衝突の可能性あり
				if (itr->stgo == true) {

					pair.body[0] = itr->coll;

					ac = actives.begin();
					ac_end = actives.end();
					for (; ac != ac_end; ac++) {
						pair.body[1] = *ac;

						if (pair.body[0] < pair.body[1])
							for (int mapx_num = 0; mapx_num < pair_mapxy[pair.body[0]].size(); mapx_num++) {
								if (pair_mapxy[pair.body[0]][mapx_num] == pair.body[1]) {
									out_pair.emplace_back(pair);
									break;
								}

							}

						else
							for (int mapx_num = 0; mapx_num < pair_mapxy[pair.body[1]].size(); mapx_num++) {
								if (pair_mapxy[pair.body[1]][mapx_num] == pair.body[0]) {
									out_pair.emplace_back(pair);
									break;
								}

							}
					}
					actives.emplace_front(itr->coll);
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

#elif 1

{
	int axis_size = axis_list[SAP_axis].size();

	std::list<edge>::iterator itr = axis_list[SAP_axis].begin();
	for (int list_num = 0; list_num < axis_size; list_num++) {
		//colliderの始点ならactivelistにあるものと衝突の可能性あり
		if (itr->stgo == true) {

			pair.body[0] = itr->coll;

			ac = actives.begin();
			ac_end = actives.end();
			for (; ac != ac_end; ac++) {
				pair.body[1] = *ac;
				out_pair.emplace_back(pair);
			}
			actives.emplace_front(itr->coll);
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


	Work_meter::stop("Sweep&Prune");



}





#endif