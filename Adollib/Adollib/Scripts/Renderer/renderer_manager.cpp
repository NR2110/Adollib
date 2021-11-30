
#include "renderer_manager.h"

#include "renderer_base.h"
#include "directional_shadow.h"
#include "sky_sphere.h"
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

	Work_meter::tag_start("render");

	// メインのRTVのclear
	Systems::Clear();

	// Imguiがgpuの処理を全部待ちやがる & depthで上書きしちゃうので 前フレームのtextureを描画して Imguiの描画をここで行う
	// camera->clearにgpu負荷があり、Imguiがその処理を待つので 下に置くと重くなる
	{
		for (const auto& camera : cameras.at(Sce)) {
			// posteffectの処理 & mainのRTVに描画
			camera->posteffect_render();
		}

		Adollib::Imgui_manager::render();
	}

	// 三角形の描画方法
	Systems::DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	if (Sce == Scenelist::scene_null)return;

	// light情報をコンスタントバッファーにセットする
	set_light_Constantbuffer(lights.at(Sce));

	//そのシーンのカメラの数だけ回す
	for (const auto& camera : cameras.at(Sce)) {
		if (camera->gameobject->active == false)continue;

		const auto& frustum_data = camera->calculate_frustum_data();
		const auto& render_scene = camera->render_scene; //カメラがrenderするscene

		// 影用の描画
		{
			Systems::SetViewPort(10000, 10000);
			camera->directional_shadow->setup();
			camera->directional_shadow->shader_activate();
			auto shadow_frustum = camera->directional_shadow->calculate_frustum_data();
 			for (auto& renderer : renderers[render_scene]) {
				renderer->render(shadow_frustum);
			}
			camera->directional_shadow->set_ShaderResourceView();


		}



		Systems::SetViewPort(Al_Global::SCREEN_WIDTH, Al_Global::SCREEN_HEIGHT);

		// cameraの持つtextureのclear
		camera->clear();

		// camera情報を準備(constantbuffer,rendertargetview)
		camera->setup();

		Work_meter::start("render_obj");
		// renderを呼ぶ
		for (auto& renderer : renderers[render_scene]) {
			renderer->shader_activete();
			renderer->render(frustum_data);
		}
		Work_meter::stop("render_obj");

		// sky sphere
		{
			camera->sky_sphere->position = camera->transform->position + Vector3(0,-500,0);
			camera->sky_sphere->rotate = Vector3(0,180,0);
			camera->sky_sphere->fov = camera->fov;
			camera->sky_sphere->aspect = camera->aspect;
			camera->sky_sphere->nearZ = camera->nearZ;
			camera->sky_sphere->farZ = 100000.0f;
			camera->sky_sphere->shader_activate();
			camera->sky_sphere->render();
		}

		// colliderのrender
		Physics_function::Physics_manager::render_collider(Sce);
	}


	Work_meter::tag_stop();
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