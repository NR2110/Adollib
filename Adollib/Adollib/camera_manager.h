#pragma once

#include "component.h"
#include "rigit_body.h"

namespace Adollib
{
	class camera_manager_s : public Component
	{
	public:
		void set_camera(vector3 pos, vector3 look_pos);

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