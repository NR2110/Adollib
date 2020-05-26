#include "scene_game.h"
#include "gameobject_manager.h"

namespace Adollib {


	void scene_game::initialize() {
		int i = 0;

		Gameobject_manager::createSphere();

		Gameobject_manager::create_camera();

	}



}