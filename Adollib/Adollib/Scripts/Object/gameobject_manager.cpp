
#include "gameobject_manager.h"

#include "../Main/systems.h"

#include "../Scene/scene.h"
#include "../Main/resource_manager.h"
#include "../Shader/cbuffer_manager.h"
#include "../Shader/light_types.h"
#include "../Physics/ALP__physics_manager.h"

#include "../Mesh/frustum_culling.h"
#include "../Mesh/material_for_collider.h"

#include "../Shader/constant_buffer.h"
#include "../Physics/ALP__physics_manager.h"

#include "hierarchy.h"

#include "../Imgui/work_meter.h"

#include <future>

using namespace Adollib;
using namespace Physics_function;
using namespace ConstantBuffer;

std::map<Scenelist, std::list<Gameobject*>> Gameobject_manager::gameobjects;
std::map<Scenelist, std::list<Light*>> Gameobject_manager::lights;
std::map<Scenelist, std::list<Camera*>> Gameobject_manager::cameras;
std::vector<Gameobject*>	Gameobject_manager::save_delete_gameobject;
std::vector<Camera*>		Gameobject_manager::save_delete_camera;
std::vector<Light*>			Gameobject_manager::save_delete_light;

int Gameobject_manager::go_count = 0;

ComPtr<ID3D11Buffer> Gameobject_manager::light_cb;
ComPtr<ID3D11Buffer> Gameobject_manager::view_cb;
ComPtr<ID3D11Buffer> Gameobject_manager::projection_cb;

void Gameobject_manager::awake() {

	Systems::CreateConstantBuffer(&light_cb, sizeof(ConstantBufferPerLight));
	Systems::CreateConstantBuffer(&view_cb, sizeof(ConstantBufferPerCamera));
	Systems::CreateConstantBuffer(&projection_cb, sizeof(ConstantBufferPerSystem));

	Systems::SetDephtStencilState(State_manager::DStypes::DS_TRUE);
	Systems::SetRasterizerState(State_manager::RStypes::RS_CULL_BACK);
	Systems::SetBlendState(State_manager::BStypes::BS_ADD);

	//sceneの数だけgo_managerを生成
	for (int i = 0; i < static_cast<int>(Scenelist::scene_list_size); i++) {
		std::list<Gameobject*> go_manager;
		gameobjects[static_cast<Scenelist>(i)] = go_manager;

		std::list<Light*> li_manager;
		lights[static_cast<Scenelist>(i)] = li_manager;

		std::list<Camera*> ca_manager;
		cameras[static_cast<Scenelist>(i)] = ca_manager;
	}

	Physics_function::Collider_renderer::initialize();

}

void Gameobject_manager::initialize(Scenelist Sce) {
	if (Sce == Scenelist::scene_null)return;


	for (auto& GO : gameobjects[Sce]) {
		GO->initialize();
	}
	for (auto& GO : lights[Sce]) {
		GO->initialize();
	}
	for (auto& GO : cameras[Sce]) {
		GO->initialize();
	}

}

void Gameobject_manager::update(Scenelist Sce) {
	if (Sce == Scenelist::scene_null)return;

	std::vector<object*> gos;

	//扱いやすいように一つの配列に保存
	for (auto& GO : gameobjects[Sce]) {
		gos.push_back(GO);
	}

	for (auto& GO : lights[Sce]) {
		gos.push_back(GO);
	}

	for (auto& GO : cameras[Sce]) {
		gos.push_back(GO);
	}


	//一番上のの親を保存
	std::vector<object*> masters; //GO親子ツリーの頂点を保存
	masters.clear();
	{
		std::unordered_map<object*, bool> masters_manag;
		for (auto& GO : gos) {
			object* master = GO->top_pearent();
			if (masters_manag.count(master) == 0) {
				masters.emplace_back(master);
			}
			masters_manag[master] = true;
		}
	}


	//親から子へupdateを呼ぶ update中に、親objectが削除されたときに対応できないためNG
	std::for_each(masters.begin(), masters.end(), [](object* ob) {ob->update_to_children(); });

	//ヒエラルキー
	Hierarchy::update_hierarchy(masters);

	Phyisics_manager::update();


	//親から子に座標の更新を行う
	{
		std::unordered_map<object*, bool> masters_manag;
		for (auto& GO : gos) {
			object* master = GO->top_pearent();
			if (masters_manag.count(master) == 0) {
				master->update_world_trans_to_children();
			}
			masters_manag[master] = true;
		}
	}

	delete_gameobjects();


}

