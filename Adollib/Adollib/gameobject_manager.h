#pragma once

#include <map>
#include "gameobject.h"
#include "gameobject_light.h"
#include "gameobject_camera.h"

#include "scene.h"
#include "scene_list.h"

namespace Adollib {
	class Gameobject_manager{
		

	private:

	public:
		//実体はすべてここで保存する
		static std::map<Scenelist, std::list<std::shared_ptr<Gameobject>>> gameobjects;
		//light
		static std::map<Scenelist, std::list<std::shared_ptr<Light>>> lights;
		//camera
		static std::map<Scenelist, std::list<std::shared_ptr<Camera>>> cameras;

		void awake();
		static void initialize(Scenelist Sce = Scene::now_scene);
		static void update(Scenelist Sce = Scene::now_scene);
		static void render(Scenelist Sce = Scene::now_scene);
		static void destroy(Scenelist Sce = Scene::now_scene);

		static Gameobject* create(Scenelist Sce = Scene::now_scene);
		static Gameobject* create(const std::string go_name, Scenelist Sce = Scene::now_scene);

		static Camera* create_camera(Scenelist Sce = Scene::now_scene);
		static Camera* create_camera(const std::string go_name, Scenelist Sce = Scene::now_scene);

		static Light* create_light(Scenelist Sce = Scene::now_scene);
		static Light* create_light(const std::string go_name, Scenelist Sce = Scene::now_scene);

		// =====================================================
		// GameObjectを生成して、そのポインタを返す(名前を指定し、メッシュの参照を持つGameObjectを生成する。中でcreate(const string&)を呼ぶ)
		// =====================================================
		// const string&	:	GameObjectのnameに入れる名前
		// const string&	:	使用するモデルファイルのパス
		// =====================================================
		// 戻り値 GameObject*
		// =====================================================
		static Gameobject* create(const std::string& go_name, const std::string& model_filename, Scenelist Sce = Scene::now_scene);

		// ==============================================================
		// プリミティブオブジェクトと適切なコライダーを作成(Primitive.modelへのファイルパスを持つ)
		// ==============================================================
		// const string&		:	GameObjectのnameに入れる名前(省略可)
		// bool					:	同じ名前を許可する
		// ==============================================================
		// 戻り値　GameObject*	:	生成したGameObject*を返す
		// ==============================================================
		static Gameobject* createSphere(const std::string& go_name = "Sphere", Scenelist Sce = Scene::now_scene);
		static Gameobject* createCylinder(const std::string& go_name = "Cylinder", Scenelist Sce = Scene::now_scene);
		static Gameobject* createCube(const std::string& go_name = "Cube", Scenelist Sce = Scene::now_scene);
		static Gameobject* createPlane(const std::string& go_name = "Plane", Scenelist Sce = Scene::now_scene);
		static Gameobject* createSprite(const std::string& go_name = "Sprite", Scenelist Sce = Scene::now_scene);


	};
}