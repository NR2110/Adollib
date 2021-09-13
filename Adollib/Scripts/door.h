#pragma once

#include "stageparts_base.h"

namespace Adollib {

	namespace Stage_parts{
		// 指定のtagが1であれば動くギミック
		class Door : public Stageparts_base {

			Stageparts_tagbit flag = Stageparts_tags::None; //自身の管理するbit

			Vector3 dir = Vector3(0, 1, 0); //動く向き
			float move_dis = 10; //動く距離

		private:

		public:

			// addComponentされたときに呼ばれる
			void awake() override {};

			// 毎フレーム呼ばれる更新処理
			void update() override{

				//指定のbitが立っていたら力を加える
				if (this_stage->tags & flag) {





				}
			};

		};


	}
}

