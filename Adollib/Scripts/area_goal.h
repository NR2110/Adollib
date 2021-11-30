#pragma once

#include "stageparts_base.h"
#include "../Adollib/Scripts/Main/Adollib.h"

#include "stage_types.h"

namespace Adollib {
	class Stage_manager;

	namespace Stage_parts {

		// 指定のtagが1であれば動くギミック
		class Area_goal : public Stageparts_base {
		public:
			Stage_types next_stage = Stage_types::none;

		private:
			Stage_manager* stage_manager = nullptr; //stage_managerへのポインタ


		public:

			// addComponentされたときに呼ばれる
			void awake() override;

			// 毎フレーム呼ばれる更新処理
			void update() override;

		};


	}
}

