#pragma once
#include "../Adollib/Scripts/Object/component.h"
#include "../Adollib/Scripts/Physics/collider.h"

namespace Adollib
{

	class Player : public Component
	{
	private:
		float walking_speed = 3 * 5;
		float running_speed = 7 * 5;
		float max_move_y_speed = 8;
		float turn_speed = 0.4f;

		float linear_drag_xz = 0.4f;
		float move_speed = 100;
		float jump_power = 7.5f;

	private:
		bool is_running = false;

	private:

	public:
		Collider* collider;
		Camera* camera;

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