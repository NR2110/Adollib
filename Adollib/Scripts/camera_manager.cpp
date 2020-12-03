
#include "../Adollib/Scripts/Object/gameobject_manager.h"

#include "camera_manager.h"
#include <DirectXMath.h>

#include "camera_s.h"
#include "../Adollib/Scripts/Main/Adollib.h"


namespace Adollib
{
	// 所属するシーンの初期化時に一度だけ呼ばれる
	void camera_manager_s::awake() {
			set_camera(Vector3(0, 30, -50), Vector3(0, 0, 1));
	}

	void camera_manager_s::start()
	{

	}

	// 毎フレーム呼ばれる更新処理
	void camera_manager_s::update()
	{

	}

	// このスクリプトがアタッチされているGOのactiveSelfがtrueになった時呼ばれる
	void camera_manager_s::onEnable()
	{

	}

	// このスクリプトがアタッチされているGOのactiveSelfがfalseになった時呼ばれる
	void camera_manager_s::onDisable()
	{

	}

}


namespace Adollib {
	void camera_manager_s::set_camera(Vector3 pos, Vector3 look_pos) {
		Camera* camera = nullptr;
		camera = Gameobject_manager::create_camera("camera");
		camera->addComponent<camera_s>();

		camera->transform->local_pos = pos;
		camera->transform->local_orient = quaternion_look_at(pos, look_pos);
	}
}