
#include "systems.h"
#include "gameobject_manager.h"
#include "rigit_body_manager.h"
#include "material_for_collider.h"
#include "scene.h"
#include "resource_manager.h"
#include "cbuffer_manager.h"
#include "light_types.h"
namespace Adollib {

	std::map<Scenelist, std::list<std::shared_ptr<Gameobject>>> Gameobject_manager::gameobjects;
	std::map<Scenelist, std::list<std::shared_ptr<Light>>> Gameobject_manager::lights;
	std::map<Scenelist, std::list<std::shared_ptr<Camera>>> Gameobject_manager::cameras;
	vector<object*> Gameobject_manager::masters; //GO親子ツリーの頂点を保存

	ComPtr<ID3D11Buffer> Gameobject_manager::light_cb;
	ComPtr<ID3D11Buffer> Gameobject_manager::view_cb;
	ComPtr<ID3D11Buffer> Gameobject_manager::projection_cb;

	void Gameobject_manager::awake() {

		Systems::CreateConstantBuffer(&light_cb, sizeof(ConstantBufferPerLight));
		Systems::CreateConstantBuffer(&view_cb, sizeof(ConstantBufferPerCamera));
		Systems::CreateConstantBuffer(&projection_cb, sizeof(ConstantBufferPerSystem));

		Systems::SetDephtStencilState(State_manager::DStypes::DS_TRUE);
		Systems::SetRasterizerState  (State_manager::RStypes::RS_CULL_BACK);
		Systems::SetBlendState       (State_manager::BStypes::BS_ADD);

		//CB : ConstantBufferPerSystem
		ConstantBufferPerSystem s_sb;
		float fov = ToRadian(60);
		float aspect = (FLOAT)Al_Global::SCREEN_WIDTH / (FLOAT)Al_Global::SCREEN_HEIGHT;
		float nearZ = 0.1f;
		float farZ = 10000.0f;
		DirectX::XMStoreFloat4x4(&s_sb.Projection, DirectX::XMMatrixPerspectiveFovLH(fov, aspect, nearZ, farZ));
		Systems::DeviceContext->UpdateSubresource(projection_cb.Get(), 0, NULL, &s_sb, 0, 0);
		Systems::DeviceContext->VSSetConstantBuffers(2, 1, projection_cb.GetAddressOf());
		Systems::DeviceContext->PSSetConstantBuffers(2, 1, projection_cb.GetAddressOf());

		//sceneの数だけgo_managerを生成
		for (int i = 0; i < static_cast<int>(Scenelist::scene_list_size); i++) {
			std::list<std::shared_ptr<Gameobject>> go_manager;
			gameobjects[static_cast<Scenelist>(i)] = go_manager;

			std::list<std::shared_ptr<Light>> li_manager;
			lights[static_cast<Scenelist>(i)] = li_manager;

			std::list<std::shared_ptr<Camera>> ca_manager;
			cameras[static_cast<Scenelist>(i)] = ca_manager;
		}

		Collider_renderer::initialize(); 

	}

	void Gameobject_manager::initialize(Scenelist Sce) {
		if (Sce == Scenelist::scene_null)return;

		{
			//Sceのsceneにアタッチされたcomponentのinitializeを呼ぶ
			std::list<std::shared_ptr<Gameobject>>::iterator itr = gameobjects[Sce].begin();
			std::list<std::shared_ptr<Gameobject>>::iterator itr_end = gameobjects[Sce].end();

			for (; itr != itr_end; itr++) {
				itr->get()->initialize();
			}
		}
		{
			//light
			std::list<std::shared_ptr<Light>>::iterator itr = lights[Sce].begin();
			std::list<std::shared_ptr<Light>>::iterator itr_end = lights[Sce].end();

			for (; itr != itr_end; itr++) {
				itr->get()->initialize();
			}
		}
		{
			//camera
			std::list<std::shared_ptr<Camera>>::iterator itr = cameras[Sce].begin();
			std::list<std::shared_ptr<Camera>>::iterator itr_end = cameras[Sce].end();

			for (; itr != itr_end; itr++) {
				itr->get()->initialize();
			}
		}

	}

