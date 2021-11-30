#pragma once

#include "stageparts_base.h"
#include "../Adollib/Scripts/Main/Adollib.h"

namespace Adollib {

	namespace Stage_parts {

		// 指定のtagが1であれば動くギミック
		class Area_respown_set : public Stageparts_base {
		public:
			int respown_num = -1;
			Vector3 respown_position = Vector3(0);

		public:

			// addComponentされたときに呼ばれる
			void awake() override;

			// 毎フレーム呼ばれる更新処理
			void update() override;

		};


	}
}

