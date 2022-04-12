#pragma once

#include <vector>
#include <string>
#include <list>
#include <map>
#include <unordered_map>

#include "../Scene/scene_list.h"
#include "instance.h"
#include "frustum_data.h"
#include "mesh.h"
#include "material.h"


namespace Adollib {

	class Camera_component;
	class Light_component;
	class Renderer_base;


	class Renderer_manager {

	private:
		static Microsoft::WRL::ComPtr<ID3D11Buffer> instanceBuffer;
		static std::vector<Renderer_base*> sorted_renderers; //instansingのためにソートされたデータ vectorにしたいためrenderersとは別に管理
		static std::vector<Render_count> render_counts; //instancing描画のための配列 こいつをforで回して描画を行う

		static std::unordered_map<Scenelist, std::list<Renderer_base*>> renderers;

		// lightは全部まとめてcbをsetするため lightcomponentでは処理できない
		static Microsoft::WRL::ComPtr<ID3D11Buffer> light_cb;

	public:
		// renderer::componentがアタッチされたとき コンストラクタで呼ばれる
		static std::list<Renderer_base*>::iterator add_renderer(Renderer_base* renderer);

		// renderer::componentが削除されるとき呼ばれる
		static void remove_renderer(std::list<Renderer_base*>::iterator itr);

	private:
		static void set_light_Constantbuffer(const std::list<Light_component*>& lights);

		static void sort_update(std::list<Scenelist> active_scenes); //instanceのためのsort materialとmeshが同じrendererを連番にするsortを行う

		static void instance_update(const Frustum_data& frustum_data, Camera_component* camera_comp); //sort_updateでまとめた情報から instancingの回数などを計算、render_countsに保存

		static void render_instance(Camera_component* camera_comp, bool is_PSshader_update = true, bool is_shadow_render = false);

	public:
		static void awake();

		static void render(const std::list<Scenelist>& active_scenes, const std::map<Scenelist, std::list<Camera_component*>>& cameras, const std::map<Scenelist, std::list<Light_component*>>& lights);

	};


}