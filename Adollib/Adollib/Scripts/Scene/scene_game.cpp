#include "scene_game.h"
#include "../Object/gameobject_manager.h"

#include "../../../Scripts/scene_game_all.h"
namespace Adollib {


	void scene_game::initialize() {


		Gameobject* camera = Gameobject_manager::create("camera");
		camera->addComponent<Camera>();
		camera->addComponent<Camera_component>();

		Gameobject* Playre = Gameobject_manager::create("Player_manager");
		Playre->addComponent<Player_manager>();


		Gameobject* GO = Gameobject_manager::create("Stage_manager");
		GO->addComponent<Stage_manager>();


	}



}