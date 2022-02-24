#pragma once

#include "../Adollib/Scripts/Object/component.h"

namespace Adollib
{
	// keyboard、Gamepadの入力を統一し、ゲームで使いやすいように変換する
	class SE_manager : public Component
	{
	public:

		int player_num = 0;

	private:
		static const int timer_size = 4;
		union {
			struct
			{
				float timer[timer_size];
			};
			struct {
				float hand_pop_SE_timer;
				float jump_SE_timer;
				float landing_SE_timer;
				float step_SE_timer;
			};
		};

		int step_sign = -1;

	public:
		void hand_pop_SE();
		void jump_SE();
		void landing_SE();
		void step_SE();


	public:
		void awake() override;

		// 所属するシーンの初期化時に一度だけ呼ばれる
		void start() override;

		// 毎フレーム呼ばれる更新処理
		void update() override;

		// このスクリプトがアタッチされているGOのactiveSelfがtrueになった時呼ばれる
		void onEnable() override {};

		// このスクリプトがアタッチされているGOのactiveSelfがfalseになった時呼ばれる
		void onDisable() override {};

	};

}