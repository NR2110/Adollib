#include "ALP_broadphase.h"
#include <list>


#include "ALP__physics_manager.h"


using namespace Adollib;
using namespace Physics_function;
using namespace Contacts;
using namespace DOP;

using namespace Broadphase_static;


void edge_insert_sort(std::list<Insert_edge*>& axis_list, const int& axis_num, std::unordered_map<u_int, std::vector<std::list<Insert_edge*>::iterator>>& access_axislist_itr) {
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

		// �������̒l�����̒l���
		if (next_edge->pos_value[axis_num] < now_edge->pos_value[axis_num]) {

			const std::list<Insert_edge*>::iterator itr_begin = axis_list.begin();

			// ����Ȃɓ����Ă��Ȃ����Ƃ��F���� ���̒l������ɖ߂��Ă���(�O���炾�ƕ���������Ɗm���ɏd���Ȃ邩��)
			for (itr_insert = itr[now_num]; ; --itr_insert) {
				if (next_edge->pos_value[axis_num] > (*itr_insert)->pos_value[axis_num]) {
					++itr_insert;

					axis_list.emplace(itr_insert, *itr[next_num]);
					axis_list.erase(itr[next_num]);

					itr_insert--;
					(*(*itr_insert)->axis_listX_pair_itr)->axis_listX_pair_itr = itr_insert; //�����ɕۑ�����Ă��鎩�g�ւ�itr���X�V
					access_axislist_itr[(*itr_insert)->shape->get_ALPcollider()->get_index()][(*itr_insert)->shape_index] = itr_insert; //ID����A�N�Z�X�ł���悤��axis_list_edge�ɕۑ�����itr�̍X�V (�폜�p)

					itr[next_num] = itr[now_num];
					break;
				}

				if (itr_insert == itr_begin) {
					axis_list.emplace(itr_insert, *itr[next_num]);
					axis_list.erase(itr[next_num]);

					itr_insert--;
					(*(*itr_insert)->axis_listX_pair_itr)->axis_listX_pair_itr = itr_insert; //�����ɕۑ�����Ă��鎩�g�ւ�itr���X�V
					access_axislist_itr[(*itr_insert)->shape->get_ALPcollider()->get_index()][(*itr_insert)->shape_index] = itr_insert; //ID����A�N�Z�X�ł���悤��axis_list_edge�ɕۑ�����itr�̍X�V (�폜�p)

					itr[next_num] = itr[now_num];
					break;
				}
			}


		}

		// ����next�ɍ���next�̎���ۑ��� next_num���X�V
		itr[now_num] = itr[next_num];
		++itr[now_num];
		next_num = now_num;
	}
}

void edge_quick_sort(std::vector<Insert_edge*>& axis_list, const int& axis_num, const std::vector<Insert_edge*>::iterator& itr_end) {
	//bool sortmaterial(const Insert_edge * left, const Insert_edge * Right)
	//{
	//	return left->get_materialnum() < Right->get_materialnum();
	//}
	std::sort(axis_list.begin(), itr_end,
		[&](const Insert_edge* left, const Insert_edge* Right){
			return left->pos_value[axis_num] < Right->pos_value[axis_num];
		}
	);

	//axis_list.sort([&](const Insert_edge* left, const Insert_edge* Right) {
	//	return left->pos_value[axis_num] > Right->pos_value[axis_num];
	//	});

	//std::list<Insert_edge*>::iterator itr[2] = { axis_list.begin(),axis_list.begin() };
	//u_int next_num = 0;

	//std::list<Insert_edge*>::iterator itr_insert;


	//if (itr[next_num] != itr_end)itr[next_num]++;

	//while (true) {
	//	if (itr[next_num] == itr_end)break;

	//	const u_int now_num = 1 - next_num;
	//	const Insert_edge* next_edge = *itr[next_num];
	//	const Insert_edge* now_edge = *itr[now_num];

	//	// �������̒l�����̒l���
	//	if (next_edge->pos_value[axis_num] < now_edge->pos_value[axis_num]) {

	//		const std::list<Insert_edge*>::iterator itr_begin = axis_list.begin();

	//		// ����Ȃɓ����Ă��Ȃ����Ƃ��F���� ���̒l������ɖ߂��Ă���(�O���炾�ƕ���������Ɗm���ɏd���Ȃ邩��)
	//		for (itr_insert = itr[now_num]; ; --itr_insert) {
	//			if (next_edge->pos_value[axis_num] > (*itr_insert)->pos_value[axis_num]) {
	//				++itr_insert;

	//				axis_list.emplace(itr_insert, *itr[next_num]);
	//				axis_list.erase(itr[next_num]);

	//				itr_insert--;

	//				itr[next_num] = itr[now_num];
	//				break;
	//			}

	//			if (itr_insert == itr_begin) {
	//				axis_list.emplace(itr_insert, *itr[next_num]);
	//				axis_list.erase(itr[next_num]);

	//				itr_insert--;

	//				itr[next_num] = itr[now_num];
	//				break;
	//			}
	//		}


	//	}

	//	// ����next�ɍ���next�̎���ۑ��� next_num���X�V
	//	itr[now_num] = itr[next_num];
	//	++itr[now_num];
	//	next_num = now_num;
	//}
}



