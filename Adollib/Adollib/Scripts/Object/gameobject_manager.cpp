
#include "gameobject_manager.h"

#include "component_camera.h"
#include "component_light.h"

#include "../Main/resource_manager.h"
#include "../Main/systems.h"

#include "../Scene/scene.h"

#include "../Renderer/renderer_manager.h"
#include "../Renderer/mesh_renderer.h"
#include "../Renderer/sprite_renderer.h"
#include "../Renderer/Shader/constant_buffer.h"
#include "../Renderer/Shader/light_types.h"
#include "../Renderer/material_for_collider.h"

#include "../Physics/ALP__physics_manager.h"
#include "hierarchy.h"

#include "../Imgui/work_meter.h"

#include <future>

#include <thread>

#define Use_physics_thread

using namespace Adollib;
using namespace Physics_function;
using namespace ConstantBuffer;

std::map<Scenelist, std::list<Gameobject*>> Gameobject_manager::gameobjects;
std::list<Gameobject*> Gameobject_manager::added_gameobjects;
std::map<Scenelist, std::list<Light_component*>> Gameobject_manager::lights;
std::map<Scenelist, std::list<Camera_component*>> Gameobject_manager::cameras;

std::vector<Gameobject*>	Gameobject_manager::save_delete_gameobject;

int Gameobject_manager::go_count = 0;

void Gameobject_manager::awake() {

	Systems::SetDephtStencilState(State_manager::DStypes::DS_TRUE);
	Systems::SetRasterizerState(State_manager::RStypes::RS_CULL_BACK);
	Systems::SetBlendState(State_manager::BStypes::BS_ADD);

	Renderer_manager::awake();

	//sceneの数だけlistを生成
	for (int i = 0; i < static_cast<int>(Scenelist::scene_list_size); i++) {
		std::list<Gameobject*> go_manager;
		gameobjects[static_cast<Scenelist>(i)] = go_manager;
	}

	//Physics_function::Collider_renderer::awake();



#ifdef Use_physics_thread
	//Physics_manager::thread_start();

#endif // Use_physics_thread

}

void Gameobject_manager::initialize(Scenelist Sce) {
	if (Sce == Scenelist::scene_null)return;

	for (auto& GO : gameobjects[Sce]) {
		GO->initialize();
	}

}

void Gameobject_manager::update(Scenelist Sce) {

	if (Sce == Scenelist::scene_null)return;

	Work_meter::start("update");
	Work_meter::tag_start("update");

	auto& gos = gameobjects[Sce];

	// 一番上の親を保存
	std::vector<Gameobject*> masters; //GO親子ツリーの頂点を保存
	masters.clear();
	{
		std::unordered_map<Gameobject*, bool> masters_manag;
		for (auto& GO : gos) {
			Gameobject* master = GO->top_parent();
			if (masters_manag.count(master) == 0) {
				masters.emplace_back(master);
			}
			masters_manag[master] = true;
		}
	}

#ifndef Use_physics_thread
	Physics_manager::update();

#endif // Use_physics_thread

	{
		// Work_meter::start("adapt_transform_to_gameobject");
		// Physics_manager::adapt_transform_to_gameobject();
		// Work_meter::stop("adapt_transform_to_gameobject");

		// 親から子に座標の更新を行う
		{
			std::unordered_map<Gameobject*, bool> masters_manag;
			for (auto& GO : gos) {
				Gameobject* master = GO->top_parent();
				if (masters_manag.count(master) == 0) {
					master->update_world_trans_to_children();
				}
				masters_manag[master] = true;
			}
		}

		// Work_meter::start("copy_gameobject_transform");
		// Physics_manager::copy_gameobject_transform();
		// Work_meter::stop("copy_gameobject_transform");
	}

	Work_meter::start("update_to_children");

	added_gameobjects.clear();
	// 親から子へupdateを呼ぶ update中に、親objectが削除されたときに対応できないため削除はいったんbufferに保管している
	std::for_each(masters.begin(), masters.end(), [](Gameobject* ob) {ob->update_to_children(); });

	// 追加されたもののinitializeを呼ぶ
	std::for_each(added_gameobjects.begin(), added_gameobjects.end(), [](Gameobject* ob) {ob->initialize(); });

	Work_meter::stop("update_to_children");

	// 親から子に座標の更新を行う
	{
		std::unordered_map<Gameobject*, bool> masters_manag;

		for (auto& GO : gos) {
			Gameobject* master = GO->top_parent();
			if (masters_manag.count(master) == 0) {
				master->update_world_trans_to_children();
			}
			masters_manag[master] = true;
		}

	}


#ifdef UseImgui
	// ヒエラルキー
	Work_meter::start("update_hierarchy");
	Hierarchy::update_hierarchy(masters);
	Work_meter::stop("update_hierarchy");

#endif // UseImgui

	// 親から子に座標の更新を行う
	{
		std::unordered_map<Gameobject*, bool> masters_manag;

		for (auto& GO : gos) {
			Gameobject* master = GO->top_parent();
			if (masters_manag.count(master) == 0) {
				master->update_world_trans_to_children();
			}
			masters_manag[master] = true;
		}

	}

	delete_gameobjects();

	Work_meter::tag_stop();
	Work_meter::stop("update");
}