	void Gameobject_manager::update(Scenelist Sce) {
		if (Sce == Scenelist::scene_null)return;

		std::vector<object*> gos;
		//updateを呼んだかのフラグを消すついでに
		//扱いやすいように一つの配列に保存
		{
			//Sceのsceneにアタッチされたcomponentのupdateを呼ぶ
			std::list<std::shared_ptr<Gameobject>>::iterator itr = gameobjects[Sce].begin();
			std::list<std::shared_ptr<Gameobject>>::iterator itr_end = gameobjects[Sce].end();

			for (; itr != itr_end; itr++) {
				gos.push_back(itr->get());
				itr->get()->updated = false;
				itr->get()->co_e.position = vector3(0, 0, 0);
				itr->get()->co_e.orient = quaternion_identity();
			}
		}
		{
			//light
			std::list<std::shared_ptr<Light>>::iterator itr = lights[Sce].begin();
			std::list<std::shared_ptr<Light>>::iterator itr_end = lights[Sce].end();

			for (; itr != itr_end; itr++) {
				gos.push_back(itr->get());
				itr->get()->updated = false;
				itr->get()->co_e.position = vector3(0, 0, 0);
				itr->get()->co_e.orient = quaternion_identity();
			}
		}
		{
			//camera
			std::list<std::shared_ptr<Camera>>::iterator itr = cameras[Sce].begin();
			std::list<std::shared_ptr<Camera>>::iterator itr_end = cameras[Sce].end();

			for (; itr != itr_end; itr++) {
				gos.push_back(itr->get());
				itr->get()->updated = false;
				itr->get()->co_e.position = vector3(0, 0, 0);
				itr->get()->co_e.orient = quaternion_identity();
			}
		}

		//ツリーの親を更新
		masters.clear();
		for (int i = 0; i < gos.size(); i++) {
			object* master =  gos[i]->get_pearent();
			if (master->updated == true)continue;
			master->updated = true;
			masters.push_back(master);
		}

		//親から子へupdateを呼ぶ
		for (int i = 0; i < masters.size(); i++) {
			masters[i]->update_P_to_C();
		}
		Rigitbody_manager::update();

		{
			//Sceのsceneにアタッチされたcomponentのupdateを呼ぶ
			std::list<std::shared_ptr<Gameobject>>::iterator itr = gameobjects[Sce].begin();
			std::list<std::shared_ptr<Gameobject>>::iterator itr_end = gameobjects[Sce].end();

			for (; itr != itr_end; itr++) {
				itr->get()->transform->local_pos += itr->get()->co_e.position;
				itr->get()->transform->local_orient *= itr->get()->co_e.orient;

			}
		}

		//親から子へワールド情報を更新
		for (int i = 0; i < masters.size(); i++) {
			masters[i]->update_world_trans();
		}


	}

