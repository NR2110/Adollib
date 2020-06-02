#include "gameobject_manager.h"

#include "object_fall.h"
#include "Adollib.h"

using namespace Adollib;

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
		float fall_pow = 0.089 * 0.5 *  Al_Global::elapsed_time;
		collier->linear_velocity += vector3(0, -1, 0) * fall_pow;
	}

	// 毎フレーム、update()後に呼ばれる更新処理
	void object_fall::lateUpdate()
	{


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
