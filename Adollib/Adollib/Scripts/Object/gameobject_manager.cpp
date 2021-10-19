
#include "gameobject_manager.h"

#include "component_camera.h"
#include "component_light.h"

#include "../Main/resource_manager.h"
#include "../Main/systems.h"

#include "../Scene/scene.h"

#include "../Renderer/renderer_manager.h"
#include "../Renderer/renderer.h"
#include "../Renderer/Shader/constant_buffer.h"
#include "../Renderer/Shader/cbuffer_manager.h"
#include "../Renderer/Shader/light_types.h"
#include "../Renderer/Mesh/frustum_culling.h"
#include "../Renderer/Mesh/material_for_collider.h"

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
std::map<Scenelist, std::list<Light_component*>> Gameobject_manager::lights;
std::map<Scenelist, std::list<Camera_component*>> Gameobject_manager::cameras;

std::vector<Gameobject*>	Gameobject_manager::save_delete_gameobject;

int Gameobject_manager::go_count = 0;

void Gameobject_manager::awake() {

	Systems::SetDephtStencilState(State_manager::DStypes::DS_TRUE);
	Systems::SetRasterizerState(State_manager::RStypes::RS_CULL_BACK);
	Systems::SetBlendState(State_manager::BStypes::BS_ADD);

	Renderer_manager::awake();

	//sceneの数だけgo_managerを生成
	for (int i = 0; i < static_cast<int>(Scenelist::scene_list_size); i++) {
		std::list<Gameobject*> go_manager;
		gameobjects[static_cast<Scenelist>(i)] = go_manager;
	}

	Physics_function::Collider_renderer::initialize();

}

void Gameobject_manager::initialize(Scenelist Sce) {
	if (Sce == Scenelist::scene_null)return;


	for (auto& GO : gameobjects[Sce]) {
		GO->initialize();
	}

	static std::thread update_physics_;

#ifdef Use_physics_thread
	Physics_manager::thread_start();

#endif // Use_physics_thread

}

void Gameobject_manager::update(Scenelist Sce) {
	if (Sce == Scenelist::scene_null)return;

	auto& gos = gameobjects[Sce];

	//一番上の親を保存
	std::vector<Object*> masters; //GO親子ツリーの頂点を保存
	masters.clear();
	{
		std::unordered_map<Object*, bool> masters_manag;
		for (auto& GO : gos) {
			Object* master = GO->top_parent();
			if (masters_manag.count(master) == 0) {
				masters.emplace_back(master);
			}
			masters_manag[master] = true;
		}
	}

#ifdef UseImgui
	Physics_manager::update_Gui();

#endif

#ifndef Use_physics_thread
	Physics_manager::update();

#endif // Use_physics_thread

	{
		if (Physics_manager::is_updated_physicsthread)
			Physics_manager::adapt_transform_to_gameobject(Sce);

		//親から子に座標の更新を行う
		{
			std::unordered_map<Object*, bool> masters_manag;
			for (auto& GO : gos) {
				Object* master = GO->top_parent();
				if (masters_manag.count(master) == 0) {
					master->update_world_trans_to_children();
				}
				masters_manag[master] = true;
			}

			Physics_manager::is_updated_mainthread = true;
			//Physics_manager::is_updated_physicsthread = true;
		}
	}

	//親から子へupdateを呼ぶ update中に、親objectが削除されたときに対応できないため削除はいったんbufferに保管している
	std::for_each(masters.begin(), masters.end(), [](Object* ob) {ob->update_to_children(); });

	//親から子に座標の更新を行う
	{
		std::unordered_map<Object*, bool> masters_manag;

		for (auto& GO : gos) {
			Object* master = GO->top_parent();
			if (masters_manag.count(master) == 0) {
				master->update_world_trans_to_children();
			}
			masters_manag[master] = true;
		}

	}


#ifdef UseImgui
	//ヒエラルキー
	Hierarchy::update_hierarchy(masters);

#endif // UseImgui

	//親から子に座標の更新を行う
	{
		std::unordered_map<Object*, bool> masters_manag;

		for (auto& GO : gos) {
			Object* master = GO->top_parent();
			if (masters_manag.count(master) == 0) {
				master->update_world_trans_to_children();
			}
			masters_manag[master] = true;
		}

	}

	delete_gameobjects();

}

void Gameobject_manager::render(Scenelist Sce) {
	Renderer_manager::render(cameras[Sce], lights[Sce], Sce);
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

	Value->tag = tag;
	Value->name = go_name;
	Value->transform = std::make_shared<Transform>();

	Value->initialize();
	++go_count;
	return Value;
}


