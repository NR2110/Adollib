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
				if (transform->position.y < this_stage->y_respown_value) {
					this_coll->reset_force();
					transform->local_pos = respown_pos;
					transform->local_orient = respown_rotate;
				}
			};

		};


	}
}