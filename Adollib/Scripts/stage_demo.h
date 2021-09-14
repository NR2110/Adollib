#pragma once
#include "stage_base.h"

namespace Adollib {

	class Stage_demo : public Stage_base {


	private:

		Gameobject* set_tree(Vector3 pos, Vector3 scale, Vector3 rotate);
		Gameobject* set_buttan(Vector3 pos, Vector3 scale, Vector3 rotate, Vector3 color = Vector3(1, 1, 1));
		Gameobject* set_door(Vector3 pos, Vector3 scale, Vector3 rotate, Vector3 color = Vector3(1, 1, 1));
		Gameobject* set_rope(Vector3 pos, Vector3 scale, Vector3 rotate, Vector3 color = Vector3(1, 1, 1));

	public:
		void awake() override;

		// ��������V�[���̏��������Ɉ�x�����Ă΂��
		void start()override;

		// ���t���[���Ă΂��X�V����
		void update()override;

		void Update_hierarchy() override {};

		// ���̃X�N���v�g���A�^�b�`����Ă���GO��activeSelf��true�ɂȂ������Ă΂��
		void onEnable()override;

		// ���̃X�N���v�g���A�^�b�`����Ă���GO��activeSelf��false�ɂȂ������Ă΂��
		void onDisable()override;
	};
}