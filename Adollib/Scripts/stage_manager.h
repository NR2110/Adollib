#pragma once
#include "../Adollib/component.h"
#include "../Adollib/collider.h"
#include "stage_base.h"

namespace Adollib {

	class Stage_manager_s : public Component {
	public:
		std::list<Stage_base*> stages;
	private:
		vector3 *camera_pos;
		quaternion *camera_orient;

	private:
		void set_cube(vector3 pos, vector3 size, vector3 rotate, vector4 color = vector4(1, 1, 1, 1));



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