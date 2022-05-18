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


void edge_insert_sort(std::list<Insert_edge*>& axis_list, std::unordered_map<u_int, std::vector<std::list<Insert_edge*>::iterator>>& access_axislist_itr, const int& axis_num) {
	std::list<Insert_edge*>::iterator itr[2] = { axis_list.begin(),axis_list.begin() };
	u_int next_num = 0;

	std::list<Insert_edge*>::iterator itr_insert;
	std::list<Insert_edge*>::iterator itr_end = axis_list.end();


	if (itr[next_num] != itr_end)itr[next_num]++;

	while (true) {
		if (itr[next_num] == itr_end)break;

		const u_int now_num = 1 - next_num;
		const Insert_edge* next_edge = *itr[next_num];
		const Insert_edge* now_edge = *itr[now_num];

		// もし次の値が今の値より
		if (next_edge->pos_value[axis_num] < now_edge->pos_value[axis_num]) {

			const std::list<Insert_edge*>::iterator itr_begin = axis_list.begin();

			// そんなに動いていないことを祈って 今の値から後ろに戻していく(前からだと物が増えると確実に重くなるから)
			for (itr_insert = itr[now_num]; ; --itr_insert) {
				if (next_edge->pos_value[axis_num] > (*itr_insert)->pos_value[axis_num]) {
					++itr_insert;

					axis_list.emplace(itr_insert, *itr[next_num]);
					axis_list.erase(itr[next_num]);

					itr_insert--;
					(*(*itr_insert)->axis_listX_pair_itr)->axis_listX_pair_itr = itr_insert; //相方に保存されている自身へのitrを更新
					access_axislist_itr[(*itr_insert)->shape->get_ALPcollider()->get_index()][(*itr_insert)->shape_index] = itr_insert; //axis_list_edgeに保存したitrの更新

					itr[next_num] = itr[now_num];
					break;
				}

				if (itr_insert == itr_begin) {
					axis_list.emplace(itr_insert, *itr[next_num]);
					axis_list.erase(itr[next_num]);

					itr_insert--;
					(*(*itr_insert)->axis_listX_pair_itr)->axis_listX_pair_itr = itr_insert; //相方に保存されている自身へのitrを更新
					access_axislist_itr[(*itr_insert)->shape->get_ALPcollider()->get_index()][(*itr_insert)->shape_index] = itr_insert; //axis_list_edgeに保存したitrの更新

					itr[next_num] = itr[now_num];
					break;
				}
			}


		}

		// 次のnextに今のnextの次を保存し next_numを更新
		itr[now_num] = itr[next_num];
		++itr[now_num];
		next_num = now_num;
	}
}



