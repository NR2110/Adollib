#pragma once

#include "../Adollib/Scripts/Object/component.h"

#include "stageparts_tags.h"

namespace Adollib {

	class Stage_base : public Component {
	public:
		Stage_parts::Stageparts_tagbit tags; //このstageが持つtag

		float y_respown_value = 0; // Yがこれ以下だとrespownされる

	public:
		void awake();

		// 所属するシーンの初期化時に一度だけ呼ばれる
		void start();

		// 毎フレーム呼ばれる更新処理
		void update();

		// このスクリプトがアタッチされているGOのactiveSelfがtrueになった時呼ばれる
		void onEnable();

		// このスクリプトがアタッチされているGOのactiveSelfがfalseになった時呼ばれる
		void onDisable();
	};

}