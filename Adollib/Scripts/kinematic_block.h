#pragma once

#include "stageparts_base.h"

namespace Adollib {

	namespace Stage_parts {
		// ���������� respown_pos�ɖ߂��K�v�����邽��component��

		class Kinematic_block : public Stageparts_base {

		public:
			Vector3 respown_pos;
			Quaternion respown_rotate;

		private:


		public:

			// addComponent���ꂽ�Ƃ��ɌĂ΂��
			void awake() override {};

			// ���t���[���Ă΂��X�V����
			void update() override {
				if (transform->position.y < this_stage->y_respown_limit) {
					this_coll->reset_force();
					this_coll->linear_velocity(Vector3(0, -1, 0)); //feeze�Ɉ���������Ȃ��悤��
					transform->local_pos = respown_pos;
					transform->local_pos.y = this_stage->y_respown_pos;
					transform->local_orient = respown_rotate;
				}
			};

		};


	}
}