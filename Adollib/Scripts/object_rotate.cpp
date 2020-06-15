#include "../Adollib/component.h"
#include "../Adollib/rigit_body.h"

#include "object_rotate.h"


namespace Adollib
{
	// ��������V�[���̏��������Ɉ�x�����Ă΂��
	void object_rotate::awake() {

	}

	void object_rotate::start()
	{

	}

	// ���t���[���Ă΂��X�V����
	void object_rotate::update()
	{
		vector3 rotate_vec;
		if (input->getKeyState(Key::Up))rotate_vec += vector3(-1, 0, 0);
		if (input->getKeyState(Key::Down))rotate_vec += vector3(+1, 0, 0);
		if (input->getKeyState(Key::Left))rotate_vec += vector3(0, -1, 0);
		if (input->getKeyState(Key::Right))rotate_vec += vector3(0, +1, 0);

		//collier->world_orientation *= quaternion_from_euler(rotate_vec);
		gameobject->transform->local_orient *= quaternion_from_euler(rotate_vec);

		if (input->getKeyState(Key::RightShift)) {
			//collier->world_orientation = quaternion_identity();
			gameobject->transform->local_orient = quaternion_identity();
		}
	}

	// ���̃X�N���v�g���A�^�b�`����Ă���GO��activeSelf��true�ɂȂ������Ă΂��
	void object_rotate::onEnable()
	{

	}

	// ���̃X�N���v�g���A�^�b�`����Ă���GO��activeSelf��false�ɂȂ������Ă΂��
	void object_rotate::onDisable()
	{

	}

}
