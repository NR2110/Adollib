#pragma once

#include <vector>
#include <string>
#include <list>
#include <map>
#include <unordered_map>

#include "mesh.h"
#include "material.h"
#include "../Scene/scene_list.h"


namespace Adollib {

	class Camera_component;
	class Light_component;
	class Renderer_base;

	class Renderer_manager {

	private:
		static std::unordered_map<Scenelist, std::list<Renderer_base*>> renderers;

		// lightは全部まとめてcbをsetするため lightcomponentでは処理できない
		static Microsoft::WRL::ComPtr<ID3D11Buffer> light_cb;

		static void set_light_Constantbuffer(const std::list<Light_component*>& lights);
	public:
		// renderer::componentがアタッチされたとき コンストラクタで呼ばれる
		static std::list<Renderer_base*>::iterator add_renderer(Renderer_base* renderer);

		// renderer::componentが削除されるとき呼ばれる
		static void remove_renderer(std::list<Renderer_base*>::iterator itr);

	public:
		static void awake();

		static void render(const std::map<Scenelist, std::list<Camera_component*>>& cameras, const std::map<Scenelist, std::list<Light_component*>>& lights, Scenelist Sce);

	};


}