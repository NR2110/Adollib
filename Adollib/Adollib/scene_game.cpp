#include "scene_game.h"
#include "gameobject_manager.h"

#include "camera_manager.h"
namespace Adollib {


	void scene_game::initialize() {
		int i = 0;

		Gameobject* GO =  Gameobject_manager::createSphere();
		GO->addComponent<camera_manager_s>();

		//Gameobject_manager::create_camera();

		Light* L = Gameobject_manager::create_light();
		L->set_dirLight(vector3(0, -1, 0), vector3(1, 1, 0));

	}



}