#pragma once

#include "stageparts_base.h"
#include "../Adollib/Scripts/Main/Adollib.h"

namespace Adollib {

	namespace Stage_parts {
		// 指定のtagが1であれば動くギミック
		class Touch_moveable : public Stageparts_base {
		public:
			Collider_tagbit tag = 0; //このtagのオブジェクトに接触したら collidersのmoveableをtrueにする
			std::vector<Collider*> judge_colliders; //このcolliderにtagのcolliderが接触したら
			std::vector<Collider*> move_colliders;  //このcolliderのmoveableをtrueにする


		private:



		public:

			// addComponentされたときに呼ばれる
			void awake() override {};

			// 毎フレーム呼ばれる更新処理
			void update() override {

				bool is_contact = false;
				for (auto& coll : judge_colliders) {
					if (coll->concoll_enter(tag)) is_contact = true;
					if (is_contact)break;
				}

				if (is_contact) {
					for (auto& coll : move_colliders)coll->physics_data.is_moveable = true;
				}


			};

		};


	}
}

