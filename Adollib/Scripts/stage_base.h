#pragma once

#include "../Adollib/Scripts/Object/component.h"

#include "stageparts_tags.h"

namespace Adollib {

	class Stage_base : public Component {
	public:
		Stage_parts::Stageparts_tagbit tags; //����stage������tag

		float y_respown_value = 0; // Y������ȉ�����respown�����

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