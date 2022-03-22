#pragma once

#include <d3d11.h>
#include <vector>
#include <list>

#include "Shader/shader.h"
#include "../Renderer/UI.h"

namespace Adollib {
	class Texture;
	class Gameobject;
	class Camera_component;

	class Posteffect_base {

	protected:
		//Shader shader;
		UI ui;

	protected:
		void Load_VS(const char* cso_name) { ui.shader.Load_VS(cso_name); };
		void Load_PS(const char* cso_name) { ui.shader.Load_PS(cso_name); };
		void Load_GS(const char* cso_name) { ui.shader.Load_GS(cso_name); };
		void Load_HS(const char* cso_name) { ui.shader.Load_HS(cso_name); };
		void Load_DS(const char* cso_name) { ui.shader.Load_DS(cso_name); };

	public:
		Camera_component* camera = nullptr;
		std::list<std::shared_ptr<Posteffect_base>>::iterator this_itr;

	public:
		virtual void awake() = 0;
		virtual void render(std::shared_ptr<Texture> output_tex, std::shared_ptr<Texture> color_tex, std::shared_ptr<Texture> normal_tex, std::shared_ptr<Texture> depth_tex, const UI_data& ui_data) = 0;



	};


}