#pragma once

#pragma once

#include "../Adollib/Scripts/Object/component.h"
#include "../Adollib/Scripts/Physics/collider.h"

#include "stage_base.h"
#include "stageparts_tags.h"

namespace Adollib {

	namespace Stage_parts {
		// 落下した際 respown_posに戻す必要があるためcomponentに

		class Kinematic_block : public Component {
			Stage_base* this_stage; //このpartsが存在しているstage
			Collider* this_coll; //自身のcollider

			Vector3 respown_pos;
			Quaternion respown_rotate;
		private:


		public:
			// 毎フレーム呼ばれる更新処理
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