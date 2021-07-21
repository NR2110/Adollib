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

		if (0)
		{
			Gameobject* GO = Gameobject_manager::create("object_manager");
			GO->addComponent<object_manager>();
		}
		else
		{
			Gameobject* GO = Gameobject_manager::create("object_manager");
			GO->addComponent<object_manager>();

			//Gameobject* GO = Gameobject_manager::create("Demo_house");
			//GO->addComponent<Demo_house>();

			Gameobject* Playre = Gameobject_manager::create("Player_manager");
			Playre->addComponent<Player_manager>();

			//GO->transform->local_pos = vector3(0, -5, 0);
			//GO->transform->local_scale = vector3(30, 0.1, 30);
			//GO->material->color = vector4(1, 0, 1, 1);
			//GO->add_collider_box();
		}

		//{
		//	Gameobject* GO = Gameobject_manager::create("player_manager");
		//	GO->addComponent<Player_manager>();
		//}

		//Gameobject_manager::create_camera();

		//Light* L = Gameobject_manager::create_light();
		//L->set_dirLight(vector3(0, -1, 0), vector3(1, 1, 0));

	}



}