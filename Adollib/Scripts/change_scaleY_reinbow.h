#pragma once

#include "stageparts_base.h"
#include "../Adollib/Scripts/Main/Adollib.h"

#include "../Adollib/Scripts/Object/gameobject.h"
#include "../Adollib/Scripts/Renderer/renderer_base.h"
#include "../Adollib/Scripts/Main/Adollib.h"

namespace Adollib {

	namespace Stage_parts {
		// 指定のtagが1であれば動くギミック
		class Change_scaleY_reinbow : public Stageparts_base {
		public:
			Stageparts_tagbit tag = Stageparts_tags::None; //自身の管理するbit

			float timer = 0;

			float base_scaleY = 1;
			float base_posY = 1;

		private:


		public:

			// addComponentされたときに呼ばれる
			void awake() override {};

			// 毎フレーム呼ばれる更新処理
			void update() override {
				timer += time->deltaTime() * 3;

				// tagが立っていたらtimerを進める
				//if (this_stage->tags & tag)	gameobject->renderer->color = Vector4(Al_Global::get_gaming(timer, 2, 196 / 255.0f, 1), 1);
				//else  gameobject->renderer->color = Vector4(base_color, 1);

				if (this_coll->physics_data.is_moveable == true)return;

				float scaleY = base_scaleY;
				if (this_stage->tags & tag) {
					float pow = (cosf(timer) * 0.5f + 0.3f) * 0.4f + 1;
					scaleY = base_scaleY * pow;
				}

				Vector3 scale = gameobject->transform->local_scale;
				scale.y = scaleY;
				gameobject->transform->local_scale = scale;

			};

		};


	}
}

