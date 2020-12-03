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

		static std::vector<object*> masters; //GO親子ツリーの頂点を保存
		static std::vector<object*> gos;	  //GOを1つの配列に保存

		static std::thread physics_thread;
		static bool physics_thread_finish;

		static int go_count;

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

		static Gameobject* create(const std::string& go_name, const u_int& tag, Scenelist Sce = Scene::now_scene);
		static Gameobject* create(const std::string& go_name, Scenelist Sce = Scene::now_scene) {return create(go_name, GO_tag::None, Sce);}
		static Gameobject* create(const u_int& tag, Scenelist Sce = Scene::now_scene) {return create(std::string("GO_" + std::to_string(go_count)), tag, Sce);}
		static Gameobject* create(Scenelist Sce = Scene::now_scene) {return create(std::string("GO_" + std::to_string(go_count)), GO_tag::None, Sce);}

		static Camera* create_camera(const std::string tag, Scenelist Sce = Scene::now_scene);
		static Camera* create_camera(Scenelist Sce = Scene::now_scene) { create_camera(std::string("camera"), Sce); }

		static Light* create_light(const std::string go_name, Scenelist Sce = Scene::now_scene);
		static Light* create_light(Scenelist Sce = Scene::now_scene) { create_light(std::string("light"), Sce); };

		//// =====================================================
		//// GameObjectを生成して、そのポインタを返す(名前を指定し、メッシュの参照を持つGameObjectを生成する。中でcreate(const string&)を呼ぶ)
		//// =====================================================
		//// const string&	:	GameObjectのnameに入れる名前
		//// const string&	:	使用するモデルファイルのパス
		//// =====================================================
		//// 戻り値 GameObject*
		//// =====================================================
		//static Gameobject* create(const u_int& go_name, const std::string& model_filename, Scenelist Sce = Scene::now_scene);

		// ==============================================================
		// プリミティブオブジェクトと適切なコライダーを作成(Primitive.modelへのファイルパスを持つ)
		// ==============================================================
		// const string&		:	GameObjectのnameに入れる名前(省略可)
		// bool					:	同じ名前を許可する
		// ==============================================================
		// 戻り値　GameObject*	:	生成したGameObject*を返す
		// ==============================================================


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
		static Gameobject* createPlane(const std::string go_name, const u_int = GO_tag::Plane, Scenelist Sce = Scene::now_scene);
		static Gameobject* createPlane(const u_int tag = GO_tag::Plane, Scenelist Sce = Scene::now_scene) {
			return createPlane(std::string("GO_" + std::to_string(go_count)), tag, Sce);
		}
		static Gameobject* createSprite(const std::string go_name, const u_int = GO_tag::Sprite, Scenelist Sce = Scene::now_scene);
		static Gameobject* createSprite(const u_int tag = GO_tag::Sprite, Scenelist Sce = Scene::now_scene) {
			return createSprite(std::string("GO_" + std::to_string(go_count)), tag, Sce);
		}


		static Gameobject* find(const std::string name, Scenelist Sce = Scene::now_scene)
		{
			const auto itr_end = gameobjects[Sce].end();
			for (auto itr = gameobjects[Sce].begin(); itr != itr_end; itr++)
			{
				if (itr->get()->name == name)
				{
					return itr->get();
				}
			}
			return nullptr;
		}
		static Camera* find_camera(const std::string& name, Scenelist Sce = Scene::now_scene)
		{
			const auto itr_end = cameras[Sce].end();
			for (auto itr = cameras[Sce].begin(); itr != itr_end; itr++)
			{
				if (itr->get()->name == name)
				{
					return itr->get();
				}
			}
			return nullptr;
		}
		static Light* find_light(const std::string& name, Scenelist Sce = Scene::now_scene)
		{
			const auto itr_end = lights[Sce].end();
			for (auto itr = lights[Sce].begin(); itr != itr_end; itr++)
			{
				if (itr->get()->name == name)
				{
					return itr->get();
				}
			}
			return nullptr;
		}


	};
}