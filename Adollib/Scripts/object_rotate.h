#pragma once

#include "../Adollib/gameobject.h"
#include "../Adollib/component.h"
#include "../Adollib/rigit_body.h"


namespace Adollib
{
	class object_rotate : public Component
	{
	public:
		Rigitbody* collier;

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