void Physics_function::BroadMidphase(
	std::list<Physics_function::ALP_Collider*>& ALP_collider,
	std::vector<Contacts::Contact_pair*>& out_pair,
	std::vector<Physics_function::ALP_Collider*>& moved_collider, //動いたもの
	std::vector<Physics_function::ALP_Collider*>& added_collider, //追加されたもの
	std::mutex& mtx
) {

	// s&pで使用する配列を更新
	{

		Work_meter::start("make_axis_list", 1);
		//::: 追加されたものをaxis_listに追加 :::
		{
			auto& axis_list = axis_listX_;
			auto& access_axislist_itr = access_axislist_itr_;

			for (auto& added : added_collider) {
				if (added == nullptr)continue;

				const u_int collider_index = added->get_index(); //colliderのuniqueなID

				access_axislist_itr[collider_index].clear(); // 初期化

				u_int shape_count = 0;

				for (auto& shape : added->get_shapes()) {

					{
						Insert_edge* ed = newD Insert_edge;
						ed->pos_value[0] = shape->get_DOP().pos[0] + shape->get_DOP().max[0];
						ed->pos_value[1] = shape->get_DOP().pos[2] + shape->get_DOP().max[2];
						ed->shape = shape;
						ed->edge_start = false;
						ed->shape_index = shape_count;
						axis_list.emplace_back(ed);

						shape_count++;
					}
					{
						Insert_edge* ed = newD Insert_edge;
						ed->pos_value[0] = shape->get_DOP().pos[0] + shape->get_DOP().min[0];
						ed->pos_value[1] = shape->get_DOP().pos[2] + shape->get_DOP().min[2];
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

					// 互いにitrを保存
					(*itr_S)->axis_listX_pair_itr = itr_G;
					(*itr_G)->axis_listX_pair_itr = itr_S;

					// colliderからアクセスできるようにaccess_axislist_itrに保存
					access_axislist_itr[collider_index].push_back(itr_S);
					access_axislist_itr[collider_index].push_back(itr_G);
				}
			}
		}


		Work_meter::stop("make_axis_list", 1);

		//::: 動いたもののvalueを更新 :::
		{
			auto& axis_list = axis_listX_;
			auto& access_axislist_itr = access_axislist_itr_;

			Work_meter::start("update_Value", 1);
			{
				std::lock_guard<std::mutex> lock(mtx);

				for (auto& moved : moved_collider) {
					if (moved == nullptr)continue;

					for (auto& edge_itr : access_axislist_itr[moved->get_index()]) {
						auto& edge = *edge_itr;

						if (edge->edge_start == false) {
							edge->pos_value[0] = edge->shape->get_DOP().pos[0] + edge->shape->get_DOP().max[0];
							edge->pos_value[1] = edge->shape->get_DOP().pos[2] + edge->shape->get_DOP().max[2];
						}
						else {
							edge->pos_value[0] = edge->shape->get_DOP().pos[0] + edge->shape->get_DOP().min[0];
							edge->pos_value[1] = edge->shape->get_DOP().pos[2] + edge->shape->get_DOP().min[2];
						}
					}

				}
			}

			for (auto& added : added_collider) {
				if (added == nullptr)continue;

				for (auto& edge_itr : access_axislist_itr[added->get_index()]) {
					auto& edge = *edge_itr;

					if (edge->edge_start == false) {
						edge->pos_value[0] = edge->shape->get_DOP().pos[0] + edge->shape->get_DOP().max[0];
						edge->pos_value[1] = edge->shape->get_DOP().pos[2] + edge->shape->get_DOP().max[2];
					}
					else {
						edge->pos_value[0] = edge->shape->get_DOP().pos[0] + edge->shape->get_DOP().min[0];
						edge->pos_value[1] = edge->shape->get_DOP().pos[2] + edge->shape->get_DOP().min[2];
					}

				}
			}

			Work_meter::stop("update_Value", 1);
		}

		Work_meter::start("insert_sort", 1);
		//::: 挿入ソート :::
		{
			//::: Xの挿入ソート ::::::::::::::
			// small ----> big
			{
				edge_insert_sort(axis_listX_, access_axislist_itr_, 0);
			}

			//::: Zの挿入ソート ::::::::::::::
			// small ----> big / small ----> big / small ----> big /
			{
				auto& axis_list = axis_listZ_;
				auto& access_axislist_itr = access_axislist_itr_;

				const int X_count_num = 20;
				int X_count;
				for (auto edgeZs : axis_list) {
					for (auto edgeZs : axis_list) {
						edge_insert_sort(edgeZs, access_axislist_itr_, 1);
					}
				}

			}
		}
		Work_meter::stop("insert_sort", 1);

	}


	{
		Work_meter::start("Sweep&Prune", 1);
		// Sweep&Prune

		out_pair.clear();

		std::list<Collider_shape*> actives;
		std::list<Collider_shape*> static_actives;

		{
			const auto& axis_list = axis_listX_;

			for (auto& insert_edge_itr : axis_list) {
				auto& insert_edge = *insert_edge_itr;

				// colliderの始点ならactivelistにあるものと衝突の可能性あり
				if (insert_edge.edge_start == true) {

					if (insert_edge.shape->get_ALPcollider()->get_ALPphysics()->is_static == true) { // get_collptr()が

						// staticなオブジェクトはstatic_activesとは衝突しない
						for (auto& shape : actives) {
							Midphase_DOP_14(out_pair, shape, insert_edge.shape);
						}
						static_actives.emplace_back(insert_edge.shape);

						auto pair_itr = static_actives.end();
						--pair_itr;
						insert_edge.active_list_itr = pair_itr;
					}
					else {

						// dynamicなオブジェクトはすべてに可能性がある
						for (auto& shape : static_actives) {
							Midphase_DOP_14(out_pair, shape, insert_edge.shape);
						}
						for (auto& shape : actives) {
							Midphase_DOP_14(out_pair, shape, insert_edge.shape);
						}
						actives.emplace_back(insert_edge.shape);

						auto pair_itr = actives.end();
						--pair_itr;
						insert_edge.active_list_itr = pair_itr;
					}

				}

				else {

					if (insert_edge.shape->get_ALPcollider()->get_ALPphysics()->is_static == true)
						static_actives.erase((*insert_edge.axis_listX_pair_itr)->active_list_itr);
					else
						actives.erase((*insert_edge.axis_listX_pair_itr)->active_list_itr);
				}
			}
		}

	}


	moved_collider.clear();
	added_collider.clear();

	Work_meter::stop("Sweep&Prune", 1);
}


void Physics_function::remove_collider_broad_phase(Physics_function::ALP_Collider* removed) {
	//::: 削除されたものをaxis_listから削除する :::
	{
		const u_int index = removed->get_index();

		if (Broadphase_static::access_axislist_itr_.count(removed->get_index()) == 0)return;

		auto remove_edge_itrs = Broadphase_static::access_axislist_itr_[index];

		for (auto& edge : remove_edge_itrs) {
			delete* edge;
			Broadphase_static::axis_listX_.erase(edge);

		}
	}
}

// DOP14による大雑把な当たり判定
bool Broadphase_static::Check_insert_DOP14(const Collider_shape* meshA, const Collider_shape* meshB) {
	using namespace DirectX;
	// 無限PlaneはDOPが作れないためnarrowに投げる?
	// if (meshA->shape == ALP_Collider_shape::Plane || meshB->shape == ALP_Collider_shape::Plane) return true;

	for (int i = 0; i < DOP_size; i++) {

		const float dis = vector3_dot(DOP_14_axis[i], meshA->get_DOP().pos - meshB->get_DOP().pos);

		if (
			0 > +dis + meshA->get_DOP().max[i] - meshB->get_DOP().min[i] ||
			0 > -dis + meshB->get_DOP().max[i] - meshA->get_DOP().min[i]
			) {
			return false;
		}
	}

	// SIMDに変換が重かった
	//{
	//	const auto& A_ = meshA->get_DOP().pos;
	//	const auto& B_ = meshB->get_DOP().pos;

	//	const XMVECTOR A_pos = XMLoadFloat3(&A_);
	//	const XMVECTOR B_pos = XMLoadFloat3(&B_);
	//	const XMVECTOR dis = A_pos - B_pos;

	//	const XMVECTOR Amax_sim = XMLoadFloat(&meshA->get_DOP().max[0]);
	//	const XMVECTOR Bmax_sim = XMLoadFloat(&meshB->get_DOP().max[0]);
	//	const XMVECTOR Amin_sim = XMLoadFloat(&meshA->get_DOP().min[0]);
	//	const XMVECTOR Bmin_sim = XMLoadFloat(&meshB->get_DOP().min[0]);

	//	if (
	//		XMVector3Greater(-dis,  Amax_sim - Bmin_sim) ||
	//		XMVector3Greater(+dis,  Bmax_sim - Amin_sim)
	//		) {
	//		return false;
	//	}
	//}

	return true;
}
bool Broadphase_static::Check_insert_Plane(const Collider_shape* plane, const Collider_shape* mesh) {

	Vector3 V;
	float plane_dis = 0, coll_dis = FLT_MAX;

	V = vector3_quatrotate(Vector3(0, 1, 0), plane->world_orientation());
	plane_dis = vector3_dot(V, plane->world_position());

	for (int i = 0; i < DOP_size; i++) {
		float coll_len = vector3_dot(V, mesh->world_position() + DOP_14_axis[i] * mesh->get_DOP().max[i]);
		if (plane_dis > coll_len)return true;
	}

	return false;
}
void Broadphase_static::Midphase_DOP_14(std::vector<Contacts::Contact_pair*>& new_pairs, Collider_shape* meshA, Collider_shape* meshB) {

	if (meshA->get_ALPcollider()->get_gameobject() == meshB->get_ALPcollider()->get_gameobject())return; //同じGOにアタッチされたshape同士は衝突しない

	const ALP_Collider* collA = meshA->get_ALPcollider();
	const ALP_Collider* collB = meshB->get_ALPcollider();

	bool is_generate_contact = true;
	// sleep同士の場合は衝突検知のみ
	// if (collA->get_ALPphysics()->is_sleep == true && collB->get_ALPphysics()->is_sleep == true)is_generate_contact = false;

	// タグによる衝突の是非
	if (collA->get_ALPphysics()->is_hitable == false || (meshA->get_tag() & meshB->get_ignore_tags())) is_generate_contact = false;
	if (collB->get_ALPphysics()->is_hitable == false || (meshB->get_tag() & meshA->get_ignore_tags())) is_generate_contact = false;


	bool check_oncoll_only = false;
	if (is_generate_contact == false) {
		if (collA->get_oncoll_check_bits() & meshB->get_tag())check_oncoll_only = true;
		if (collB->get_oncoll_check_bits() & meshA->get_tag())check_oncoll_only = true;
	}
	if (is_generate_contact == false && check_oncoll_only == false)return;

	// DOPによる大雑把な当たり判定
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
	// new_pair.body[0]にアドレスの大きいほうをしまう
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
