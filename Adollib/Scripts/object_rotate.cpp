#include "../Adollib/component.h"
#include "../Adollib/collider.h"

#include "object_rotate.h"


namespace Adollib
{
	// 所属するシーンの初期化時に一度だけ呼ばれる
	void object_rotate::awake() {

	}

	void object_rotate::start()
	{

	}

	// 毎フレーム呼ばれる更新処理
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

	// このスクリプトがアタッチされているGOのactiveSelfがtrueになった時呼ばれる
	void object_rotate::onEnable()
	{

	}

	// このスクリプトがアタッチされているGOのactiveSelfがfalseになった時呼ばれる
	void object_rotate::onDisable()
	{

	}

}
