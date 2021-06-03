#pragma once
#include "gameobject.h"
#include "gameobject_light.h"
#include "gameobject_camera.h"

#include "../Scene/scene.h"
#include "../Scene/scene_list.h"

#include <d3d11.h>
#include <WRL.h>
#include <iostream>
#include <thread>
#include <mutex>
#include <map>


namespace Adollib {
	class Gameobject_manager{
	private:
		static Microsoft::WRL::ComPtr<ID3D11Buffer> light_cb;
		static Microsoft::WRL::ComPtr<ID3D11Buffer> view_cb;
		static Microsoft::WRL::ComPtr<ID3D11Buffer> projection_cb;

		static int go_count;

		//’x‚ê‚¹‚Ädelete‚·‚é‚½‚ß‚±‚±‚É•Û‘¶‚·‚é
		static std::vector<Gameobject*> save_delete_gameobject;
		static std::vector<Camera*>		save_delete_camera;
		static std::vector<Light*>		save_delete_light;

		static void delete_gameobjects(){
			for (auto go : save_delete_gameobject) {
				go->destroy();
				delete go;
			}
			for (auto go : save_delete_camera) {
				go->destroy();
				delete go;
			}
			for (auto go : save_delete_light) {
				go->destroy();
				delete go;
			}

			save_delete_gameobject.clear();
			save_delete_camera.clear();
			save_delete_light.clear();
		}

	public:
		//ŽÀ‘Ì‚Í‚·‚×‚Ä‚±‚±‚Å•Û‘¶‚·‚é
		static std::map<Scenelist, std::list<Gameobject*>> gameobjects;
		//light
		static std::map<Scenelist, std::list<Light*>> lights;
		//camera
		static std::map<Scenelist, std::list<Camera*>> cameras;

		void awake();
		static void initialize(Scenelist Sce = Scene::now_scene);
		static void update(Scenelist Sce = Scene::now_scene);
		static void render(Scenelist Sce = Scene::now_scene);
		static void destroy(Scenelist Sce = Scene::now_scene);

		static Gameobject* create(const std::string& go_name, const u_int& tag, Scenelist Sce = Scene::now_scene);
		static Gameobject* create(const std::string& go_name, Scenelist Sce = Scene::now_scene) {return create(go_name, GO_tag::None, Sce);}
		static Gameobject* create(const u_int& tag, Scenelist Sce = Scene::now_scene) {return create(std::string("GO_" + std::to_string(go_count)), tag, Sce);}
		static Gameobject* create(Scenelist Sce = Scene::now_scene) {return create(std::string("GO_" + std::to_string(go_count)), GO_tag::None, Sce);}

		static Camera* create_camera(const std::string tag, Scenelist Sce = Scene::now_scene);
		static Camera* create_camera(Scenelist Sce = Scene::now_scene) { create_camera(std::string("camera"), Sce); }

		static Light* create_light(const std::string go_name, Scenelist Sce = Scene::now_scene);
		static Light* create_light(Scenelist Sce = Scene::now_scene) { create_light(std::string("light"), Sce); };



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
		static void removeCamera(Scenelist Sce, std::list<Camera*>::iterator itr) { cameras[Sce].erase(itr); };
		static void removeLight(Scenelist Sce, std::list<Light*>::iterator itr) { lights[Sce].erase(itr); };

		//GO‚Ìíœ‚ðs‚¤
		static void deleteGameobject(Gameobject* gameobject) {
			save_delete_gameobject.emplace_back(gameobject);
		};
		static void deleteCamera(Camera* gameobject) {
			save_delete_camera.emplace_back(gameobject);
		};
		static void deleteLight(Light* gameobject) {
			save_delete_light.emplace_back(gameobject);
		};


		static Gameobject* find(const std::string name, Scenelist Sce = Scene::now_scene)
		{
			for (auto& go : gameobjects[Sce])
			{
				if (go->name == name) return go;
			}
			return nullptr;
		}
		static Camera* find_camera(const std::string& name, Scenelist Sce = Scene::now_scene)
		{
			const auto itr_end = cameras[Sce].end();
			for (auto& go : cameras[Sce])
			{
				if (go->name == name) return go;
			}
			return nullptr;
		}
		static Light* find_light(const std::string& name, Scenelist Sce = Scene::now_scene)
		{
			const auto itr_end = lights[Sce].end();
			for (auto& go : lights[Sce])
			{
				if (go->name == name) return go;
			}
			return nullptr;
		}


	};
}