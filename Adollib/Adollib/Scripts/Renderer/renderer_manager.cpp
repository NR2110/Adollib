
#include "renderer_manager.h"

#include "renderer_base.h"

#include "Shader/constant_buffer.h"
#include "../Main/systems.h"
#include "../Object/gameobject.h"
#include "../Object/component_camera.h"
#include "../Object/component_light.h"
#include "../Physics/ALP__physics_manager.h"

#include "../Imgui/work_meter.h"
#include "../Imgui/imgui_manager.h"

using namespace Adollib;
using namespace ConstantBuffer;

std::unordered_map<Scenelist, std::list<Renderer_base*>> Renderer_manager::renderers;

ComPtr<ID3D11Buffer> Renderer_manager::light_cb;

void Renderer_manager::awake() {

	Systems::CreateConstantBuffer(&light_cb, sizeof(ConstantBufferPerLight));
}

std::list<Renderer_base*>::iterator Renderer_manager::add_renderer(Renderer_base* renderer) {
	const auto& Sce = renderer->gameobject->get_scene();
	renderers[Sce].emplace_back(renderer);

	auto itr = renderers[Sce].end();
	--itr;

	return itr;
};

void Renderer_manager::remove_renderer(std::list<Renderer_base*>::iterator itr) {
	renderers[(*itr)->gameobject->get_scene()].erase(itr);
}

void Renderer_manager::render(const std::map<Scenelist, std::list<Camera_component*>>& cameras,const std::map<Scenelist, std::list<Light_component*>>& lights, Scenelist Sce) {

	Work_meter::start(std::string("Renderer_manager"));

	Work_meter::tag_start("render");

	Work_meter::start(std::string("Systems::Clear"));
	// メインのRTVのclear
	Systems::Clear();
	Work_meter::stop(std::string("Systems::Clear"));

	// Imguiがgpuの処理を全部待ちやがる & depthで上書きしちゃうので 前フレームのtextureを描画して Imguiの描画をここで行う
	// camera->clearにgpu負荷があり、Imguiがその処理を待つので 下に置くと重くなる
	{
		Work_meter::start(std::string("posteffect_render"));
		for (const auto& camera : cameras.at(Sce)) {
			// posteffectの処理 & mainのRTVに描画
			camera->posteffect_render();
		}
		Work_meter::stop(std::string("posteffect_render"));

		Work_meter::start(std::string("renderImgui"));
		Adollib::Imgui_manager::render();
		Work_meter::stop(std::string("renderImgui"));
	}

	// 三角形の描画方法
	Systems::DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	if (Sce == Scenelist::scene_null)return;

	Work_meter::start(std::string("set_light_Constantbuffer"));
	// light情報をコンスタントバッファーにセットする
	set_light_Constantbuffer(lights.at(Sce));
	Work_meter::stop(std::string("set_light_Constantbuffer"));

	//そのシーンのカメラの数だけ回す
	for (const auto& camera : cameras.at(Sce)) {
		if (camera->gameobject->active == false)continue;

		Work_meter::start("calculate_frustum_data");
		auto frustum_data = camera->calculate_frustum_data();
		Work_meter::stop("calculate_frustum_data");

		Work_meter::start("clear");
		// cameraの持つtextureのclear
		camera->clear();
		Work_meter::stop("clear");

		Work_meter::start(std::string("set_Constantbuffer"));
		// camera情報をコンスタントバッファーにセットする
		camera->set_Constantbuffer();
		Work_meter::stop(std::string("set_Constantbuffer"));

		Work_meter::start("render_obj");
		// renderを呼ぶ
		auto render_scene = camera->render_scene; //カメラがrenderするscene
		for (auto& renderer : renderers[render_scene]) {
			renderer->render(frustum_data);
		}
		Work_meter::stop("render_obj");

		Work_meter::start(std::string("render_collider"));
		// colliderのrender
		Physics_function::Physics_manager::render_collider(Sce);
		Work_meter::stop(std::string("render_collider"));
	}


	Work_meter::tag_stop();

	Work_meter::stop(std::string("Renderer_manager"));
}

void Renderer_manager::set_light_Constantbuffer(const std::list<Light_component*>& lights) {
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
		for (const auto& light : lights) {
			for (u_int o = 0; o < light->PointLight.size(); o++) {
				if (light->gameobject->active == false)return;
				PointLight[point_num] = *light->PointLight[o];
				//	PointLight[point_num].pos = (*itr_li->get()->PointLight[o]->pos )+( *itr_li->get()->transform->position);
				point_num++;
			}

			for (u_int o = 0; o < light->SpotLight.size(); o++) {
				if (light->gameobject->active == false)return;
				SpotLight[spot_num] = *light->SpotLight[o];
				spot_num++;
			}
		}
	}

	memcpy(l_cb.PointLight, PointLight, sizeof(POINTLIGHT) * POINTMAX);
	memcpy(l_cb.SpotLight,  SpotLight, sizeof(SPOTLIGHT) * SPOTMAX);
	Systems::DeviceContext->UpdateSubresource(light_cb.Get(), 0, NULL, &l_cb, 0, 0);
	Systems::DeviceContext->VSSetConstantBuffers(4, 1, light_cb.GetAddressOf());
	Systems::DeviceContext->PSSetConstantBuffers(4, 1, light_cb.GetAddressOf());
}