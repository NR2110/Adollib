#pragma once

#include "../Adollib/Scripts/Object/gameobject_manager.h"
#include "../Adollib/Scripts/Object/component.h"

namespace Adollib {

	class Tutrial_base;


	class Tutrial_manager : public Component {

	private:
		std::vector<std::shared_ptr<Tutrial_base>> tutrials;


	public:
		float tutrial_ui_x = -63;

	private:



	public:

		void awake();

		// 所属するシーンの初期化時に一度だけ呼ばれる
		void start();

		// 毎フレーム呼ばれる更新処理
		void update();

		// このスクリプトがアタッチされているGOのactiveSelfがtrueになった時呼ばれる
		void onEnable() {};

		// このスクリプトがアタッチされているGOのactiveSelfがfalseになった時呼ばれる
		void onDisable() {};


	};


}