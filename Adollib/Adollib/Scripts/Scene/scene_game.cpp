#include "scene_game.h"
#include "../Object/gameobject_manager.h"

#include "../../../Scripts/scene_game_all.h"
namespace Adollib {

	void Scene_game::awake() {
		{
			Gameobject* GO = Gameobject_manager::create("Stage_manager");
			auto comp = GO->addComponent<Stage_manager>();
			Gameobject_manager::find("Player_manager",Scenelist::scene_player)->findComponent<Player_manager>()->set_stage_manager_ptr(comp);
		}

		{
			Gameobject* GO = Gameobject_manager::create("Tutrial_manager");
			auto comp = GO->addComponent<Tutrial_manager>();
		}

	}

	void Scene_game::initialize() {

		//Gameobject* camera = Gameobject_manager::create("camera");
		//camera->addComponent<Camera>();
		//camera->addComponent<Camera_component>();

		//Gameobject* Player = Gameobject_manager::create("Player_manager");
		//Player->addComponent<Player_manager>();


		//Gameobject_manager::find("camera")->findComponent<Camera_component>()->gameobject->is_active = true;
		//Gameobject_manager::find("camera", Scenelist::scene_player)->findComponent<Camera_component>()->gameobject->is_active = true;



	}

	void Scene_game::destroy() {
		//Scene_manager::set_inactive(Scenelist::scene_player);
	}



}