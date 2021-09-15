#pragma once

// ボタン 押すとstage_baseの変数の指定のbitが立つ

#include "stageparts_base.h"
#include "../Adollib/Scripts/Main/Adollib.h"

namespace Adollib{

	namespace Stage_parts {

		class Buttan : public Stageparts_base {
		public:
			Stageparts_tagbit tag = Stageparts_tags::None; //自身の管理するbit

			float base_scale_y = 1;
			Vector3 base_material_color = Vector3(0.5f);

		private:
			float timer = 0; //離れても少しの間はONになるようにするため(ちかちかさせたくない)
		public:

			// addComponentされたときに呼ばれる
			void awake() override {};

			// 毎フレーム呼ばれる更新処理
			void update() override {
				constexpr float color_cheng_time = 5.5f;

				if (this_coll->concoll_enter(Collider_tags::Human | Collider_tags::Kinematic_Stage)) timer = 0.25f;
				else timer -= Al_Global::second_per_frame;

				//playerか動く物体に触れていたら
				if (timer > 0) {

					//bitを立てる
					this_stage->tags |= tag;

					//色の変更
					gameobject->material->color.x = ALClamp(gameobject->material->color.x + (base_material_color.y - base_material_color.x) * color_cheng_time * Al_Global::second_per_frame, base_material_color.y, base_material_color.x);
					gameobject->material->color.y = ALClamp(gameobject->material->color.y + (base_material_color.x - base_material_color.y) * color_cheng_time * Al_Global::second_per_frame, base_material_color.y, base_material_color.x);

					//scaleの調整
					transform->local_scale.y = ALClamp(transform->local_scale.y - 0.2f * Al_Global::second_per_frame, base_scale_y, base_scale_y + 0.2f);

				}
				else {
					//bitを立てる
					this_stage->tags &= ~tag;

					//色の変更
					gameobject->material->color.x = ALClamp(gameobject->material->color.x - (base_material_color.y - base_material_color.x) * color_cheng_time * Al_Global::second_per_frame, base_material_color.y, base_material_color.x);
					gameobject->material->color.y = ALClamp(gameobject->material->color.y - (base_material_color.x - base_material_color.y) * color_cheng_time * Al_Global::second_per_frame, base_material_color.y, base_material_color.x);

					//scaleの調整
					transform->local_scale.y = ALClamp(transform->local_scale.y + 0.2f * Al_Global::second_per_frame, base_scale_y, base_scale_y + 0.2f);
				}

			};

		};


	}
}

