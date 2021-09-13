#include "scene_game.h"
#include "../Object/gameobject_manager.h"

#include "../../../Scripts/scene_game_all.h"
namespace Adollib {


	void scene_game::initialize() {


		if (0)
		{
			// debug_stage
			Gameobject* GO = Gameobject_manager::create("object_manager");
			GO->addComponent<object_manager>();

			// camera
			Gameobject* camera = Gameobject_manager::create("camera_manager");
			camera->addComponent<camera_manager_s>();

		}
		else if (1)
		{
			Gameobject* camera = Gameobject_manager::create("camera");
			camera->addComponent<Camera>();
			camera->addComponent<Camera_component>();

			Gameobject* GO = Gameobject_manager::create("Stage_demo");
			GO->addComponent<Stage_demo>();

			Gameobject* Playre = Gameobject_manager::create("Player_manager");
			Playre->addComponent<Player_manager>();

		}
		else if (0)
		{
			Gameobject* camera = Gameobject_manager::create("camera");
			camera->addComponent<Camera>();
			camera->addComponent<Camera_component>();

			Gameobject* GO = Gameobject_manager::create("Demo_house");
			GO->addComponent<Demo_house>();

			Gameobject* Playre = Gameobject_manager::create("Player_manager");
			Playre->addComponent<Player_manager>();

		}
		else if(1)
		{
			Gameobject* camera = Gameobject_manager::create("camera");
			camera->addComponent<Camera>();
			camera->addComponent<Camera_component>();

			Gameobject* GO = Gameobject_manager::create("object_manager");
			GO->addComponent<object_manager>();

			Gameobject* Playre = Gameobject_manager::create("Player_manager");
			Playre->addComponent<Player_manager>();

		}


	}



}