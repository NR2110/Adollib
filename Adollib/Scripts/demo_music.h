#pragma once

#include "../Adollib/Scripts/Object/gameobject.h"
#include "../Adollib/Scripts/Object/component.h"

namespace Adollib {

	class Demo_music : public Component {

	private:



	private:


	public:
		void awake() override;

		// 所属するシーンの初期化時に一度だけ呼ばれる
		void start() override {};

		// 毎フレーム呼ばれる更新処理
		void update() override;

		// このスクリプトがアタッチされているGOのactiveSelfがtrueになった時呼ばれる
		void onEnable() override {};

		// このスクリプトがアタッチされているGOのactiveSelfがfalseになった時呼ばれる
		void onDisable() override {};
	};


}