
#include "../Adollib/Scripts/Main/Adollib.h"
#include "../Adollib/Scripts/Object/gameobject_manager.h"

#include "player.h"

namespace Adollib
{
	// ��������V�[���̏��������Ɉ�x�����Ă΂��
	void Player::awake() {

	}

	void Player::start()
	{
	}

	// ���t���[���Ă΂��X�V����
	void Player::update()
	{
		Collider* Head_collider = nullptr;
		Collider* Rsholder_collider = nullptr;
		Collider* Relbow_collider = nullptr;
		Collider* Lsholder_collider = nullptr;
		Collider* Lelbow_collider = nullptr;
		Collider* Body_collider = nullptr;
		Collider* Waist_collider = nullptr;
		Collider* Rleg_collider = nullptr;
		Collider* Rfoot_collider = nullptr;
		Collider* Lleg_collider = nullptr;
		Collider* Lfoot_collider = nullptr;




	}

	// ���̃X�N���v�g���A�^�b�`����Ă���GO��activeSelf��true�ɂȂ������Ă΂��
	void Player::onEnable()
	{

	}

	// ���̃X�N���v�g���A�^�b�`����Ă���GO��activeSelf��false�ɂȂ������Ă΂��
	void Player::onDisable()
	{

	}

}
