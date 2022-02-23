#pragma once

// ボタン 押すとstage_baseの変数の指定のbitが立つ

#include "stageparts_base.h"
#include "../Adollib/Scripts/Main/Adollib.h"

namespace Adollib {

	namespace Stage_parts {

#pragma optimize("", off)
		class Lever : public Stageparts_base {
		public:
			Stageparts_tagbit left_tag = Stageparts_tags::None; //自身の管理するbit
			Stageparts_tagbit right_tag = Stageparts_tags::None; //自身の管理するbit

			Vector3 base_dir = Vector3(0, 1, 0);
			Vector3 hinge_dir = Vector3(1, 0, 0);

		private:

		public:

			// addComponentされたときに呼ばれる
			void awake() override {};

			// 毎フレーム呼ばれる更新処理
			void update() override {

				// bitの初期化
				this_stage->tags &= ~left_tag;
				this_stage->tags &= ~right_tag;

				// 現在のleverの向き
				Vector3 now_dir = vector3_quatrotate(base_dir, transform->orientation);

				// 回転軸
				Vector3 rot_axis = vector3_cross(now_dir, base_dir);
				this_coll->add_torque(rot_axis * fabsf(transform->orientation.radian() * 1000));

				// leverがあまり倒れていなければflagを立てずにreturn
				if (fabsf(vector3_dot(base_dir, now_dir)) > 0.96f)return;


				Vector3 roted_hinge_dir = vector3_quatrotate(hinge_dir, transform->orientation);
				// 外積によって建てるbitを変える
				if(vector3_dot(rot_axis, roted_hinge_dir) > 0)
					this_stage->tags |= left_tag;
				else
					this_stage->tags |= right_tag;


			};

		};

#pragma optimize("", on)


	}
}

