#pragma once
#include "../Adollib/component.h"
#include "../Adollib/collider.h"

#include "enemy.h"

namespace Adollib {

	class Enemy_manager : public Component
	{
	private:
		std::vector<Enemy*> enemys;

	public:

		void add_enemy(Enemy::Enemy_type enemy_type, Vector3 start_pos, Quaternion start_orient, Vector4 color = Vector4(1, 0, 0, 1));





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