#pragma once
#include "../Adollib/component.h"
#include "../Adollib/rigit_body.h"

namespace Adollib
{

	class Enemy : public Component
	{
	private:
		static enum class Move_pattern {
			init,
			choice,
			wait_init,
			wait_update,
			strate_init,
			strate_update,
			tarn_init,
			tarn_update,
			move_to_p_init,
			move_tp_p_update,
			Neet
		};

		quaternion look_q();

		void sample_move1();
		void sample_move2();
		void sample_move3();

	public:
		static enum class Enemy_type {
			normal
		};

		Transfome* player;

		Enemy_type enemy_type = Enemy_type::normal;
		Move_pattern moving = Move_pattern::Neet;

		Rigitbody* collier;

		float count = 0;
		float move_pow;
		float rot_pow ;

	public:
		void awake();

		// 所属するシーンの初期化時に一度だけ呼ばれる
		void start();

		// 毎フレーム呼ばれる更新処理
		void update();

		// 毎フレーム、update()後に呼ばれる更新処理
		void lateUpdate();

		// このスクリプトがアタッチされているGOのactiveSelfがtrueになった時呼ばれる
		void onEnable();

		// このスクリプトがアタッチされているGOのactiveSelfがfalseになった時呼ばれる
		void onDisable();

	};

}