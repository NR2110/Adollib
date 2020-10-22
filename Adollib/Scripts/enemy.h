#pragma once
#include "../Adollib/component.h"
#include "../Adollib/collider.h"

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

		Quaternion look_q(float max_speed = 1);

		void sample_move1();
		void sample_move2();
		void sample_move3();

	public:
		static enum class Enemy_type {
			pat_1,
			pat_2,
			pat_3
		};

		Transfome* player;

		Enemy_type enemy_type = Enemy_type::pat_1;
		Move_pattern moving = Move_pattern::Neet;

		Collider* collier;

		float count = 0;
		float move_pow;
		float rot_pow ;

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