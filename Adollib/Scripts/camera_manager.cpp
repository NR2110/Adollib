
#include "../Adollib/gameobject_manager.h"

#include "camera_manager.h"
#include <DirectXMath.h>

#include "camera_s.h"
#include "../Adollib/Adollib.h"


namespace Adollib
{
	// 所属するシーンの初期化時に一度だけ呼ばれる
	void Camera_manager_s::awake() {
			set_camera(vector3(0, 30, -50), vector3(0, 0, 1));
			Al_Global::render_collider_flag = false;
	}

	void Camera_manager_s::start()
	{
		//	set_camera(vector3(0, 0, 20), vector3(0, 0, 1));

	}

	// 毎フレーム呼ばれる更新処理
	void Camera_manager_s::update()
	{
		
	}

	// このスクリプトがアタッチされているGOのactiveSelfがtrueになった時呼ばれる
	void Camera_manager_s::onEnable()
	{

	}

	// このスクリプトがアタッチされているGOのactiveSelfがfalseになった時呼ばれる
	void Camera_manager_s::onDisable()
	{

	}

}


namespace Adollib {
	void Camera_manager_s::set_camera(vector3 pos, vector3 look_pos) {
		Camera* camera = nullptr;
		camera = Gameobject_manager::create_camera("camera");
		camera->addComponent<Camera_s>();

		camera->transform->local_pos = pos;
		camera->transform->local_orient = quaternion_look_at(pos, look_pos);

		//camera->transform->local_position = pos.get_XM3();
		//camera->transform->local_rotation = quaternion_look_at(pos, vector3(0, 0, 0)).euler().get_XM3();
		//camera->transform->position = pos.get_XM3();
		//camera->transform->position = DirectX::XMFLOAT3(0, 0, 0);

	}
}