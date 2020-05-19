#include "gameobject_manager.h"
#include "scene.h"

namespace Adollib {
	std::map<Scenelist, std::list<std::shared_ptr<Gameobject>>> Gameobject_manager::gameobjects = {};

	void Gameobject_manager::awake() {

		//sceneの数だけgo_managerを生成
		for (int i = 0; i < static_cast<int>(Scenelist::scene_list_size); i++) {

			std::list<std::shared_ptr<Gameobject>> go_manager;
			gameobjects[static_cast<Scenelist>(i)] = go_manager;
		}

	}

	void Gameobject_manager::initialize(Scenelist Sce) {
		if (Sce == Scenelist::scene_null)return;

		//Sceのsceneにアタッチされたcomponentのinitializeを呼ぶ
		std::list<std::shared_ptr<Gameobject>>::iterator itr = gameobjects[Sce].begin();
		std::list<std::shared_ptr<Gameobject>>::iterator itr_end = gameobjects[Sce].end();

		for (itr++; itr != itr_end; itr++) {
			itr->get()->initialize();
		}

	}

	void Gameobject_manager::update(Scenelist Sce) {
		if (Sce == Scenelist::scene_null)return;

		//Sceのsceneにアタッチされたcomponentのupdateを呼ぶ
		std::list<std::shared_ptr<Gameobject>>::iterator itr = gameobjects[Sce].begin();
		std::list<std::shared_ptr<Gameobject>>::iterator itr_end = gameobjects[Sce].end();

		for (itr++; itr != itr_end; itr++) {
			itr->get()->update();
		}

	}

	void Gameobject_manager::render(Scenelist Sce) {
		if (Sce == Scenelist::scene_null)return;

		//Sceのsceneにアタッチされたcomponentのrenderを呼ぶ
		std::list<std::shared_ptr<Gameobject>>::iterator itr = gameobjects[Sce].begin();
		std::list<std::shared_ptr<Gameobject>>::iterator itr_end = gameobjects[Sce].end();

		for (itr++; itr != itr_end; itr++) {
			itr->get()->render();
		}

	}

	void Gameobject_manager::destroy(Scenelist Sce) {
		if (Sce == Scenelist::scene_null)return;

		//適当にSceのsceneにアタッチされたgoを削除
		gameobjects[Sce].clear();
	}

	

	Gameobject* Gameobject_manager::create() {
		std::shared_ptr <Gameobject> Value;
		gameobjects[Scene::now_scene].push_back(Value);
		Value.get()->go_iterator = gameobjects[Scene::now_scene].end();
		Value.get()->this_scene = Scene::now_scene;

		return Value.get();
	}
	Gameobject* Gameobject_manager::create(const std::string go_name) {
		std::shared_ptr <Gameobject> Value;
		gameobjects[Scene::now_scene].push_back(Value);
		Value.get()->name = go_name;
		Value.get()->go_iterator = gameobjects[Scene::now_scene].end();
		Value.get()->this_scene = Scene::now_scene;

		return Value.get();
	}
	//Gameobject* Gameobject_manager::create(const std::string& go_name, const std::string& model_filename) {

	//}

	//Gameobject* Gameobject_manager::createSphere(const std::string& go_name) {

	//}
	//Gameobject* Gameobject_manager::createCylinder(const std::string& go_name) {

	//}
	//Gameobject* Gameobject_manager::createCube(const std::string& go_name) {

	//}
	//Gameobject* Gameobject_manager::createPlane(const std::string& go_name) {

	//}
}