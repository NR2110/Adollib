#include "scene_player.h"
#include "../Object/gameobject_manager.h"

#include "../../../Scripts/scene_game_all.h"
namespace Adollib {


	void Scene_player::initialize() {

		Gameobject* Player = Gameobject_manager::create("Player_manager");
		Player->addComponent<Player_manager>();

		Gameobject* camera = Gameobject_manager::create("camera");
		camera->addComponent<Camera>();
		camera->addComponent<Camera_component>();


	}



}