#pragma once

#pragma once

#include "../Adollib/Scripts/Object/component.h"
#include "../Adollib/Scripts/Physics/collider.h"

#include "stage_base.h"
#include "stageparts_tags.h"

namespace Adollib {

	namespace Stage_parts {
		// 指定のtagが1であれば動くギミック
		class Door : public Component {
			Stage_base* tihs_stage; //このpartsが存在しているstage
			Collider* this_coll; //自身のcollider

			Stageparts_tagbit flag = Stageparts_tags::None; //自身の管理するbit

			Vector3 dir = Vector3(0, 1, 0); //動く向き
			float move_dis = 10; //動く距離
		private:

		public:
			// 毎フレーム呼ばれる更新処理
			void update() override{

				//指定のbitが立っていたら力を加える
				if (tihs_stage->tags & flag) {





				}
			};

		};


	}
}

