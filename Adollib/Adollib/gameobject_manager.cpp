#include "gameobject_manager.h"
#include "scene.h"

namespace Adollib {
	std::map<Scenelist, std::list<std::shared_ptr<Gameobject>>> Gameobject_manager::gameobjects;
	std::map<Scenelist, std::list<std::shared_ptr<Light>>> Gameobject_manager::lights;
	std::map<Scenelist, std::list<std::shared_ptr<Camera>>> Gameobject_manager::cameras;

	void Gameobject_manager::awake() {

		//sceneの数だけgo_managerを生成
		for (int i = 0; i < static_cast<int>(Scenelist::scene_list_size); i++) {
			std::list<std::shared_ptr<Gameobject>> go_manager;
			gameobjects[static_cast<Scenelist>(i)] = go_manager;

			std::list<std::shared_ptr<Light>> li_manager;
			lights[static_cast<Scenelist>(i)] = li_manager;

			std::list<std::shared_ptr<Camera>> ca_manager;
			cameras[static_cast<Scenelist>(i)] = ca_manager;
		}

	}

	void Gameobject_manager::initialize(Scenelist Sce) {
		if (Sce == Scenelist::scene_null)return;

		{
			//Sceのsceneにアタッチされたcomponentのinitializeを呼ぶ
			std::list<std::shared_ptr<Gameobject>>::iterator itr = gameobjects[Sce].begin();
			std::list<std::shared_ptr<Gameobject>>::iterator itr_end = gameobjects[Sce].end();

			for (itr++; itr != itr_end; itr++) {
				itr->get()->initialize();
			}
		}
		{
			//light
			std::list<std::shared_ptr<Light>>::iterator itr = lights[Sce].begin();
			std::list<std::shared_ptr<Light>>::iterator itr_end = lights[Sce].end();

			for (itr++; itr != itr_end; itr++) {
				itr->get()->initialize();
			}
		}
		{
			//camera
			std::list<std::shared_ptr<Camera>>::iterator itr = cameras[Sce].begin();
			std::list<std::shared_ptr<Camera>>::iterator itr_end = cameras[Sce].end();

			for (itr++; itr != itr_end; itr++) {
				itr->get()->initialize();
			}
		}

	}

	void Gameobject_manager::update(Scenelist Sce) {
		if (Sce == Scenelist::scene_null)return;

		{
			//Sceのsceneにアタッチされたcomponentのupdateを呼ぶ
			std::list<std::shared_ptr<Gameobject>>::iterator itr = gameobjects[Sce].begin();
			std::list<std::shared_ptr<Gameobject>>::iterator itr_end = gameobjects[Sce].end();

			for (itr++; itr != itr_end; itr++) {
				itr->get()->update();
			}
		}
		{
			//light
			std::list<std::shared_ptr<Light>>::iterator itr = lights[Sce].begin();
			std::list<std::shared_ptr<Light>>::iterator itr_end = lights[Sce].end();

			for (itr++; itr != itr_end; itr++) {
				itr->get()->update();
			}
		}
		{
			//camera
			std::list<std::shared_ptr<Camera>>::iterator itr = cameras[Sce].begin();
			std::list<std::shared_ptr<Camera>>::iterator itr_end = cameras[Sce].end();

			for (itr++; itr != itr_end; itr++) {
				itr->get()->update();
			}
		}

	}

	void Gameobject_manager::render(Scenelist Sce) {
		if (Sce == Scenelist::scene_null)return;

		{
			//Sceのsceneにアタッチされたcomponentのrenderを呼ぶ
			std::list<std::shared_ptr<Gameobject>>::iterator itr = gameobjects[Sce].begin();
			std::list<std::shared_ptr<Gameobject>>::iterator itr_end = gameobjects[Sce].end();

			for (itr++; itr != itr_end; itr++) {
				itr->get()->render();
			}
		}

	}





	void Gameobject_manager::destroy(Scenelist Sce) {
		if (Sce == Scenelist::scene_null)return;

		//適当にSceのsceneにアタッチされたgoを削除
		gameobjects[Sce].clear();
		lights[Sce].clear();
		cameras[Sce].clear();

	}

	

	Gameobject* Gameobject_manager::create(Scenelist Sce) {
		std::shared_ptr <Gameobject> Value;
		gameobjects[Sce].push_back(Value);
		Value.get()->go_iterator = gameobjects[Sce].end();
		Value.get()->this_scene = Sce;

		return Value.get();
	}
	Gameobject* Gameobject_manager::create(const std::string go_name, Scenelist Sce) {
		std::shared_ptr <Gameobject> Value;
		gameobjects[Sce].push_back(Value);
		Value.get()->name = go_name;
		Value.get()->go_iterator = gameobjects[Sce].end();
		Value.get()->this_scene = Sce;

		return Value.get();
	}

	Light* Gameobject_manager::create_light(Scenelist Sce) {
		std::shared_ptr <Light> Value;
		lights[Sce].push_back(Value);
		Value.get()->go_iterator = lights[Sce].end();
		Value.get()->this_scene = Sce;

		return Value.get();
	}
	Light* Gameobject_manager::create_light(const std::string go_name, Scenelist Sce) {
		std::shared_ptr <Light> Value;
		lights[Sce].push_back(Value);
		Value.get()->name = go_name;
		Value.get()->go_iterator = lights[Sce].end();
		Value.get()->this_scene = Sce;

		return Value.get();
	}

	Camera* Gameobject_manager::create_camera(Scenelist Sce) {
		std::shared_ptr <Camera> Value;
		cameras[Sce].push_back(Value);
		Value.get()->go_iterator = cameras[Sce].end();
		Value.get()->this_scene = Sce;

		return Value.get();
	}
	Camera* Gameobject_manager::create_camera(const std::string go_name, Scenelist Sce) {
		std::shared_ptr <Camera> Value;
		cameras[Sce].push_back(Value);
		Value.get()->name = go_name;
		Value.get()->go_iterator = cameras[Sce].end();
		Value.get()->this_scene = Sce;

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