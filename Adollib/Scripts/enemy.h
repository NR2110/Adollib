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

		quaternion look_q(float max_speed = 1);

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

		Rigitbody* collier;

		float count = 0;
		float move_pow;
		float rot_pow ;

	public:
		void awake();

		// ��������V�[���̏��������Ɉ�x�����Ă΂��
		void start();

		// ���t���[���Ă΂��X�V����
		void update();

		// ���̃X�N���v�g���A�^�b�`����Ă���GO��activeSelf��true�ɂȂ������Ă΂��
		void onEnable();

		// ���̃X�N���v�g���A�^�b�`����Ă���GO��activeSelf��false�ɂȂ������Ă΂��
		void onDisable();

	};

}