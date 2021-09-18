#pragma once

#include "stageparts_base.h"

namespace Adollib {

	namespace Stage_parts {
		// player��respown����ꏊ��ύX����

		class Respown_area : public Stageparts_base {

		public:
			Vector3 respown_pos;
			int respown_num;

		private:


		public:

			// addComponent���ꂽ�Ƃ��ɌĂ΂��
			void awake() override {};

			// ���t���[���Ă΂��X�V����
			void update() override {
				if (this_stage->player_respown_num < respown_num && this_coll->concoll_enter(Collider_tags::Human)) {
					this_stage->player_respown_pos = respown_pos;
					this_stage->player_respown_num = respown_num;
				}
			}

		};


	}
}