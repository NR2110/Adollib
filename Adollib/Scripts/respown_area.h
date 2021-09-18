#pragma once

#include "stageparts_base.h"

namespace Adollib {

	namespace Stage_parts {
		// playerのrespownする場所を変更する

		class Respown_area : public Stageparts_base {

		public:
			Vector3 respown_pos;
			int respown_num;

		private:


		public:

			// addComponentされたときに呼ばれる
			void awake() override {};

			// 毎フレーム呼ばれる更新処理
			void update() override {
				if (this_stage->player_respown_num < respown_num && this_coll->concoll_enter(Collider_tags::Human)) {
					this_stage->player_respown_pos = respown_pos;
					this_stage->player_respown_num = respown_num;
				}
			}

		};


	}
}