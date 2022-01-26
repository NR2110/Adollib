
#include "renderer_manager.h"

#include "renderer_base.h"
#include "directional_shadow.h"
#include "sky_sphere.h"
#include "material_manager.h"

#include "Shader/constant_buffer.h"
#include "../Main/systems.h"
#include "../Object/gameobject.h"
#include "../Object/component_camera.h"
#include "../Object/component_light.h"
#include "../Physics/ALP__physics_manager.h"

#include "../Imgui/work_meter.h"
#include "../Imgui/imgui_manager.h"
#include "../Imgui/debug.h"

#include <algorithm>

//#define use_instancing_batch

using namespace Adollib;
using namespace ConstantBuffer;

std::unordered_map<Scenelist, std::list<Renderer_base*>> Renderer_manager::renderers;
Microsoft::WRL::ComPtr<ID3D11Buffer> Renderer_manager::instanceBuffer;
std::vector<Renderer_base*> Renderer_manager::sorted_renderers;
std::vector<Render_count> Renderer_manager::render_counts; //instancing描画のための配列 こいつをforで回して描画を行う

ComPtr<ID3D11Buffer> Renderer_manager::light_cb;

void Renderer_manager::awake() {

	Systems::CreateConstantBuffer(light_cb.ReleaseAndGetAddressOf(), sizeof(ConstantBufferPerLight));

	constexpr int maxElements = 10000;
	HRESULT hr = S_OK;
	std::shared_ptr<Instance> instances(new Instance[maxElements]);
	{
		D3D11_BUFFER_DESC bd = {};
		bd.ByteWidth = sizeof(Instance) * maxElements;	// インスタンスバッファのサイズ
		// GPU->読み取りのみ　CPU->書き込みのみ
		bd.Usage = D3D11_USAGE_DYNAMIC;				// バッファの読み書き方法
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;	// バッファのパイプラインへのバインド方式
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;	// リソースに許可されるCPUアクセスの指定
		bd.MiscFlags = 0;
		bd.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA initData = {};
		initData.pSysMem = instances.get();	// 頂点のアドレス
		initData.SysMemPitch = 0;		//Not use for vertex buffers.mm
		initData.SysMemSlicePitch = 0;	//Not use for vertex buffers.
		hr = Systems::Device->CreateBuffer(&bd, &initData, instanceBuffer.ReleaseAndGetAddressOf());
		if (FAILED(hr))
		{
			assert(0 && "failed create instance buffer dynamic(render_manager)");
			return;
		}
	}

	auto mat = Material_manager::create_material("default_material");

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

void Renderer_manager::render(std::list<Scenelist> active_scenes, const std::map<Scenelist, std::list<Camera_component*>>& cameras, const std::map<Scenelist, std::list<Light_component*>>& lights) {

	Work_meter::tag_start("render");

	// メインのRTVのclear
	//Systems::Clear(); //全部上書きするからしなくてよい

	// Imguiがgpuの処理を全部待ちやがる & depthで上書きしちゃうので 前フレームのtextureを描画して Imguiの描画をここで行う
	// camera->clearにgpu負荷があり、Imguiがその処理を待つので 下に置くと重くなる
	{
		for (const auto& Sce : active_scenes) {
			if (cameras.count(Sce) == 0)continue;
			for (const auto& camera : cameras.at(Sce)) {
				// posteffectの処理 & mainのRTVに描画
				camera->posteffect_render();
			}
		}
	}

	// 三角形の描画方法
	Systems::DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//if (Sce == Scenelist::scene_null)return;

	// バッチ用に現在のシーンのmesh,materialのsortを行う
	sort_update(active_scenes);

	for (const auto& Sce : active_scenes) {

		// light情報をコンスタントバッファーにセットする
		if (lights.count(Sce) != 0)
			set_light_Constantbuffer(lights.at(Sce));


		//そのシーンのカメラの数だけ回す
		if (cameras.count(Sce) != 0)
			for (const auto& camera : cameras.at(Sce)) {
				if (camera->gameobject->is_active == false)continue;

				const auto& frustum_data = camera->calculate_frustum_data();
				const auto& render_scene = camera->render_scene; //カメラがrenderするscene


				// カメラの存在するシーンをカメラが描画するシーンが違えばupdateをしていないので updateを呼ぶ
				//if (Sce != render_scene) sort_update(Sce);

				// 影用の描画
				{
					camera->directional_shadow->setup();
					camera->directional_shadow->shader_activate();
					auto shadow_frustum = camera->directional_shadow->calculate_frustum_data();

					// instansing用
					instance_update(shadow_frustum, render_scene); //frastumによって調整するため

					render_instance(true, true);

					camera->directional_shadow->set_ShaderResourceView();
				}

				{
					Systems::SetViewPort(Al_Global::SCREEN_WIDTH, Al_Global::SCREEN_HEIGHT);

					// cameraの持つtextureのclear
					camera->clear();

					// camera情報を準備(constantbuffer,rendertargetview)
					camera->setup();

					Work_meter::start("render_obj");

					// instansing用
					instance_update(frustum_data, render_scene);

					// renderを呼ぶ
					render_instance(true);
					Work_meter::stop("render_obj");
				}

				// sky sphere
				{
					camera->sky_sphere->position = camera->transform->position + Vector3(0, -500, 0);
					camera->sky_sphere->rotate = Vector3(0, 180, 0);
					camera->sky_sphere->fov = camera->fov;
					camera->sky_sphere->aspect = camera->aspect;
					camera->sky_sphere->nearZ = camera->nearZ;
					camera->sky_sphere->farZ = 100000.0f;
					camera->sky_sphere->shader_activate();
					camera->sky_sphere->render();
				}

				// colliderのrender
				Physics_function::Physics_manager::render_collider();
			}
	}

	Work_meter::tag_stop();
}


// Queueソート用
bool sortmesh(const Renderer_base* left, const Renderer_base* Right)
{
	return left->get_meshnum() < Right->get_meshnum();
}
bool sortmaterial(const Renderer_base* left, const Renderer_base* Right)
{
	return left->get_materialnum() < Right->get_materialnum();
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
				if (light->gameobject->is_active == false)return;
				PointLight[point_num] = *light->PointLight[o];
				//	PointLight[point_num].pos = (*itr_li->get()->PointLight[o]->pos )+( *itr_li->get()->transform->position);
				point_num++;
			}

			for (u_int o = 0; o < light->SpotLight.size(); o++) {
				if (light->gameobject->is_active == false)return;
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

}

void Renderer_manager::sort_update(std::list<Scenelist> active_scenes) {

	//if (renderers[Sce].size() == 0)return;
	int scene_size = 0;
	for (const auto& Sce : active_scenes) scene_size += renderers[Sce].size();

	sorted_renderers.clear();
	sorted_renderers.reserve(scene_size);

	for (const auto& Sce : active_scenes) {
		for (auto& renderer : renderers[Sce]) {
			sorted_renderers.emplace_back(renderer);
		}
	}

	// meshでsort
	{
		auto begin = sorted_renderers.begin();
		auto end = sorted_renderers.end();
		// insert_sortが良いと思うがめんどくさいのでstd::sortを使用する
		std::sort(begin, end, sortmesh);
	}

	// materialでソート
	{
		auto itr = sorted_renderers.begin();
		auto itr_end = sorted_renderers.end();

		auto sort_itr_begin = itr;
		auto sort_itr_end = itr;

		long mesh_num = (*itr)->get_meshnum();
		for (itr = sorted_renderers.begin(); itr != itr_end; ++itr) {

			// 同じmeshでないところを見つけたらその範囲をsort
			if ((*itr)->get_meshnum() != mesh_num) {
				sort_itr_end = itr;
				//--sort_itr_end;
				std::sort(sort_itr_begin, sort_itr_end, sortmaterial);

				sort_itr_begin = itr;
			}
		}

		// 最後のmesh内は上記のforでsortに引っかからないのでここでsort
		{
			sort_itr_end = itr;
			std::sort(sort_itr_begin, sort_itr_end, sortmaterial);
		}
	}
}

void Renderer_manager::instance_update(const Frustum_data& frustum_data, Scenelist Sce) {

	Instance* instances = nullptr;
	if (sorted_renderers.size() == 0)return;

	// instance_bufferをMapする
	HRESULT hr = S_OK;
	const D3D11_MAP map = D3D11_MAP_WRITE_DISCARD;
	D3D11_MAPPED_SUBRESOURCE mappedBuffer = {};
	hr = Systems::DeviceContext->Map(instanceBuffer.Get(), 0, map, 0, &mappedBuffer);

	if (FAILED(hr))
	{
		assert(0 && "failed Map InstanceBuffer dynamic(RenderManager)");
		return;
	}
	instances = static_cast<Instance*>(mappedBuffer.pData);

	// rendererの情報から
	{
		long mesh_num = sorted_renderers[0]->get_meshnum();
		long material_num = sorted_renderers[0]->get_materialnum();

		// instancing描画のための配列を初期化
		render_counts.clear();

		Render_count render_count;
		render_count.renderer = sorted_renderers[0];
		render_count.bufferStart = 0;
		render_count.bufferCount = 0;

		std::vector<Renderer_base*> no_use_instancing_renderers;

		for (auto& renderer : sorted_renderers) {

			if (renderer->is_render == false)continue;
			// instancing を行わない場合 別の配列に保存
			if (renderer->is_use_instancing == false) no_use_instancing_renderers.emplace_back(renderer);

			// 新しいinstance
			if (mesh_num != renderer->get_meshnum() ||
				material_num != renderer->get_materialnum()
				) {
				render_counts.emplace_back(render_count);

				render_count.bufferStart = render_count.bufferStart + render_count.bufferCount;
				render_count.bufferCount = 0;
				render_count.renderer = renderer;

				mesh_num = renderer->get_meshnum();
				material_num = renderer->get_materialnum();
			}

			// frustumに入っていたらinstanceに追加
			if (renderer->check_frustum(frustum_data)) {
				// countを増やす
				renderer->set_instance(instances[render_count.bufferStart + render_count.bufferCount]);
				++render_count.bufferCount;
			}

		}
		// 一番最後の要素はfor外で追加する
		render_counts.emplace_back(render_count);

		// instancing を行わない物を個別にrender_countに保存
		{
			for (auto& renderer : no_use_instancing_renderers) {

				if (renderer->check_frustum(frustum_data)) {
					render_count.bufferStart = 0;
					render_count.bufferCount = 1;
					render_count.renderer = renderer;
					render_counts.emplace_back(render_count);
				}
			}
		}
	}

	// instance_bufferをUnmapする
	Systems::DeviceContext->Unmap(instanceBuffer.Get(), 0);

}

void Renderer_manager::render_instance(bool is_shader_activate, bool is_shadow_render) {
	//return;
	for (const auto& render : render_counts) {

		// 影の描画を行わなければcontinue
		if (is_shadow_render && render.renderer->get_material() != nullptr) {
			if (render.renderer->get_material()->is_render_shadow == false)continue;
		}


		Work_meter::start("shader_activete");
		if (is_shader_activate)render.renderer->shader_activete();
		Work_meter::stop("shader_activete");

		Work_meter::start("render_instancing");
		render.renderer->render_instancing(instanceBuffer, render.bufferStart, render.bufferCount);
		Work_meter::start("render_instancing");
	}
}