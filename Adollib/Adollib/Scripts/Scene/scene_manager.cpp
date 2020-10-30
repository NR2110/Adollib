#include "../Object/gameobject_manager.h"

#include "scene_manager.h"
#include "scene_game.h"
#include "scene_title.h"

namespace Adollib {

	Scenelist Scene::now_scene = Scenelist::scene_null;
	Scenelist Scene::next_scene = Scenelist::scene_null;

	//::: Scene‚ğ•ÏX‚·‚éÛ‚Í‚±‚±‚ğ•ÏX ::::::::::::::::::::::::::::::::::::::::
	void Scene_manager::initialize() {
		Scene::now_scene = Scenelist::scene_null;
		Scene::next_scene = Scenelist::scene_game;

		scenes[Scenelist::scene_game] = std::make_unique<scene_game>();
		scenes[Scenelist::scene_title] = std::make_unique<scene_title>();
	}
	//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

	void Scene_manager::update() {

		if (!(Scene::now_scene == Scene::next_scene)) {

			Gameobject_manager::destroy(Scene::now_scene); //‘Oscene‚Ìgameobject‚ğíœ

			Scene::now_scene = Scene::next_scene;
			if(Scene::now_scene != Scenelist::scene_null)

				scenes[Scene::now_scene]->initialize(); //Ÿscene‚Ìcomponent‚ğİ’è

			Gameobject_manager::initialize(Scene::now_scene); //Ÿscene‚Ìinitialize‚ğŒÄ‚Ô
	
		}

	}


}