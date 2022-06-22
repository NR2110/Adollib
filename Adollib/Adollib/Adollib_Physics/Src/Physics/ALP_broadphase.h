#pragma once
#include <memory>
#include <unordered_map>

#include "ALP_contact.h"
#include "ALP_physics.h"
#include "collider_shape.h"

namespace Adollib {
	namespace Physics_function {
		// �}���@��
		struct Insert_edge {
			Collider_shape* shape; //shape�ւ̃|�C���^
			float pos_value[2] = { 0 }; //�l 0:X 1:Z
			bool edge_start = false; //true = start, false = goal
			u_int shape_index = 0; //Insert_edge�̏�񂩂�Ή�����access_axislist_itr_�ɃA�N�Z�X�ł���悤��

			std::list<Insert_edge*>::iterator axis_listX_pair_itr;     //����edge��pair�ƂȂ�edge�ւ�itr
			std::list<Collider_shape*>::iterator active_list_itr;     //edge��activelist�ւ̃C�e���[�^
		};

		namespace Broadphase_static {

			static struct Edge_sort_axis {
				constexpr static u_int X = 0;
				constexpr static u_int Z = 1;
			};


			static std::list<Insert_edge*> axis_listX_;
			static std::vector<Insert_edge*> axis_listZ_; //std::sort�g������vector

			static std::unordered_map<u_int, std::vector<std::list<Insert_edge*>::iterator>> access_axislist_itr_; //collider��index��񂩂�Ή�����axis_list�փA�N�Z�X�ł���悤�ɂ���

			bool Check_insert_DOP14(const Collider_shape* meshA, const Collider_shape* meshB);
			bool Check_insert_Plane(const Collider_shape* plane, const Collider_shape* mesh);
			void Midphase_DOP_14(std::vector<Contacts::Contact_pair*>& new_pairs, Collider_shape* meshA, Collider_shape* meshB);

			void SweepAndPrune(std::list<Collider_shape*>& actives, std::list<Collider_shape*>& static_actives, Insert_edge* insert_edge, std::vector<Contacts::Contact_pair*>& out_pair);
		}

		// �ƂĂ���G�c�ȓ����蔻��(Boardphase) �� DOP(Midphase)���s��
		// sweep&prune�̏����R�s�[����̂��d�����߁AMidphase�������ōs��
		void BroadMidphase(
			std::list<Physics_function::ALP_Collider*>& ALP_colliders,
			//std::vector<Contacts::Collider_2>& out_pair,
			std::vector < Contacts::Contact_pair*>& out_pair,
			std::vector<Physics_function::ALP_Collider*>& moved_collider, //����������
			std::vector<Physics_function::ALP_Collider*>& added_collider, //�ǉ����ꂽ����
			std::mutex& mtx
		);

		void remove_collider_broad_phase(Physics_function::ALP_Collider* removed);

	}
}
