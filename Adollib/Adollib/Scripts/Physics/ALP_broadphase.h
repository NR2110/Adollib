#pragma once
#include <memory>
#include <unordered_map>

#include "ALP_contact.h"
#include "ALP_physics.h"
#include "collider_shape.h"

#include "../Scene/scene_list.h"

namespace Adollib {
	namespace Physics_function {
		// �}���@��
		struct Insert_edge {
			Collider_shape* shape; //shape�ւ̃|�C���^
			float value = 0; //�l
			bool edge_start = false; //true = start, false = goal
			u_int shape_index = 0; //Insert_edge�̏�񂩂�Ή�����access_axislist_itr�ɃA�N�Z�X�ł���悤��

			std::list<Insert_edge*>::iterator axis_list_pair_itr;     //����edge��pair�ƂȂ�edge�ւ�itr
			std::list<Collider_shape*>::iterator active_list_pair_itr;//����edge��pair�ƂȂ�edge��activelist�ւ̃C�e���[�^
		};

		namespace Broadphase_static {

			static std::unordered_map<Scenelist, std::list<Insert_edge*>> axis_list_;

			static std::unordered_map<Scenelist, std::unordered_map<u_int, std::vector<std::list<Insert_edge*>::iterator>>> access_axislist_itr_; //collider��index��񂩂�Ή�����axis_list�փA�N�Z�X�ł���悤�ɂ���

		}

		// �ƂĂ���G�c�ȓ����蔻��(Boardphase) �� DOP(Midphase)���s��
		// sweep&prune�̏����R�s�[����̂��d�����߁AMidphase�������ōs��
		void BroadMidphase(const std::list<Scenelist>& active_scenes,
			std::unordered_map<Scenelist, std::list<Physics_function::ALP_Collider*>>& ALP_colliders,
			//std::vector<Contacts::Collider_2>& out_pair,
			std::vector < Contacts::Contact_pair*> & out_pair,
			std::unordered_map<Scenelist, std::vector<Physics_function::ALP_Collider*>>& moved_collider, //����������
			std::unordered_map<Scenelist, std::vector<Physics_function::ALP_Collider*>>& added_collider, //�ǉ����ꂽ����
			std::mutex& mtx
		);

		void remove_collider_broad_phase(Physics_function::ALP_Collider* removed);

	}
}