
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
		Gameobject* camera_go = Gameobject_manager::create("camera");

		camera_go->addComponent<Camera_component>();
		camera_go->addComponent<camera_s>();

		camera_go->transform->local_pos = pos;
		camera_go->transform->local_orient = quaternion_look_at(pos, look_pos);
	}
}