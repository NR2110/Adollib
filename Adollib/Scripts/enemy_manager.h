#pragma once
#include "../Adollib/component.h"
#include "../Adollib/rigit_body.h"

#include "enemy.h"

namespace Adollib {

	class Enemy_manager : public Component
	{
	private:
		std::vector<Enemy*> enemys;

	public:

		void add_enemy(Enemy::Enemy_type enemy_type, vector3 start_pos, quaternion start_orient, vector4 color = vector4(1, 0, 0, 1));





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