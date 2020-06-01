#include "scene_game.h"
#include "gameobject_manager.h"

#include "camera_manager.h"
#include "object_manager.h"
namespace Adollib {


	void scene_game::initialize() {
		{
			Gameobject* GO = Gameobject_manager::create();
			GO->addComponent<camera_manager_s>();
			//GO->transform->local_pos = vector3(0, 0, 0);
		}
		{
			Gameobject* GO = Gameobject_manager::create();
			GO->addComponent<object_manager>();
			//GO->transform->local_pos = vector3(0, 0, 3);
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