void Gameobject_manager::render(const std::list<Scenelist>& active_scenes) {

	Renderer_manager::render(active_scenes, cameras, lights);
}



void Gameobject_manager::destroy(Scenelist Sce) {
	if (Sce == Scenelist::scene_null)return;

	//終了処理
	for (auto& GO : gameobjects[Sce]) {
		GO->clearComponent();
		delete GO;
	}

	//適当にSceのsceneにアタッチされたgoを削除
	gameobjects[Sce].clear();
	cameras[Sce].clear();
	lights[Sce].clear();

}


Gameobject* Gameobject_manager::create(const std::string& go_name, const u_int& tag, Scenelist Sce) {
	Gameobject* null = nullptr;
	gameobjects[Sce].emplace_back(null);
	auto itr = gameobjects[Sce].end();
	itr--;
	*itr = newD Gameobject(true, Sce, itr);
	Gameobject* Value = *itr;
	added_gameobjects.emplace_back(*itr);

	Value->tag = tag;
	Value->name = go_name;
	Value->transform = std::make_shared<Transform>();

	Value->initialize();
	++go_count;
	return Value;
}


Gameobject* Gameobject_manager::createFromFBX(const std::string go_name, const std::string FBX_pass, const bool is_marge_vertex, u_int tag, Scenelist Sce) {
	Gameobject* null = nullptr;
	gameobjects[Sce].emplace_back(null);
	auto itr = gameobjects[Sce].end();
	itr--;
	*itr = newD Gameobject(false, Sce, itr);
	Gameobject* Value = *itr;
	added_gameobjects.emplace_back(*itr);

	Value->tag = tag;
	Value->name = go_name;
	Value->transform = std::make_shared<Transform>();

	Value->renderer = Value->addComponent<Mesh_renderer>();

	std::vector<Mesh::mesh>* meshes = nullptr;
	ResourceManager::CreateModelFromFBX(&meshes, FBX_pass.c_str(), is_marge_vertex);
	Value->renderer->set_meshes(meshes);

	Value->initialize();
	++go_count;
	return Value;
}

Gameobject* Gameobject_manager::createSphere(const std::string go_name, Scenelist Sce) {
	Gameobject* null = nullptr;
	gameobjects[Sce].emplace_back(null);
	auto itr = gameobjects[Sce].end();
	itr--;
	*itr = newD Gameobject(false, Sce, itr);
	Gameobject* Value = *itr;
	added_gameobjects.emplace_back(*itr);

	Value->tag = GO_tag::Sphere;
	Value->name = go_name;
	Value->transform = std::make_shared<Transform>();

	Value->renderer = Value->addComponent<Mesh_renderer>();

	std::vector<Mesh::mesh>* meshes = nullptr;
	ResourceManager::CreateModelFromFBX(&meshes, "./DefaultModel/sphere.fbx");
	Value->renderer->set_meshes(meshes);

	Value->initialize();
	++go_count;
	return Value;
}

