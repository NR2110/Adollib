#pragma once

#include "stageparts_base.h"
#include "../Adollib/Scripts/Main/Adollib.h"

namespace Adollib {

	namespace Stage_parts {
		// 指定のtagが1であれば動くギミック
		class Tag_Andcicuit : public Stageparts_base {
		public:

			Stageparts_tagbit in_tag0 = Stageparts_tags::None; //in_tag0,in_tag1が立っているときout_tagを立てる
			Stageparts_tagbit in_tag1 = Stageparts_tags::None; //in_tag0,in_tag1が立っているときout_tagを立てる
			Stageparts_tagbit out_tag = Stageparts_tags::None; //in_tag0,in_tag1が立っているときout_tagを立てる

		private:



		public:

			// addComponentされたときに呼ばれる
			void awake() override {};

			// 毎フレーム呼ばれる更新処理
			void update() override {

				if (this_stage->tags & in_tag0 && this_stage->tags & in_tag1) this_stage->tags |= out_tag;
				else this_stage->tags &= ~out_tag;

			};

		};


	}
}

