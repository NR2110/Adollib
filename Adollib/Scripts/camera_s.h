#pragma once

#include "../Adollib/Scripts/Object/component_camera.h"
#include "../Adollib/Scripts/Physics/collider.h"

namespace Adollib
{
	class camera_s : public Component
	{
	private:
		bool follow_player = false;
		float dis = 10;

	public:
		std::shared_ptr<Transfome> player;
		Vector3 c_pos_save;

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