#pragma once

#include "../Adollib/Scripts/Object/gameobject_manager.h"
#include "../Adollib/Scripts/Object/component.h"

namespace Adollib {

	class Tutrial_base;


	class Tutrial_manager : public Component {

	private:
		std::vector<std::shared_ptr<Tutrial_base>> tutrials;


	public:
		float tutrial_ui_x = -63;

	private:



	public:

		void awake();

		// ��������V�[���̏��������Ɉ�x�����Ă΂��
		void start();

		// ���t���[���Ă΂��X�V����
		void update();

		// ���̃X�N���v�g���A�^�b�`����Ă���GO��activeSelf��true�ɂȂ������Ă΂��
		void onEnable() {};

		// ���̃X�N���v�g���A�^�b�`����Ă���GO��activeSelf��false�ɂȂ������Ă΂��
		void onDisable() {};


	};


}