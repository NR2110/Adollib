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
		float fall_pow = 0.089 * 0.5 * Al_Global::elapsed_time;
		collier->liner_acceleration += vector3(0, -1, 0) * fall_pow;
	}

	// ���t���[���Aupdate()��ɌĂ΂��X�V����
	void object_fall::lateUpdate()
	{


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