void Physics_function::BroadMidphase(
	std::list<Physics_function::ALP_Collider*>& ALP_collider,
	std::vector<Contacts::Contact_pair*>& out_pair,
	std::vector<Physics_function::ALP_Collider*>& moved_collider, //����������
	std::vector<Physics_function::ALP_Collider*>& added_collider, //�ǉ����ꂽ����
	std::mutex& mtx
) {

	// s&p�Ŏg�p����z����X�V
	{

		//::: �ǉ����ꂽ���̂�axis_list�ɒǉ� :::
		{
			auto& axis_list = axis_listX_;
			auto& access_axislist_itr = access_axislist_itr_;

			for (auto& added : added_collider) {
				if (added == nullptr)continue;

				const u_int collider_index = added->get_index(); //collider��unique��ID

				access_axislist_itr[collider_index].clear(); // ������

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

					// �݂���itr��ۑ�
					(*itr_S)->axis_listX_pair_itr = itr_G;
					(*itr_G)->axis_listX_pair_itr = itr_S;

					// collider����A�N�Z�X�ł���悤��access_axislist_itr�ɕۑ�
					access_axislist_itr[collider_index].push_back(itr_S);
					access_axislist_itr[collider_index].push_back(itr_G);
				}
			}
		}

		//::: ���������̂�value���X�V :::
		{
			auto& axis_list = axis_listX_;
			auto& access_axislist_itr = access_axislist_itr_;

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

				moved_collider.clear();
				added_collider.clear();
			}

		}

		//::: �}���\�[�g :::
		{
			//::: X�̑}���\�[�g ::::::::::::::
			// small ----> big
			{
				edge_insert_sort(axis_listX_, Edge_sort_axis::X, access_axislist_itr_);
			}
		}

	}

	// Sweep&Prune
	{

		out_pair.clear();

		std::list<Collider_shape*> actives;
		std::list<Collider_shape*> static_actives;

		{
			const auto& axis_listX = axis_listX_;
			auto& axis_listZ = axis_listZ_;

			// Z���p��list X����20���ۑ����AZ���Ń\�[�g���s��
			//std::vector<Insert_edge*> axis_listZ;
			axis_listZ.resize(Physics_manager::physicsParams.sweep_and_prune_divide_value);
			std::vector<Insert_edge*>::iterator resorce_copy_itr = axis_listZ.begin();
			std::vector<Insert_edge*>::iterator resorce_copy_itr_end = axis_listZ.end();


			for (auto& insert_edgeX_itr : axis_listX) {

				*resorce_copy_itr = insert_edgeX_itr;
				++resorce_copy_itr;
				if (resorce_copy_itr != resorce_copy_itr_end) continue;

				// Z���\�[�g
				{
					edge_quick_sort(axis_listZ, Edge_sort_axis::Z, resorce_copy_itr_end);
				}

				// SweepAndPrune
				for (auto& insert_edgeZ_ptr : axis_listZ) {
					SweepAndPrune(actives, static_actives, insert_edgeZ_ptr, out_pair);
				}

				resorce_copy_itr = axis_listZ.begin();
			}
			edge_quick_sort(axis_listZ, Edge_sort_axis::Z, resorce_copy_itr);
			for (auto& insert_edgeZ_ptr : axis_listZ) {
				if (insert_edgeZ_ptr == *resorce_copy_itr)break;
				SweepAndPrune(actives, static_actives, insert_edgeZ_ptr, out_pair);
			}


		}
	}
}


	void Physics_function::remove_collider_broad_phase(Physics_function::ALP_Collider * removed) {
		//::: �폜���ꂽ���̂�axis_list����폜���� :::
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

	// DOP14�ɂ���G�c�ȓ����蔻��
	bool Broadphase_static::Check_insert_DOP14(const Collider_shape * meshA, const Collider_shape * meshB) {
		using namespace DirectX;
		// ����Plane��DOP�����Ȃ�����narrow�ɓ�����?
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

		// SIMD�ɕϊ����d������
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
	bool Broadphase_static::Check_insert_Plane(const Collider_shape * plane, const Collider_shape * mesh) {

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
	void Broadphase_static::Midphase_DOP_14(std::vector<Contacts::Contact_pair*> & new_pairs, Collider_shape * meshA, Collider_shape * meshB) {

		if (meshA->get_ALPcollider()->get_UUID() == meshB->get_ALPcollider()->get_UUID())return; //����GO�ɃA�^�b�`���ꂽshape���m�͏Փ˂��Ȃ�

		const ALP_Collider* collA = meshA->get_ALPcollider();
		const ALP_Collider* collB = meshB->get_ALPcollider();

		bool is_generate_contact = true;
		// sleep���m�̏ꍇ�͏Փˌ��m�̂�
		// if (collA->get_ALPphysics()->is_sleep == true && collB->get_ALPphysics()->is_sleep == true)is_generate_contact = false;

		// �^�O�ɂ��Փ˂̐���
		if (collA->get_ALPphysics()->is_hitable == false || (meshA->get_tag() & meshB->get_ignore_tags())) is_generate_contact = false;
		if (collB->get_ALPphysics()->is_hitable == false || (meshB->get_tag() & meshA->get_ignore_tags())) is_generate_contact = false;


		bool check_oncoll_only = false;
		if (is_generate_contact == false) {
			if (collA->get_oncoll_check_bits() & meshB->get_tag())check_oncoll_only = true;
			if (collB->get_oncoll_check_bits() & meshA->get_tag())check_oncoll_only = true;
		}
		if (is_generate_contact == false && check_oncoll_only == false)return;

		// DOP�ɂ���G�c�ȓ����蔻��
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
		// new_pair.body[0]�ɃA�h���X�̑傫���ق������܂�
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

	void Broadphase_static::SweepAndPrune(std::list<Collider_shape*>& actives, std::list<Collider_shape*>& static_actives, Insert_edge* insert_edge, std::vector<Contacts::Contact_pair*>& out_pair) {

			// collider�̎n�_�Ȃ�activelist�ɂ�����̂ƏՓ˂̉\������
			if (insert_edge->edge_start == true) {
				if (insert_edge->shape->get_ALPcollider()->get_ALPphysics()->is_static == true) { // get_collptr()��

					// static�ȃI�u�W�F�N�g��static_actives�Ƃ͏Փ˂��Ȃ�
					for (auto& shape : actives) {
						Midphase_DOP_14(out_pair, shape, insert_edge->shape);
					}
					static_actives.emplace_back(insert_edge->shape);

					auto pair_itr = static_actives.end();
					--pair_itr;
					insert_edge->active_list_itr = pair_itr;
				}
				else {

					// dynamic�ȃI�u�W�F�N�g�͂��ׂĂɉ\��������
					for (auto& shape : static_actives) {
						Midphase_DOP_14(out_pair, shape, insert_edge->shape);
					}
					for (auto& shape : actives) {
						Midphase_DOP_14(out_pair, shape, insert_edge->shape);
					}
					actives.emplace_back(insert_edge->shape);

					auto pair_itr = actives.end();
					--pair_itr;
					insert_edge->active_list_itr = pair_itr;
				}

			}

			else {

				if (insert_edge->shape->get_ALPcollider()->get_ALPphysics()->is_static == true)
					static_actives.erase((*insert_edge->axis_listX_pair_itr)->active_list_itr);
				else
					actives.erase((*insert_edge->axis_listX_pair_itr)->active_list_itr);
			}


	}