	void Gameobject_manager::render(Scenelist Sce) {
		if (Sce == Scenelist::scene_null)return;

		std::list<std::shared_ptr<Light>>::iterator itr_li = lights[Sce].begin();
		std::list<std::shared_ptr<Camera>>::iterator itr_ca = cameras[Sce].begin();

		//CB : ConstantBufferPerLight
		ConstantBufferPerLight l_cb;
		l_cb.AmbientColor = Ambient.get_XM4();
		l_cb.LightDir = LightDir.get_XM4();
		l_cb.LightColor = DirLightColor.get_XM4();

		//コンスタントバッファに渡すためにpointlight,spotlightの配列を整理
		POINTLIGHT PointLight[POINTMAX];
		SPOTLIGHT SpotLight[SPOTMAX];
		{
			int point_num = 0;
			int spot_num = 0;
			for (int i = 0; i < lights[Sce].size(); i++) {
				for (int o = 0; o < itr_li->get()->PointLight.size(); o++) {
					if (itr_li->get()->active == false)return;
					PointLight[point_num] = *itr_li->get()->PointLight[o];
				//	PointLight[point_num].pos = (*itr_li->get()->PointLight[o]->pos )+( *itr_li->get()->transform->position);
					point_num++;
				}

				for (int o = 0; o < itr_li->get()->SpotLight.size(); o++) {
					if (itr_li->get()->active == false)return;
					SpotLight[spot_num] = *itr_li->get()->SpotLight[o];
					spot_num++;
				}
				itr_li++;
			}
		}

		memcpy(l_cb.PointLight, PointLight, sizeof(POINTLIGHT) * POINTMAX);
		memcpy(l_cb.SpotLight, SpotLight, sizeof(SPOTLIGHT) * SPOTMAX);
		Systems::DeviceContext->UpdateSubresource(light_cb.Get(), 0, NULL, &l_cb, 0, 0);
		Systems::DeviceContext->VSSetConstantBuffers(3, 1, light_cb.GetAddressOf());
		Systems::DeviceContext->PSSetConstantBuffers(3, 1, light_cb.GetAddressOf());

		//CB : ConstantBufferPerCamera
		ConstantBufferPerCamera c_cb;
		//そのシーンのカメラの数だけ回す
		for (int i = 0; i < cameras[Sce].size(); i++, itr_ca++) {
			if (itr_ca->get()->active == false)return;

			// ビュー行列
			vector3 pos = itr_ca->get()->transform->position;
			quaternion orient = itr_ca->get()->transform->orientation;
			vector3 look_pos = pos + vector3_be_rotated_by_quaternion(vector3(0, 0, 1), orient);

			DirectX::XMVECTOR eye = DirectX::XMVectorSet(pos.x, pos.y, pos.z, 1.0f);
			DirectX::XMVECTOR focus = DirectX::XMVectorSet(look_pos.x, look_pos.y, look_pos.z, 1.0f);
			DirectX::XMVECTOR up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f);
			XMStoreFloat4x4(&c_cb.View, DirectX::XMMatrixLookAtLH(eye, focus, up));
			c_cb.Eyepos = DirectX::XMFLOAT4(pos.x, pos.y, pos.z, 1.0f);
			Systems::DeviceContext->UpdateSubresource(view_cb.Get(), 0, NULL, &c_cb, 0, 0);
			Systems::DeviceContext->VSSetConstantBuffers(1, 1, view_cb.GetAddressOf());
			Systems::DeviceContext->PSSetConstantBuffers(1, 1, view_cb.GetAddressOf());

			//Sceのsceneにアタッチされたgoのrenderを呼ぶ
			std::list<std::shared_ptr<Gameobject>>::iterator itr = gameobjects[Sce].begin();
			std::list<std::shared_ptr<Gameobject>>::iterator itr_end = gameobjects[Sce].end();

			for (; itr != itr_end; itr++) {
				if (itr->get()->active == false)return;
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

	Light* Gameobject_manager::create_light(Scenelist Sce) {
		std::shared_ptr <Light> Value = std::make_shared<Light>();
		lights[Sce].push_back(Value);

		Value.get()->this_scene = Sce;
		Value.get()->initialize();
		return Value.get();
	}
	Light* Gameobject_manager::create_light(const std::string go_name, Scenelist Sce) {
		std::shared_ptr <Light> Value = std::make_shared<Light>();
		lights[Sce].push_back(Value);
		Value.get()->name = go_name;

		Value.get()->this_scene = Sce;
		Value.get()->initialize();
		return Value.get();
	}

	Camera* Gameobject_manager::create_camera(Scenelist Sce) {
		std::shared_ptr <Camera> Value = std::make_shared<Camera>();
		cameras[Sce].push_back(Value);

		Value.get()->this_scene = Sce;
		Value.get()->transform = new Transfome;
		Value.get()->transform->position = vector3(0, 0, 0);
		Value.get()->initialize();
		return Value.get();
	}
	Camera* Gameobject_manager::create_camera(const std::string go_name, Scenelist Sce) {
		std::shared_ptr <Camera> Value = std::make_shared<Camera>();
		cameras[Sce].push_back(Value);
		Value.get()->name = go_name;

		Value.get()->this_scene = Sce;
		Value.get()->transform = new Transfome;
		Value.get()->initialize();
		return Value.get();
	}

	Gameobject* Gameobject_manager::create(Scenelist Sce) {
		std::shared_ptr <Gameobject> Value = std::make_shared<Gameobject>();
		gameobjects[Sce].push_back(Value);
		Value.get()->name = "no_name";

		Value.get()->this_scene = Sce;
		Value.get()->transform = new Transfome;
		Value.get()->no_material = true;

		Value.get()->initialize();
		return Value.get();


	}
	Gameobject* Gameobject_manager::create(const std::string go_name, Scenelist Sce) {
		std::shared_ptr <Gameobject> Value = std::make_shared<Gameobject>();
		gameobjects[Sce].push_back(Value);

		Value.get()->name = go_name;
		Value.get()->this_scene = Sce;
		Value.get()->no_material = true;

		Value.get()->transform = new Transfome;
		Value.get()->initialize();
		return Value.get();
	}

	Gameobject* Gameobject_manager::createFromFBX(const std::string& FBX_pass, const std::string& go_name, Scenelist Sce) {
		std::shared_ptr <Gameobject> Value = std::make_shared<Gameobject>();
		gameobjects[Sce].push_back(Value);
		Value.get()->name = go_name;

		Value.get()->this_scene = Sce;
		Value.get()->transform = new Transfome;

		Value.get()->material = new Material;
		Value.get()->material->Load_VS("./DefaultShader/default_vs.cso");
		Value.get()->material->Load_PS("./DefaultShader/default_ps.cso");
		ResourceManager::CreateModelFromFBX(&Value.get()->material->meshes, FBX_pass.c_str(), "");
		Value.get()->initialize();
		return Value.get();
	}

	Gameobject* Gameobject_manager::createSphere(const std::string& go_name,  Scenelist Sce) {
		std::shared_ptr <Gameobject> Value = std::make_shared<Gameobject>();
		gameobjects[Sce].push_back(Value);
		Value.get()->name = go_name;

		Value.get()->this_scene = Sce;
		Value.get()->transform = new Transfome;

		Value.get()->material = new Material;
		Value.get()->material->Load_VS("./DefaultShader/default_vs.cso");
		Value.get()->material->Load_PS("./DefaultShader/default_ps.cso");
		ResourceManager::CreateModelFromFBX(&Value.get()->material->meshes, "./DefaultModel/sphere.fbx", "");
		Value.get()->initialize();
		return Value.get();
	}

	Gameobject* Gameobject_manager::createCube(const std::string& go_name, Scenelist Sce) {
		std::shared_ptr <Gameobject> Value = std::make_shared<Gameobject>();
		gameobjects[Sce].push_back(Value);
		Value.get()->name = go_name;

		Value.get()->this_scene = Sce;
		Value.get()->transform = new Transfome;

		Value.get()->material = new Material;
		Value.get()->material->Load_VS("./DefaultShader/default_vs.cso");
		Value.get()->material->Load_PS("./DefaultShader/default_ps.cso");
		ResourceManager::CreateModelFromFBX(&Value.get()->material->meshes, "./DefaultModel/cube.fbx", "");
		Value.get()->initialize();
		return Value.get();
	}
	//Gameobject* Gameobject_manager::createCylinder(const std::string& go_name) {

	//}
	Gameobject* Gameobject_manager::createCylinder(const std::string& go_name, Scenelist Sce) {
		std::shared_ptr <Gameobject> Value = std::make_shared<Gameobject>();
		gameobjects[Sce].push_back(Value);
		Value.get()->name = go_name;

		Value.get()->this_scene = Sce;
		Value.get()->transform = new Transfome;

		Value.get()->material = new Material;
		Value.get()->material->Load_VS("./DefaultShader/default_vs.cso");
		Value.get()->material->Load_PS("./DefaultShader/default_ps.cso");
		ResourceManager::CreateModelFromFBX(&Value.get()->material->meshes, "./DefaultModel/cylinder.fbx", "");
		Value.get()->initialize();
		return Value.get();
	}
	//Gameobject* Gameobject_manager::createPlane(const std::string& go_name) {

	//}
}