Gameobject* Gameobject_manager::createFromFBX(const std::string go_name, const std::string& FBX_pass, u_int tag, Scenelist Sce) {
	Gameobject* null = nullptr;
	gameobjects[Sce].emplace_back(null);
	auto itr = gameobjects[Sce].end();
	itr--;
	*itr = newD Gameobject(false, Sce, itr);
	Gameobject* Value = *itr;

	Value->tag = tag;
	Value->name = go_name;
	Value->transform = std::make_shared<Transform>();

	auto renderer = Value->addComponent<Renderer>();
	Value->material = newD Material;
	Value->material->Load_VS("./DefaultShader/default_vs.cso");
	Value->material->Load_PS("./DefaultShader/default_ps.cso");
	renderer->set_material(Value->material);

	std::vector<Mesh::mesh>* meshes = nullptr;
	ResourceManager::CreateModelFromFBX(&meshes, FBX_pass.c_str(), "");
	renderer->set_meshes(meshes);

	Value->initialize();
	++go_count;
	return Value;
}

Gameobject* Gameobject_manager::createSphere(const std::string go_name, u_int tag, Scenelist Sce) {
	Gameobject* null = nullptr;
	gameobjects[Sce].emplace_back(null);
	auto itr = gameobjects[Sce].end();
	itr--;
	*itr = newD Gameobject(false, Sce, itr);
	Gameobject* Value = *itr;

	Value->tag = tag;
	Value->name = go_name;
	Value->transform = std::make_shared<Transform>();

	auto renderer = Value->addComponent<Renderer>();
	Value->material = newD Material;
	Value->material->Load_VS("./DefaultShader/default_vs.cso");
	Value->material->Load_PS("./DefaultShader/default_ps.cso");
	renderer->set_material(Value->material);

	std::vector<Mesh::mesh>* meshes = nullptr;
	ResourceManager::CreateModelFromFBX(&meshes, "./DefaultModel/sphere.fbx", "");
	renderer->set_meshes(meshes);

	Value->initialize();
	++go_count;
	return Value;
}

Gameobject* Gameobject_manager::createCube(const std::string go_name, u_int tag, Scenelist Sce) {
	Gameobject* null = nullptr;
	gameobjects[Sce].emplace_back(null);
	auto itr = gameobjects[Sce].end();
	itr--;
	*itr = newD Gameobject(false, Sce, itr);
	Gameobject* Value = *itr;

	Value->tag = tag;
	Value->name = go_name;
	Value->transform = std::make_shared<Transform>();

	auto renderer = Value->addComponent<Renderer>();
	Value->material = newD Material;
	Value->material->Load_VS("./DefaultShader/default_vs.cso");
	Value->material->Load_PS("./DefaultShader/default_ps.cso");
	renderer->set_material(Value->material);

	std::vector<Mesh::mesh>* meshes = nullptr;
	ResourceManager::CreateModelFromFBX(&meshes, "./DefaultModel/cube.fbx", "");
	renderer->set_meshes(meshes);

	++go_count;
	Value->initialize();
	return Value;
}

Gameobject* Gameobject_manager::createCapsule(const std::string go_name, u_int tag, Scenelist Sce) {
	Gameobject* null = nullptr;
	gameobjects[Sce].emplace_back(null);
	auto itr = gameobjects[Sce].end();
	itr--;
	*itr = newD Gameobject(false, Sce, itr);
	Gameobject* Value = *itr;

	Value->tag = tag;
	Value->name = go_name;
	Value->transform = std::make_shared<Transform>();

	auto renderer = Value->addComponent<Renderer>();
	Value->material = newD Material;
	Value->material->Load_VS("./DefaultShader/default_vs.cso");
	Value->material->Load_PS("./DefaultShader/default_ps.cso");
	renderer->set_material(Value->material);

	std::vector<Mesh::mesh>* meshes = nullptr;
	ResourceManager::CreateModelFromFBX(&meshes, "./DefaultModel/cube.fbx", "");
	renderer->set_meshes(meshes);

	++go_count;
	Value->initialize();
	return Value;
}
//Gameobject* Gameobject_manager::createCylinder( u_int go_name) {

//}
Gameobject* Gameobject_manager::createCylinder(const std::string go_name, u_int tag, Scenelist Sce) {
	Gameobject* null = nullptr;
	gameobjects[Sce].emplace_back(null);
	auto itr = gameobjects[Sce].end();
	itr--;
	*itr = newD Gameobject(false, Sce, itr);
	Gameobject* Value = *itr;

	Value->tag = tag;
	Value->name = go_name;
	Value->transform = std::make_shared<Transform>();

	auto renderer = Value->addComponent<Renderer>();
	Value->material = newD Material;
	Value->material->Load_VS("./DefaultShader/default_vs.cso");
	Value->material->Load_PS("./DefaultShader/default_ps.cso");
	renderer->set_material(Value->material);

	std::vector<Mesh::mesh>* meshes = nullptr;
	ResourceManager::CreateModelFromFBX(&meshes, "./DefaultModel/cylinder.fbx", "");
	renderer->set_meshes(meshes);

	Value->initialize();
	++go_count;
	return Value;
}
//Gameobject* Gameobject_manager::createPlane( u_int go_name) {

//}
