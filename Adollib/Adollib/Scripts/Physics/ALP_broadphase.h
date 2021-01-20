#pragma once
#include <memory>
#include "contact.h"
#include "ALP_physics.h"

#include "../Scene/scene.h"

namespace Adollib {
	namespace Physics_function {
		//挿入法の
		struct Insert_edge {
			ALP_Collider_mesh* mesh;
			float value = 0;
			bool edge_start = false; //true = st, false = go
			u_int mesh_index = 0;
			std::list<Insert_edge>::iterator axis_list_pair_itr;
			std::list<ALP_Collider_mesh*>::iterator active_list_pair_itr;
		};

		namespace Broadphase_static {

			static std::unordered_map <Scenelist, std::list<Insert_edge>> axis_list_pS;
			static std::unordered_map <Scenelist, std::unordered_map<u_int, std::vector<std::list<Insert_edge>::iterator>>> axis_list_edge_pS;

		}

		//とても大雑把な当たり判定(Boardphase)
		void Broadphase(Scenelist Sce,
			std::list<ALP_Collider>& ALP_collider, std::vector<Contacts::Collider_2>& out_pair,
			std::vector<std::list<Physics_function::ALP_Collider>::iterator>& moved_collider, //動いたもの
			std::vector<std::list<Physics_function::ALP_Collider>::iterator>& added_collider //追加されたもの
		);

		void remove_collider_broad_phase(std::list<Physics_function::ALP_Collider>::iterator removed);

	}
}