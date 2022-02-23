#pragma once

#include "stageparts_base.h"
#include "../Adollib/Scripts/Main/Adollib.h"

#include "../Adollib/Scripts/Object/gameobject.h"
#include "../Adollib/Scripts/Renderer/renderer_base.h"
#include "../Adollib/Scripts/Main/Adollib.h"

namespace Adollib {

	namespace Stage_parts {
		// 指定のtagが1であれば動くギミック
		class Change_color_reinbow : public Stageparts_base {
		public:
			Stageparts_tagbit tag = Stageparts_tags::None; //自身の管理するbit

			float timer = 0;

			Vector3 base_color = Vector3(1, 1, 1);

		private:


		public:

			// addComponentされたときに呼ばれる
			void awake() override {};

			// 毎フレーム呼ばれる更新処理
			void update() override {
				timer += time->deltaTime();

				// tagが立っていたらtimerを進める
				//if (this_stage->tags & tag)	gameobject->renderer->color = Vector4(Al_Global::get_gaming(timer, 2, 196 / 255.0f, 1), 1);
				//else  gameobject->renderer->color = Vector4(base_color, 1);


				Vector4 color = Vector4(base_color, 1);
				if (this_stage->tags & tag) color = Vector4(Al_Global::get_gaming(timer, 2, 196 / 255.0f, 1), 1);

				if (gameobject->renderer != nullptr)gameobject->renderer->color = color;
				for (auto& child : *gameobject->children())if (child->renderer != nullptr)child->renderer->color = color;

			};

		};


	}
}

