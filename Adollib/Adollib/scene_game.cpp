#include "scene_game.h"
#include "gameobject_manager.h"

#include "../Scripts/scene_game_all.h"
namespace Adollib {


	void scene_game::initialize() {

		//::: camera :::::::::
		{
			Gameobject* GO = Gameobject_manager::create();
			GO->addComponent<camera_manager_s>();
		}

		////::: field ::::::::::
		//{
		//	Gameobject* GO = Gameobject_manager::createCube();
		//	GO->transform->local_pos = vector3(0, -1, 0);
		//	GO->transform->local_scale = vector3(60, 0.1, 60);
		//	GO->material->color = vector4(1, 1, 1, 1);

		//	//Rigitbody* R = GO->add_collider_box();
		//	Rigitbody* R = GO->add_collider_plane();
		//	//Rigitbody* R = GO->add_collider_sphere();
		//	R->move = false;

		//}

		////::: player :::::::::
		//{
		//	Gameobject* GO = Gameobject_manager::createFromFBX("../Data/FBX/tank.fbx","player");
		//	GO->transform->local_pos = vector3(0, 5, -5);
		//	GO->transform->local_orient = quaternion_from_euler(30, 0, 30);
		//	GO->transform->local_scale = vector3(5, 5, 5);
		//	GO->material->color = vector4(0, 0, 1, 1);

		//	GO->add_collider_box(vector3(0, 0.34, 0), vector3(0.50, 0.34, 0.72));

		//	object_fall* O = GO->addComponent<object_fall>();
		//	O->collier = GO->collider[0];

		//	player* R = GO->addComponent<player>();
		//	R->collier = GO->collider[0];

		//	//object_rotate* OR = GO->addComponent <object_rotate> ();
		//	//OR->collier = GO->collider[0];
		//}

		////::: enemy ::::::::
		//{
		//	Gameobject* GO = Gameobject_manager::create();
		//	GO->addComponent<Enemy_manager>();
		//}



		{
			Gameobject* GO = Gameobject_manager::create();
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