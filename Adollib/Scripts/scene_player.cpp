#include "scene_player.h"
#include "../Adollib/Scripts/Object/gameobject_manager.h"

#include "player_manager.h"
namespace Adollib {


	void Scene_player::initialize() {

		Gameobject* Playre = Gameobject_manager::create("Player_manager");
		Playre->addComponent<Player_manager>();

	}



}