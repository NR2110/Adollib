#include "../Adollib/gameobject_manager.h"
#include "../Adollib/Adollib.h"
#include "object_fall.h"

namespace Adollib
{
	// 所属するシーンの初期化時に一度だけ呼ばれる
	void object_fall::awake() {

	}

	void object_fall::start()
	{

	}

	// 毎フレーム呼ばれる更新処理
	void object_fall::update()
	{
		float fall_pow = 8.9;
		collier->liner_acceleration += vector3(0, -1, 0) * fall_pow;
	}

	// このスクリプトがアタッチされているGOのactiveSelfがtrueになった時呼ばれる
	void object_fall::onEnable()
	{

	}

	// このスクリプトがアタッチされているGOのactiveSelfがfalseになった時呼ばれる
	void object_fall::onDisable()
	{

	}

}
