#include "../Adollib/gameobject_manager.h"
#include "../Adollib/Adollib.h"
#include "object_fall.h"

namespace Adollib
{
	// ��������V�[���̏��������Ɉ�x�����Ă΂��
	void object_fall::awake() {

	}

	void object_fall::start()
	{

	}

	// ���t���[���Ă΂��X�V����
	void object_fall::update()
	{
		float fall_pow = 8.9;
		collier->liner_acceleration += vector3(0, -1, 0) * fall_pow;

		//static vector3 speed = vector3{ 0 };
		//speed += vector3(0, -1, 0) * fall_pow * 0.016;
		//collier->local_position += speed;
	}

	// ���̃X�N���v�g���A�^�b�`����Ă���GO��activeSelf��true�ɂȂ������Ă΂��
	void object_fall::onEnable()
	{

	}

	// ���̃X�N���v�g���A�^�b�`����Ă���GO��activeSelf��false�ɂȂ������Ă΂��
	void object_fall::onDisable()
	{

	}

}
