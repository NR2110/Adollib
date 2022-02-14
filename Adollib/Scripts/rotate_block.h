#pragma once

#include "stageparts_base.h"
#include "../Adollib/Scripts/Main/Adollib.h"

namespace Adollib {

	namespace Stage_parts {
		// 指定のtagが1であれば動くギミック
		class Rotate_block : public Stageparts_base {
		public:
			Stageparts_tagbit tag = Stageparts_tags::None; //自身の管理するbit

			Vector3 rotate_axis = Vector3(1, 0, 0); //回転軸
			float rotate_speed = 0.1f; //回転速度
			float rotate_speed_easing_pow = 1; //回転速度のeasingの値 0からrotate_speedまで、この時間で到達する easingは y = x^2


		private:

			float timer = 0;



		public:

			// addComponentされたときに呼ばれる
			void awake() override {};

			// 毎フレーム呼ばれる更新処理
			void update() override {

				// tagが立っていたらtimerを進める
				if (this_stage->tags & tag) timer += time->deltaTime() / rotate_speed_easing_pow;
				else timer -= time->deltaTime() / rotate_speed_easing_pow;
				timer = ALClamp(timer, 0, 1);

				// timerの値から回転測語を求める
				const float& speed = (timer * timer) * rotate_speed;
				if (speed != 0) {
					this_coll->angula_velocity(rotate_axis * speed);
				}
					//transform->local_orient *= quaternion_axis_angle(rotate_axis, speed);

			};

		};


	}
}

