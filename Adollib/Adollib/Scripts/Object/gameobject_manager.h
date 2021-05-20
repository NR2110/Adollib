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

		static std::vector<object*> masters; //GO�e�q�c���[�̒��_��ۑ�
		static std::vector<object*> gos;	  //GO��1�̔z��ɕۑ�

		static std::thread physics_thread;
		static bool physics_thread_finish;

		static int go_count;

	public:
		//���̂͂��ׂĂ����ŕۑ�����
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

		//GO�̍폜���s��
		static void deleteGameobject(Gameobject* gameobject) {
			gameobject->destroy();
			delete gameobject;
		};
		static void deleteCamera(Camera* gameobject) {
			gameobject->destroy();
			delete gameobject;
		};
		static void deleteLight(Light* gameobject) {
			gameobject->destroy();
			delete gameobject;
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