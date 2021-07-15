#pragma once
#include "../Math/math.h"
#include "../Scene/scene_list.h"
#include "contact.h"
#include "ALP_anchor.h"

namespace Adollib {

	namespace Physics_function {
		class Joint_base;

		//phasics_manager�ɕۑ�����Ă���
		class ALP_Joint {

		public:
			ALP_Joint(std::list<ALP_Joint*>::iterator l_this_itr) :this_itr(l_this_itr) {};

		public:
			float bias = 0; // �S���̋����̒����l

			int anchor_count = 0; //anchor���������邩
			Anchor* anchor; // ���̂̃��[�J�����W�n�ɂ�����ڑ��_

			Collider* collider_comp[2] = { nullptr }; // ���̂�component�ւ̃|�C���^
			ALP_Collider* ALPcollider[2] = { nullptr }; // ���̂ւ̃|�C���^

			Contacts::Constraint constraint[3]; // anchor�S���v�Z�p

			Vector3 limit_constraint_pos[2]; //limit�S���v�Z�p �S���_
			Contacts::Constraint constraint_limit; // limit�S���v�Z�p

			Joint_base* joint = nullptr; //���[�U�[�̐G����ւ̃|�C���^

		private:
			std::list<ALP_Joint*>::iterator this_itr; //physics_manager���̎��g�ւ̃C�e���[�^


		public:
			void reset();

			void adapt_Jointdata();

			void destroy(ALP_Collider* coll_ptr = nullptr, const bool is_delete_userjoint = true);

		};
	}

}