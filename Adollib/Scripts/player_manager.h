#pragma once
#include "../Adollib/component.h"
#include "../Adollib/collider.h"
#include "player_base.h"

namespace Adollib {

	class Player_manager_s : public Component {
		std::list<Player_base*> players;
		Player_base* player_leader;

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