void Gameobject_manager::render(Scenelist Sce) {
	Work_meter::tag_start("render");
	if (Sce == Scenelist::scene_null)return;

	//std::list<std::shared_ptr<Light>>::iterator itr_li = lights[Sce].begin();

	//CB : ConstantBufferPerLight
	ConstantBufferPerLight l_cb;
	l_cb.AmbientColor = Ambient.get_XM4();
	l_cb.LightDir = LightDir.get_XM4();
	l_cb.LightColor = DirLightColor.get_XM4();

	//コンスタントバッファに渡すためにpointlight,spotlightの配列を整理
	POINTLIGHT PointLight[POINTMAX] = { 0 };
	SPOTLIGHT SpotLight[SPOTMAX] = { 0 };
	{
		int point_num = 0;
		int spot_num = 0;
		for (const auto& light : lights[Sce]) {
			for (u_int o = 0; o < light->PointLight.size(); o++) {
				if (light->active == false)return;
				PointLight[point_num] = *light->PointLight[o];
				//	PointLight[point_num].pos = (*itr_li->get()->PointLight[o]->pos )+( *itr_li->get()->transform->position);
				point_num++;
			}

			for (u_int o = 0; o < light->SpotLight.size(); o++) {
				if (light->active == false)return;
				SpotLight[spot_num] = *light->SpotLight[o];
				spot_num++;
			}
		}
	}

	memcpy(l_cb.PointLight, PointLight, sizeof(POINTLIGHT) * POINTMAX);
	memcpy(l_cb.SpotLight, SpotLight, sizeof(SPOTLIGHT) * SPOTMAX);
	Systems::DeviceContext->UpdateSubresource(light_cb.Get(), 0, NULL, &l_cb, 0, 0);
	Systems::DeviceContext->VSSetConstantBuffers(4, 1, light_cb.GetAddressOf());
	Systems::DeviceContext->PSSetConstantBuffers(4, 1, light_cb.GetAddressOf());

	ConstantBufferPerCamera c_cb;
	ConstantBufferPerSystem s_sb;
	//そのシーンのカメラの数だけ回す

	Work_meter::start("drawobj_per_camera");
	for (const auto& camera : cameras[Sce]) {
		if (camera->active == false)continue;

		//CB : ConstantBufferPerCamera
		// ビュー行列
		Vector3 pos = camera->transform->position;
		Quaternion orient = camera->transform->orientation;
		Vector3 look_pos = pos + vector3_quatrotate(Vector3(0, 0, 1), orient);

		DirectX::XMVECTOR eye = DirectX::XMVectorSet(pos.x, pos.y, pos.z, 1.0f);
		DirectX::XMVECTOR focus = DirectX::XMVectorSet(look_pos.x, look_pos.y, look_pos.z, 1.0f);
		DirectX::XMVECTOR up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f);
		XMStoreFloat4x4(&c_cb.View, DirectX::XMMatrixLookAtLH(eye, focus, up));
		c_cb.Eyepos = DirectX::XMFLOAT4(pos.x, pos.y, pos.z, 1.0f);
		Systems::DeviceContext->UpdateSubresource(view_cb.Get(), 0, NULL, &c_cb, 0, 0);
		Systems::DeviceContext->VSSetConstantBuffers(1, 1, view_cb.GetAddressOf());
		Systems::DeviceContext->PSSetConstantBuffers(1, 1, view_cb.GetAddressOf());

		//CB : ConstantBufferPerSystem
		float fov = ToRadian(camera->fov);
		float aspect = camera->aspect;
		float nearZ = camera->nearZ;
		float farZ = camera->farZ;
		DirectX::XMStoreFloat4x4(&s_sb.Projection, DirectX::XMMatrixPerspectiveFovLH(fov, aspect, nearZ, farZ));
		Systems::DeviceContext->UpdateSubresource(projection_cb.Get(), 0, NULL, &s_sb, 0, 0);
		Systems::DeviceContext->VSSetConstantBuffers(2, 1, projection_cb.GetAddressOf());
		Systems::DeviceContext->PSSetConstantBuffers(2, 1, projection_cb.GetAddressOf());


		//視錐台カリングにカメラ情報のセット
		FrustumCulling::update_frustum(camera);

		//Sceのsceneにアタッチされたgoのrenderを呼ぶ

		Work_meter::start("render_obj");
		for (auto& go : gameobjects[Sce]) {
			if (go->active == false)continue;
			go->render();
		}
		Work_meter::stop("render_obj");


		Phyisics_manager::render_collider(Sce);
	}
	Work_meter::stop("drawobj_per_camera");

	Work_meter::tag_stop();
}





