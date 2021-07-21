
#include "../Adollib/Scripts/Main/Adollib.h"
#include "../Adollib/Scripts/Object/gameobject_manager.h"

#include "player.h"

namespace Adollib
{
	// 所属するシーンの初期化時に一度だけ呼ばれる
	void Player::awake() {

	}

	void Player::start()
	{
	}

	// 毎フレーム呼ばれる更新処理
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

	// このスクリプトがアタッチされているGOのactiveSelfがtrueになった時呼ばれる
	void Player::onEnable()
	{

	}

	// このスクリプトがアタッチされているGOのactiveSelfがfalseになった時呼ばれる
	void Player::onDisable()
	{

	}

}
