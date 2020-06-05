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

		void add_enemy(Enemy::Enemy_type enemy_type,vector3 start_pos,quaternion start_orient);





	public:
		void awake();

		// ��������V�[���̏��������Ɉ�x�����Ă΂��
		void start();

		// ���t���[���Ă΂��X�V����
		void update();

		// ���t���[���Aupdate()��ɌĂ΂��X�V����
		void lateUpdate();

		// ���̃X�N���v�g���A�^�b�`����Ă���GO��activeSelf��true�ɂȂ������Ă΂��
		void onEnable();

		// ���̃X�N���v�g���A�^�b�`����Ă���GO��activeSelf��false�ɂȂ������Ă΂��
		void onDisable();

	};
}