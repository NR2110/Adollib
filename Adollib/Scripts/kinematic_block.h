#pragma once

#include "stageparts_base.h"

namespace Adollib {

	namespace Stage_parts {
		// ���������� respown_pos�ɖ߂��K�v�����邽��component��

		class Kinematic_block : public Stageparts_base {

		public:
			Vector3 respown_pos;
			Quaternion respown_rotate;
			Gameobject* pearent = nullptr;

		private:


		public:

			// addComponent���ꂽ�Ƃ��ɌĂ΂��
			void awake() override {};

			// ���t���[���Ă΂��X�V����
			void update() override {
				if (transform->position.y < this_stage->y_respown_limit) {
					if (pearent == nullptr) {
						this_coll->reset_force();
						this_coll->linear_velocity(Vector3(0, -1, 0)); //feeze�Ɉ���������Ȃ��悤��
						transform->local_pos = respown_pos;
						transform->local_pos.y = this_stage->y_respown_pos;
						transform->local_orient = respown_rotate;
					}
					else {
						Vector3 off = Vector3(respown_pos.x, this_stage->y_respown_pos, respown_pos.y) - transform->local_pos;
						for (auto& child : *pearent->children()) {
							child->transform->local_pos += off;
							auto coll = child->findComponent<Collider>();
							if (coll == nullptr)continue;
							coll->reset_force();
							coll->linear_velocity(Vector3(0, -1, 0)); //feeze�Ɉ���������Ȃ��悤��
						}
					}
				}
			};

		};


	}
}