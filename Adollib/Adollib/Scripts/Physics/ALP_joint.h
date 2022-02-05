#pragma once
#include "../Math/math.h"
#include "ALP_contact.h"
#include "ALP_anchor.h"

#include "ALP_struct_world_transform.h"

namespace Adollib {
	class Joint_base;

	namespace Physics_function {

		//physics_manager�ɕۑ�����Ă���
		class ALP_Joint {

		public:
			explicit ALP_Joint(const std::list<ALP_Joint*>::iterator l_this_itr) :this_itr(l_this_itr) {};

		public:
			float shrink_bias = 0.1f; // �S���̋����̒����l(�k)
			float stretch_bias = 0.1f; // �S���̋����̒����l(�L)
			float slop = 0; // �S���̋��e�덷
			float offset = 0; //�S���̏ꏊ�̂��� (ex. �z��anchor = 0,0 offset = ���_�̋��� �Ŏ������Ă���)
			float limit_bias = 0;

			int anchor_count = 0; //anchor���������邩
			const Anchor* anchor = nullptr; // ���̂̃��[�J�����W�n�ɂ�����ڑ��_

			//Collider* collider_comp[2] = { nullptr }; // ���̂�component�ւ̃|�C���^
			ALP_Collider* ALPcollider[2] = { nullptr }; // ���̂ւ̃|�C���^

			Contacts::Constraint constraint[3]; // anchor�S���v�Z�p

			Vector3 limit_constraint_pos[2]; //limit�S���v�Z�p �S���_
			Contacts::Constraint constraint_limit; // limit�S���v�Z�p

			Adollib::Joint_base* userjoint = nullptr; //���[�U�[�̐G����ւ̃|�C���^

			//::: added�̔z�񂩂烁�C���̔z���adapt���ꂽ��true�ɂ��� (itr��added���w���̂�main�̔z����h���̂��킩��Ȃ�����)
			bool is_added = false;

		private:
			std::list<ALP_Joint*>::iterator this_itr{}; //physics_manager���̎��g�ւ̃C�e���[�^

		public:
			void set_this_itr(std::list<ALP_Joint*>::iterator itr) { this_itr = itr; };

			void set_ptr_to_joint(ALP_Joint* joint); //collider_component�폜���� �������joint���폜�ł���悤�ɕۑ�����

		public:
			void reset();

			void adapt_Jointdata();

			void apply_joint_velocityeffect();

			void destroy(ALP_Collider* coll_ptr = nullptr);

		};
	}

}