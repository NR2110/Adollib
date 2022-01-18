#pragma once

#include "../Adollib/Scripts/Object/component_camera.h"
#include "../Adollib/Scripts/Physics/collider.h"

namespace Adollib
{

	class Title_stage : public Component
	{
	private:
		Vector4 wall_color ;
		Vector4 floar_color;
		Vector4 stair_color;

	public:

		void awake() override;

		// ��������V�[���̏��������Ɉ�x�����Ă΂��
		void start() override;

		// ���t���[���Ă΂��X�V����
		void update() override;

		// ���̃X�N���v�g���A�^�b�`����Ă���GO��activeSelf��true�ɂȂ������Ă΂��
		void onEnable() override;

		// ���̃X�N���v�g���A�^�b�`����Ă���GO��activeSelf��false�ɂȂ������Ă΂��
		void onDisable() override;

	};

}