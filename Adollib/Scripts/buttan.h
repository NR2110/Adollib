#pragma once

// ボタン 押すとstage_baseの変数の指定のbitが1になる

#include "stageparts_base.h"

namespace Adollib{

	namespace Stage_parts {

		class Buttan : public Stageparts_base {

			Stageparts_tagbit flag = Stageparts_tags::None; //自身の管理するbit

		public:

			// addComponentされたときに呼ばれる
			void awake() override {};

			// 毎フレーム呼ばれる更新処理
			void update() override {
				//playerか動く物体に触れていたらbitを立てる
				if (this_coll->concoll_enter(Collider_tags::Human | Collider_tags::Kinematic_Stage)) {
					this_stage->tags |= flag;
				}
				else this_stage->tags &= ~flag;

			};

		};


	}
}

