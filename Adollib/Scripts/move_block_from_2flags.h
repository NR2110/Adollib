#pragma once

#include "stageparts_base.h"
#include "../Adollib/Scripts/Main/Adollib.h"

namespace Adollib {

	namespace Stage_parts {
		// 指定のtagが1であれば動くギミック
		class Move_block_from_2flags : public Stageparts_base {
		public:
			Stageparts_tagbit left_tag = Stageparts_tags::None; //自身の管理するbit
			Stageparts_tagbit right_tag = Stageparts_tags::None; //自身の管理するbit

			// goal位置
			Vector3 left_pos;
			Vector3 right_pos;

			// 端から端に到達する時間のinverse
			float time_pow = 1;

			// 現在の時間
			float now_time = 0.5f;

		private:
			float acc = 0;


		public:

			// addComponentされたときに呼ばれる
			void awake() override {};

			// 毎フレーム呼ばれる更新処理
			void update() override {

				//float add_time = 0;
				//if (this_stage->tags & left_tag) add_time += time->deltaTime() * time_pow * acc;
				//if (this_stage->tags & right_tag)add_time -= time->deltaTime() * time_pow * acc;

				//now_time += add_time;
				//now_time = ALClamp(now_time, 0, 1);

				//if (add_time == 0) acc = 0;
				//acc += time->deltaTime() * 2;
				//acc = ALClamp(acc, 0, 1);


				//Vector3 offset = left_pos + (right_pos - left_pos) * now_time - transform->local_pos;
				//transform->local_pos += offset;

				const Vector3 left_to_right_dir = (right_pos - left_pos).unit_vect();

				// 座標調整 ルートを外れていた場合 ルート上に強制的に戻す
				transform->local_pos = left_pos + left_to_right_dir * vector3_dot(left_to_right_dir, transform->local_pos - left_pos);
				// 座標調整 はみ出ていたら戻す
				if (vector3_dot(left_pos - transform->local_pos, right_pos - transform->local_pos) > 0) {
					if ((left_pos - transform->local_pos).norm() < (right_pos - transform->local_pos).norm())
						transform->local_pos = left_pos;
					else
						transform->local_pos = right_pos;
				}

				// 速度の方向
				float sign = 0;
				if (this_stage->tags & left_tag) sign -= 1;
				if (this_stage->tags & right_tag)sign += 1;

				// 目標地点にたどり着いていれば 速度を0にしてreturn
				if (
					(sign == -1 && transform->local_pos == left_pos) ||
					(sign == +1 && transform->local_pos == right_pos)
					) {
					this_coll->linear_velocity(Vector3(0));
					acc = 0;
				}

				// 適当なeasing用
				if (sign == 0)acc -= time->deltaTime();
				else acc += time->deltaTime() * 2;
				acc = ALClamp(acc, 0, 1);

				Vector3 vec = left_to_right_dir * sign * acc;

				this_coll->linear_velocity(vec * time_pow * 60);

			};

		};


	}
}

