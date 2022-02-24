#pragma once

#include "../Adollib/Scripts/Object/component.h"

namespace Adollib
{
	// keyboard�AGamepad�̓��͂𓝈ꂵ�A�Q�[���Ŏg���₷���悤�ɕϊ�����
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

		// ��������V�[���̏��������Ɉ�x�����Ă΂��
		void start() override;

		// ���t���[���Ă΂��X�V����
		void update() override;

		// ���̃X�N���v�g���A�^�b�`����Ă���GO��activeSelf��true�ɂȂ������Ă΂��
		void onEnable() override {};

		// ���̃X�N���v�g���A�^�b�`����Ă���GO��activeSelf��false�ɂȂ������Ă΂��
		void onDisable() override {};

	};

}