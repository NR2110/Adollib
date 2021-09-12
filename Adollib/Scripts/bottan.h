#pragma once

// ボタン 押すとstage_baseの変数の指定のbitが1になる

#include "../Adollib/Scripts/Object/component.h"
#include "../Adollib/Scripts/Physics/collider.h"

#include "stage_base.h"
#include "stageparts_tags.h"

namespace Adollib{

	namespace Stage_parts {
		// 基本的に1つのbitを複数のボタンで管理しない。 ボタンのuppdateでbitを0/1を切り替えるため
		// (componentを使わなければbottanupdateを先に行うことでこの問題は解決するが 32個あればとりあえず足りそうだから放置)

		class Bottan : public Component {
			Stage_base* tihs_stage; //このpartsが存在しているstage
			Collider* this_coll; //自身のcollider

			Stageparts_tagbit flag = Stageparts_tags::None; //自身の管理するbit

		public:
			// 毎フレーム呼ばれる更新処理
			void update() override {
				//playerか動く物体に触れていたらbitを立てる
				if (this_coll->concoll_enter(Collider_tags::Human | Collider_tags::Kinematic_Stage)) {
					tihs_stage->tags |= flag;
				}
				else tihs_stage->tags &= ~flag;

			};

		};


	}
}

