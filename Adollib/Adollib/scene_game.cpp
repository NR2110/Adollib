#include "scene_game.h"
#include "gameobject_manager.h"

#include "camera_manager.h"
#include "object_manager.h"

#include "object_fall.h"

#include "../Scripts/scene_game_all.h"
namespace Adollib {


	void scene_game::initialize() {

		//::: camera :::::::::
		{
			Gameobject* GO = Gameobject_manager::create();
			GO->addComponent<camera_manager_s>();
		}

		//::: field ::::::::::
		{
			Gameobject* GO = Gameobject_manager::createCube();
			GO->transform->local_pos = vector3(0, -1, 0);
			GO->transform->local_scale = vector3(30, 0.1, 30);
			GO->material->color = vector4(1, 0, 1, 1);

			GO->add_collider_box();
			GO->collider[0]->move = false;

			//GO->add_collider_plane();

		}

		{
			Gameobject* GO = Gameobject_manager::createFromFBX("../Data/FBX/tank.fbx");
			GO->transform->local_pos = vector3(0, 20, 0);

			//GO->add_collider_box(vector3(0,0.34,0), vector3(0.50,0.34,0.72));
			GO->add_collider_sphere();
			//player* P = GO->addComponent<player>();
			//P->collier = GO->collider[0];
			object_fall* O =  GO->addComponent<object_fall>();
			O->collier = GO->collider[0];
		}

		{
			//Gameobject* GO = Gameobject_manager::createCube();
			////GO->addComponent<camera_manager_s>();
			//GO->transform->local_pos = vector3(0, -5, 0);
			//GO->transform->local_scale = vector3(30, 0.1, 30);
			//GO->material->color = vector4(1, 0, 1, 1);
			////GO->add_collider_box();
		}

		//Gameobject_manager::create_camera();

		//Light* L = Gameobject_manager::create_light();
		//L->set_dirLight(vector3(0, -1, 0), vector3(1, 1, 0));

	}



}