#pragma once

#include "stageparts_base.h"

namespace Adollib {

	namespace Stage_parts {
		// 落下した際 respown_posに戻す必要があるためcomponentに

		class Kinematic_block : public Stageparts_base {

		public:
			Vector3 respown_pos;
			Quaternion respown_rotate;

		private:


		public:

			// addComponentされたときに呼ばれる
			void awake() override {};

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