Gameobject* Gameobject_manager::createCube(const std::string go_name, Scenelist Sce) {
	Gameobject* null = nullptr;
	gameobjects[Sce].emplace_back(null);
	auto itr = gameobjects[Sce].end();
	itr--;
	*itr = newD Gameobject(false, Sce, itr);
	Gameobject* Value = *itr;
	added_gameobjects.emplace_back(*itr);

	Value->tag = GO_tag::Box;
	Value->name = go_name;
	Value->transform = std::make_shared<Transform>();

	Value->renderer = Value->addComponent<Mesh_renderer>();

	std::vector<Mesh::mesh>* meshes = nullptr;
	ResourceManager::CreateModelFromFBX(&meshes, "./DefaultModel/cube.fbx");
	Value->renderer->set_meshes(meshes);

	++go_count;
	Value->initialize();
	return Value;
}

Gameobject* Gameobject_manager::createCapsule(const std::string go_name, Scenelist Sce) {
	Gameobject* null = nullptr;
	gameobjects[Sce].emplace_back(null);
	auto itr = gameobjects[Sce].end();
	itr--;
	*itr = newD Gameobject(false, Sce, itr);
	Gameobject* Value = *itr;
	added_gameobjects.emplace_back(*itr);

	Value->tag = GO_tag::Capsule;
	Value->name = go_name;
	Value->transform = std::make_shared<Transform>();

	Value->renderer = Value->addComponent<Mesh_renderer>();

	std::vector<Mesh::mesh>* meshes = nullptr;
	ResourceManager::CreateModelFromFBX(&meshes, "./DefaultModel/cube.fbx");
	Value->renderer->set_meshes(meshes);

	++go_count;
	Value->initialize();
	return Value;
}
//Gameobject* Gameobject_manager::createCylinder( u_int go_name) {

//}
Gameobject* Gameobject_manager::createCylinder(const std::string go_name, Scenelist Sce) {
	Gameobject* null = nullptr;
	gameobjects[Sce].emplace_back(null);
	auto itr = gameobjects[Sce].end();
	itr--;
	*itr = newD Gameobject(false, Sce, itr);
	Gameobject* Value = *itr;
	added_gameobjects.emplace_back(*itr);

	Value->tag = GO_tag::Cylinder;
	Value->name = go_name;
	Value->transform = std::make_shared<Transform>();

	Value->renderer = Value->addComponent<Mesh_renderer>();

	std::vector<Mesh::mesh>* meshes = nullptr;
	ResourceManager::CreateModelFromFBX(&meshes, "./DefaultModel/cylinder.fbx");
	Value->renderer->set_meshes(meshes);

	Value->initialize();
	++go_count;
	return Value;
}

Gameobject* Gameobject_manager::createPlane(const std::string go_name, Scenelist Sce) {
	Gameobject* null = nullptr;
	gameobjects[Sce].emplace_back(null);
	auto itr = gameobjects[Sce].end();
	itr--;
	*itr = newD Gameobject(false, Sce, itr);
	Gameobject* Value = *itr;
	added_gameobjects.emplace_back(*itr);

	Value->tag = GO_tag::Plane;
	Value->name = go_name;
	Value->transform = std::make_shared<Transform>();

	Value->renderer = Value->addComponent<Sprite_renderer>();

	//std::vector<Mesh::mesh>* meshes = nullptr;
	//ResourceManager::CreateModelFromFBX(&meshes, "./DefaultModel/plane.fbx", "");
	//renderer->set_meshes(meshes);

	++go_count;
	Value->initialize();
	return Value;
}
