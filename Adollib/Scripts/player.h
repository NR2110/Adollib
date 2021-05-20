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