

#include "player_manager.h"
#include "../Adollib/Scripts/Object/gameobject_manager.h"
#include "../Adollib/Scripts/Scene/scene_manager.h"

#include "../Adollib/Scripts/Math/closest_func.h"
#include "../Adollib/Scripts/Imgui/imgui_all.h"
#include "../Adollib/Scripts/Physics/ALP__physics_manager.h"

#include "../Adollib/Scripts/Physics/ray.h"

#include "../Adollib/Scripts/Imgui/debug.h"

#include "player.h"

namespace Adollib
{
	// 所属するシーンの初期化時に一度だけ呼ばれる
	void Player_manager::awake() {
		//auto player_go = Gameobject_manager::create(std::string("player"));
		//auto C = player_go->addComponent<Capsule>();
		//player = player_go->addComponent<Player>();
		//player->collider = C;

	}

	void Player_manager::start()
	{
		camera = Gameobject_manager::find_camera("camera");
		player->camera = camera;
	}

	// 毎フレーム呼ばれる更新処理
	void Player_manager::update()
	{

	}




	// このスクリプトがアタッチされているGOのactiveSelfがtrueになった時呼ばれる
	void Player_manager::onEnable()
	{

	}

	// このスクリプトがアタッチされているGOのactiveSelfがfalseになった時呼ばれる
	void Player_manager::onDisable()
	{

	}

}