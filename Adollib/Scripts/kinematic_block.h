#pragma once

#pragma once

#include "../Adollib/Scripts/Object/component.h"
#include "../Adollib/Scripts/Physics/collider.h"

#include "stage_base.h"
#include "stageparts_tags.h"

namespace Adollib {

	namespace Stage_parts {
		// ���������� respown_pos�ɖ߂��K�v�����邽��component��

		class Kinematic_block : public Component {
			Stage_base* this_stage; //����parts�����݂��Ă���stage
			Collider* this_coll; //���g��collider

			Vector3 respown_pos;
			Quaternion respown_rotate;
		private:


		public:
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