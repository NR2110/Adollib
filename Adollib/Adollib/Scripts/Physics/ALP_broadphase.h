#pragma once
#include <memory>
#include <unordered_map>

#include "ALP_contact.h"
#include "ALP_physics.h"
#include "collider_shape.h"

#include "../Scene/scene_list.h"

namespace Adollib {
	namespace Physics_function {
		// 挿入法の
		struct Insert_edge {
			Collider_shape* shape; //shapeへのポインタ
			float value = 0; //値
			bool edge_start = false; //true = start, false = goal
			u_int shape_index = 0; //Insert_edgeの情報から対応するaccess_axislist_itrにアクセスできるように

			std::list<Insert_edge*>::iterator axis_list_pair_itr;     //このedgeのpairとなるedgeへのitr
			std::list<Collider_shape*>::iterator active_list_pair_itr;//このedgeのpairとなるedgeのactivelistへのイテレータ
		};

		namespace Broadphase_static {

			static std::unordered_map<Scenelist, std::list<Insert_edge*>> axis_list_;

			static std::unordered_map<Scenelist, std::unordered_map<u_int, std::vector<std::list<Insert_edge*>::iterator>>> access_axislist_itr_; //colliderのindex情報から対応するaxis_listへアクセスできるようにする

		}

		// とても大雑把な当たり判定(Boardphase) と DOP(Midphase)を行う
		// sweep&pruneの情報をコピーするのが重いため、Midphaseもここで行う
		void BroadMidphase(const std::list<Scenelist>& active_scenes,
			std::unordered_map<Scenelist, std::list<Physics_function::ALP_Collider*>>& ALP_colliders,
			//std::vector<Contacts::Collider_2>& out_pair,
			std::vector < Contacts::Contact_pair*> & out_pair,
			std::unordered_map<Scenelist, std::vector<Physics_function::ALP_Collider*>>& moved_collider, //動いたもの
			std::unordered_map<Scenelist, std::vector<Physics_function::ALP_Collider*>>& added_collider, //追加されたもの
			std::mutex& mtx
		);

		void remove_collider_broad_phase(Physics_function::ALP_Collider* removed);

	}
}