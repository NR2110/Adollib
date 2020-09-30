#pragma once
#include "../Adollib/component.h"
#include "../Adollib/collider.h"
#include "player_base.h"

namespace Adollib {

	class Player_manager_s : public Component {
		std::list<Player_base*> players;
		Player_base* player_leader;

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