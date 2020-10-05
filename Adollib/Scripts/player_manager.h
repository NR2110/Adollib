#pragma once
#include "../Adollib/component.h"
#include "../Adollib/collider.h"
#include "player_base.h"

namespace Adollib {

	class Player_manager_s : public Component {
	private:
		std::list<Player_base*> players;
		Player_base* player_leader;

		float player_speed_mulnum;


	private:
		void add_player(vector3 pos, float r, float max_speed, float acc,vector4 color = vector4(0, 0, 1, 1));
		void add_player_leader(vector3 pos, float r, float max_speed, vector4 color = vector4(1, 0.2, 0.2, 1));



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