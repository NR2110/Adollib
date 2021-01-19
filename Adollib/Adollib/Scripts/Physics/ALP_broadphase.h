#pragma once
#include <memory>
#include "contact.h"
#include "ALP_physics.h"

#include "../Scene/scene.h"

namespace Adollib {
	namespace Physics_function {
		//‘}“ü–@‚Ì
		struct Insert_edge {
			std::vector<ALP_Collider_mesh>::iterator mesh;
			float value = 0;
			bool stgo = false; //true = st, false = go
			u_int mesh_index = 0;
		};

		namespace Broadphase_static {

			static std::unordered_map <Scenelist, std::list<Insert_edge>> axis_list_pS;
			static std::unordered_map <Scenelist, std::unordered_map<u_int, std::vector<std::list<Insert_edge>::iterator>>> axis_list_edge_pS;

		}

		//‚Æ‚Ä‚à‘åŽG”c‚È“–‚½‚è”»’è(Boardphase)
		void Broadphase(Scenelist Sce,
			std::list<ALP_Collider>& ALP_collider, std::vector<Contacts::Collider_2>& out_pair,
			std::vector<std::list<Physics_function::ALP_Collider>::iterator>& moved_collider, //“®‚¢‚½‚à‚Ì
			std::vector<std::list<Physics_function::ALP_Collider>::iterator>& added_collider //’Ç‰Á‚³‚ê‚½‚à‚Ì
		);

		void remove_collider_broad_phase(std::list<Physics_function::ALP_Collider>::iterator removed);

	}
}