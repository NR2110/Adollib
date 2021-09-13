#pragma once

#include "../Adollib/Scripts/Object/component.h"
#include "../Adollib/Scripts/Physics/collider.h"

#include "stage_base.h"
#include "stageparts_tags.h"

namespace Adollib {

	namespace Stage_parts {

		class Stageparts_base : public Component {
		public:
			Stage_base* this_stage = nullptr;      //このpartsが存在しているstage
			Collider* this_coll = nullptr;         //自身のcollider

		public:
			// addComponentされたときに呼ばれる
			virtual void awake() {};

			// 所属するシーンの初期化時に一度だけ呼ばれる
			virtual void start() {};

			// Hierarchyの表示(Imguiの関数 Imgui::begin,Imgui::endはいらない)
			virtual void Update_hierarchy() {};

			// 毎フレーム呼ばれる更新処理
			virtual void update() {};

			// このスクリプトがアタッチされているGOのactiveSelfがtrueになった時呼ばれる(GO生成時にスクリプトが有効な時も呼ばれる)
			virtual void onEnable() {};

			// このスクリプトがアタッチされているGOのactiveSelfがfalseになった時呼ばれる
			virtual void onDisable() {};

			// removeComponent()、clear()時に呼ぶ
			virtual void finalize() {};
		};

	}

}
