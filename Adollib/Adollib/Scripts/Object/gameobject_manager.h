#pragma once
#include "gameobject.h"

#include "../Scene/scene.h"
#include "../Scene/scene_list.h"

#include <d3d11.h>
#include <WRL.h>
#include <iostream>
#include <thread>
#include <mutex>
#include <map>


namespace Adollib {

	class Camera_component;
	class Light_component;


	class Gameobject_manager{
	private:
		static Microsoft::WRL::ComPtr<ID3D11Buffer> light_cb;
		static Microsoft::WRL::ComPtr<ID3D11Buffer> view_cb;
		static Microsoft::WRL::ComPtr<ID3D11Buffer> projection_cb;

		static int go_count;

		//遅れせてdeleteするためここに保存する
		static std::vector<Gameobject*> save_delete_gameobject;

		static void delete_gameobjects(){
			for (auto go : save_delete_gameobject) {
				go->destroy();
				delete go;
			}

			save_delete_gameobject.clear();
		}

	private:
		//Gameobjectのポインタ
		static std::map<Scenelist, std::list<Gameobject*>> gameobjects;

		//camera_componentへのポインタ
		static std::map<Scenelist, std::list<Camera_component*>> cameras;
		static std::map<Scenelist, std::list<Light_component*>> lights;

	public:

		void awake();
		static void initialize(Scenelist Sce = Scene::now_scene);
		static void update(Scenelist Sce = Scene::now_scene);
		static void render(Scenelist Sce = Scene::now_scene);
		static void destroy(Scenelist Sce = Scene::now_scene);

		static Gameobject* create(const std::string& go_name, const u_int& tag, Scenelist Sce = Scene::now_scene);
		static Gameobject* create(const std::string& go_name, Scenelist Sce = Scene::now_scene) {return create(go_name, GO_tag::None, Sce);}
		static Gameobject* create(const u_int& tag, Scenelist Sce = Scene::now_scene) {return create(std::string("GO_" + std::to_string(go_count)), tag, Sce);}
		static Gameobject* create(Scenelist Sce = Scene::now_scene) {return create(std::string("GO_" + std::to_string(go_count)), GO_tag::None, Sce);}

		static std::list<Camera_component*>::iterator add_camera_component(const Scenelist& scene, Camera_component* c_comp_ptr) {
			cameras[scene].emplace_back(c_comp_ptr);
			auto itr = cameras[scene].end();
			itr--;
			return itr;
		};
		static void remove_camera_component(const Scenelist& scene, std::list<Camera_component*>::iterator itr) {
			cameras[scene].erase(itr);
		};

		static std::list<Light_component*>::iterator add_light_component(const Scenelist& scene, Light_component* c_comp_ptr) {
			lights[scene].emplace_back(c_comp_ptr);
			auto itr = lights[scene].end();
			itr--;
			return itr;
		};
		static void remove_light_component(const Scenelist& scene, std::list<Light_component*>::iterator itr) {
			lights[scene].erase(itr);
		};


		static Gameobject* createFromFBX(const std::string go_name, const std::string& FBX_pass, const u_int = GO_tag::FBX, Scenelist Sce = Scene::now_scene);
		static Gameobject* createFromFBX(const std::string& FBX_pass, const u_int tag = GO_tag::FBX, Scenelist Sce = Scene::now_scene) {
			return createFromFBX(std::string("GO_" + std::to_string(go_count)), FBX_pass, tag, Sce);
		};
		static Gameobject* createSphere(const std::string go_name, const u_int = GO_tag::Sphere, Scenelist Sce = Scene::now_scene);
		static Gameobject* createSphere(const u_int tag = GO_tag::Sphere, Scenelist Sce = Scene::now_scene) {
			return createSphere(std::string("GO_" + std::to_string(go_count)), tag, Sce);
		}
		static Gameobject* createCylinder(const std::string go_name, const u_int = GO_tag::Cylinder, Scenelist Sce = Scene::now_scene);
		static Gameobject* createCylinder(const u_int tag = GO_tag::Cylinder, Scenelist Sce = Scene::now_scene) {
			return createCylinder(std::string("GO_" + std::to_string(go_count)), tag, Sce);
		}
		static Gameobject* createCube(const std::string go_name, const u_int = GO_tag::Box, Scenelist Sce = Scene::now_scene);
		static Gameobject* createCube(const u_int tag = GO_tag::Box, Scenelist Sce = Scene::now_scene) {
			return createCube(std::string("GO_" + std::to_string(go_count)), tag, Sce);
		}
		static Gameobject* createCapsule(const std::string go_name, const u_int = GO_tag::Box, Scenelist Sce = Scene::now_scene);
		static Gameobject* createCapsule(const u_int tag = GO_tag::Box, Scenelist Sce = Scene::now_scene) {
			return createCube(std::string("GO_" + std::to_string(go_count)), tag, Sce);
		}
		static Gameobject* createPlane(const std::string go_name, const u_int = GO_tag::Plane, Scenelist Sce = Scene::now_scene);
		static Gameobject* createPlane(const u_int tag = GO_tag::Plane, Scenelist Sce = Scene::now_scene) {
			return createPlane(std::string("GO_" + std::to_string(go_count)), tag, Sce);
		}
		static Gameobject* createSprite(const std::string go_name, const u_int = GO_tag::Sprite, Scenelist Sce = Scene::now_scene);
		static Gameobject* createSprite(const u_int tag = GO_tag::Sprite, Scenelist Sce = Scene::now_scene) {
			return createSprite(std::string("GO_" + std::to_string(go_count)), tag, Sce);
		}

		static void removeGameobject(Scenelist Sce,std::list<Gameobject*>::iterator itr) { gameobjects[Sce].erase(itr); };

		//GOの削除を行う
		static void deleteGameobject(Gameobject* gameobject) {
			save_delete_gameobject.emplace_back(gameobject);
		};


		static Gameobject* find(const std::string name, Scenelist Sce = Scene::now_scene)
		{
			for (auto& go : gameobjects[Sce])
			{
				if (go->name == name) return go;
			}
			return nullptr;
		}


	};
}