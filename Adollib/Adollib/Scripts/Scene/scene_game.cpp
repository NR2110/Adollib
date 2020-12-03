#include "scene_game.h"
#include "../Object/gameobject_manager.h"

#include "../../../Scripts/scene_game_all.h"
namespace Adollib {


	void scene_game::initialize() {

		//::: camera :::::::::
		{
			Gameobject* GO = Gameobject_manager::create("camera_manager");
			GO->addComponent<camera_manager_s>();
		}

		{
			Gameobject* GO = Gameobject_manager::create("object_manager");
			GO->addComponent<object_manager>();
			//GO->transform->local_pos = vector3(0, -5, 0);
			//GO->transform->local_scale = vector3(30, 0.1, 30);
			//GO->material->color = vector4(1, 0, 1, 1);
			//GO->add_collider_box();
		}

		//Gameobject_manager::create_camera();

		//Light* L = Gameobject_manager::create_light();
		//L->set_dirLight(vector3(0, -1, 0), vector3(1, 1, 0));

	}



}