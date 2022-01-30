#pragma once

#include "stageparts_base.h"
#include "../Adollib/Scripts/Main/Adollib.h"

namespace Adollib {

	namespace Stage_parts {
		// �w���tag��1�ł���Γ����M�~�b�N
		class Touch_moveable : public Stageparts_base {
		public:
			Collider_tagbit tag = 0; //����tag�̃I�u�W�F�N�g�ɐڐG������ colliders��moveable��true�ɂ���
			std::vector<Collider*> judge_colliders; //����collider��tag��collider���ڐG������
			std::vector<Collider*> move_colliders;  //����collider��moveable��true�ɂ���


		private:



		public:

			// addComponent���ꂽ�Ƃ��ɌĂ΂��
			void awake() override {};

			// ���t���[���Ă΂��X�V����
			void update() override {

				bool is_contact = false;
				for (auto& coll : judge_colliders) {
					if (coll->concoll_enter(tag)) is_contact = true;
					if (is_contact)break;
				}

				if (is_contact) {
					for (auto& coll : move_colliders)coll->physics_data.is_moveable = true;
				}


			};

		};


	}
}