void Gameobject_manager::destroy(Scenelist Sce) {
	if (Sce == Scenelist::scene_null)return;

	//終了処理
	for (auto& GO : gameobjects[Sce]) {
		GO->clearComponent();
		delete GO;
	}

	for (auto& GO : cameras[Sce]) {
		GO->clearComponent();
		delete GO;
	}

	for (auto& GO : lights[Sce]) {
		GO->clearComponent();
		delete GO;
	}

	//適当にSceのsceneにアタッチされたgoを削除
	gameobjects[Sce].clear();
	lights[Sce].clear();
	cameras[Sce].clear();

}

Light* Gameobject_manager::create_light(const std::string go_name, Scenelist Sce) {
	Light* null = nullptr;
	lights[Sce].emplace_back(null);
	auto itr = lights[Sce].end();
	itr--;
	*itr = newD Light(Sce,itr);
	Light* Value = *itr;

	Value->name = go_name;

	Value->initialize();
	//Value->name = std::string("light");
	return Value;
}

Camera* Gameobject_manager::create_camera(const std::string go_name, Scenelist Sce) {
	Camera* null = nullptr;
	cameras[Sce].emplace_back(null);
	auto itr = cameras[Sce].end();
	itr--;
	*itr = newD Camera(Sce, itr);
	Camera* Value = *itr;

	Value->name = go_name;
	Value->transform = std::make_shared<Transfome>();

	Value->initialize();
	return Value;
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
	Value->transform = std::make_shared<Transfome>();

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
	Value->transform = std::make_shared<Transfome>();

	Value->material = std::make_shared<Material>();
	Value->material->Load_VS("./DefaultShader/default_vs.cso");
	Value->material->Load_PS("./DefaultShader/default_ps.cso");
	ResourceManager::CreateModelFromFBX(&Value->material->meshes, FBX_pass.c_str(), "");
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
	Value->transform = std::make_shared<Transfome>();

	Value->material = std::make_shared<Material>();
	Value->material->Load_VS("./DefaultShader/default_vs.cso");
	Value->material->Load_PS("./DefaultShader/default_ps.cso");
	ResourceManager::CreateModelFromFBX(&Value->material->meshes, "./DefaultModel/sphere.fbx", "");
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
	Value->transform = std::make_shared<Transfome>();

	Value->material = std::make_shared<Material>();
	Value->material->Load_VS("./DefaultShader/default_vs.cso");
	Value->material->Load_PS("./DefaultShader/default_ps.cso");
	ResourceManager::CreateModelFromFBX(&Value->material->meshes, "./DefaultModel/cube.fbx", "");
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
	Value->transform = std::make_shared<Transfome>();

	Value->material = std::make_shared<Material>();
	Value->material->Load_VS("./DefaultShader/default_vs.cso");
	Value->material->Load_PS("./DefaultShader/default_ps.cso");
	ResourceManager::CreateModelFromFBX(&Value->material->meshes, "./DefaultModel/cube.fbx", "");
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
	Value->transform = std::make_shared<Transfome>();

	Value->material = std::make_shared<Material>();
	Value->material->Load_VS("./DefaultShader/default_vs.cso");
	Value->material->Load_PS("./DefaultShader/default_ps.cso");
	ResourceManager::CreateModelFromFBX(&Value->material->meshes, "./DefaultModel/cylinder.fbx", "");
	Value->initialize();
	++go_count;
	return Value;
}
//Gameobject* Gameobject_manager::createPlane( u_int go_name) {

//}
