#pragma once
#include <memory>
#include "contact.h"
#include "ALP_physics.h"
#include "collider_shape.h"

#include "../Scene/scene.h"

namespace Adollib {
	namespace Physics_function {
		//�}���@��
		struct Insert_edge {
			Collider_shape* shape; //
			float value = 0; //
			bool edge_start = false; //true = st, false = go
			u_int mesh_index = 0;
			std::list<Insert_edge>::iterator axis_list_pair_itr;
			std::list<Collider_shape*>::iterator active_list_pair_itr; //����edge��pair�ƂȂ�edge�ւ�itr
		};

		namespace Broadphase_static {

			static std::unordered_map <Scenelist, std::list<Insert_edge>> axis_list_pS;
			static std::unordered_map <Scenelist, std::unordered_map<u_int, std::vector<std::list<Insert_edge>::iterator>>> axis_list_edge_pS;

		}

		//�ƂĂ���G�c�ȓ����蔻��(Boardphase)
		void Broadphase(Scenelist Sce,
			const std::list<ALP_Collider*>& ALP_collider,
			std::vector<Contacts::Collider_2>& out_pair,
			std::vector<Physics_function::ALP_Collider*>& moved_collider, //����������
			std::vector<Physics_function::ALP_Collider*>& added_collider //�ǉ����ꂽ����
		);

		void remove_collider_broad_phase(Physics_function::ALP_Collider* removed);

	}
}