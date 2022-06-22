#pragma once
#include <memory>
#include <unordered_map>

#include "ALP_contact.h"
#include "ALP_physics.h"
#include "collider_shape.h"

namespace Adollib {
	namespace Physics_function {
		// 挿入法の
		struct Insert_edge {
			Collider_shape* shape; //shapeへのポインタ
			float pos_value[2] = { 0 }; //値 0:X 1:Z
			bool edge_start = false; //true = start, false = goal
			u_int shape_index = 0; //Insert_edgeの情報から対応するaccess_axislist_itr_にアクセスできるように

			std::list<Insert_edge*>::iterator axis_listX_pair_itr;     //このedgeのpairとなるedgeへのitr
			std::list<Collider_shape*>::iterator active_list_itr;     //edgeのactivelistへのイテレータ
		};

		namespace Broadphase_static {

			static struct Edge_sort_axis {
				constexpr static u_int X = 0;
				constexpr static u_int Z = 1;
			};


			static std::list<Insert_edge*> axis_listX_;
			static std::vector<Insert_edge*> axis_listZ_; //std::sort使うからvector

			static std::unordered_map<u_int, std::vector<std::list<Insert_edge*>::iterator>> access_axislist_itr_; //colliderのindex情報から対応するaxis_listへアクセスできるようにする

			bool Check_insert_DOP14(const Collider_shape* meshA, const Collider_shape* meshB);
			bool Check_insert_Plane(const Collider_shape* plane, const Collider_shape* mesh);
			void Midphase_DOP_14(std::vector<Contacts::Contact_pair*>& new_pairs, Collider_shape* meshA, Collider_shape* meshB);

			void SweepAndPrune(std::list<Collider_shape*>& actives, std::list<Collider_shape*>& static_actives, Insert_edge* insert_edge, std::vector<Contacts::Contact_pair*>& out_pair);
		}

		// とても大雑把な当たり判定(Boardphase) と DOP(Midphase)を行う
		// sweep&pruneの情報をコピーするのが重いため、Midphaseもここで行う
		void BroadMidphase(
			std::list<Physics_function::ALP_Collider*>& ALP_colliders,
			//std::vector<Contacts::Collider_2>& out_pair,
			std::vector < Contacts::Contact_pair*>& out_pair,
			std::vector<Physics_function::ALP_Collider*>& moved_collider, //動いたもの
			std::vector<Physics_function::ALP_Collider*>& added_collider, //追加されたもの
			std::mutex& mtx
		);

		void remove_collider_broad_phase(Physics_function::ALP_Collider* removed);

	}
}
