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

			// triggerとして使用するか
			bool is_use_trigger = false;
		private:
			float timer = 0; //離れても少しの間はONになるようにするため(ちかちかさせたくない)
			float trigger_timer = 0;

			bool is_trigger_pushed = false;

		public:

			// addComponentされたときに呼ばれる
			void awake() override {};

			// 毎フレーム呼ばれる更新処理
			void update() override {
				constexpr float color_cheng_time = 5.5f;

				if (is_use_trigger) {

					// 前のフレーム押されていないとき
					if (trigger_timer < 0){
						// 押されたら
						if (this_coll->concoll_enter(Collider_tags::Human | Collider_tags::Kinematic_Stage)) is_trigger_pushed = !is_trigger_pushed; //切り替える
					}

					if (this_coll->concoll_enter(Collider_tags::Human | Collider_tags::Kinematic_Stage))trigger_timer = 1;
					else trigger_timer -= Al_Global::second_per_frame;

					//
					if (is_trigger_pushed)  timer = 1;
					else timer = -1;

				}
				else {
					if (this_coll->concoll_enter(Collider_tags::Human | Collider_tags::Kinematic_Stage)) timer = 0.25f;
					else timer -= Al_Global::second_per_frame;
				}

				// 触れているときbuttonを下げる
				if (this_coll->concoll_enter(Collider_tags::Human | Collider_tags::Kinematic_Stage)) {
					// scaleの調整
					transform->local_scale.y = ALClamp(transform->local_scale.y - 0.2f * Al_Global::second_per_frame, base_scale_y, base_scale_y + 0.2f);
				}
				else {
					// scaleの調整
					transform->local_scale.y = ALClamp(transform->local_scale.y + 0.2f * Al_Global::second_per_frame, base_scale_y, base_scale_y + 0.2f);
				}

				// playerか動く物体に触れていたら
				if (timer > 0) {

					// bitを立てる
					this_stage->tags |= tag;

					// 色の変更
					gameobject->material->color.x = ALClamp(gameobject->material->color.x + (base_material_color.y - base_material_color.x) * color_cheng_time * Al_Global::second_per_frame, base_material_color.y, base_material_color.x);
					gameobject->material->color.y = ALClamp(gameobject->material->color.y + (base_material_color.x - base_material_color.y) * color_cheng_time * Al_Global::second_per_frame, base_material_color.y, base_material_color.x);

				}
				else {
					// bitを立てる
					this_stage->tags &= ~tag;

					// 色の変更
					gameobject->material->color.x = ALClamp(gameobject->material->color.x - (base_material_color.y - base_material_color.x) * color_cheng_time * Al_Global::second_per_frame, base_material_color.y, base_material_color.x);
					gameobject->material->color.y = ALClamp(gameobject->material->color.y - (base_material_color.x - base_material_color.y) * color_cheng_time * Al_Global::second_per_frame, base_material_color.y, base_material_color.x);
				}

			};

		};


	}
}

