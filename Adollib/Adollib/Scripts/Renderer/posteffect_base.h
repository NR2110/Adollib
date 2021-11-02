#pragma once

#include <d3d11.h>
#include <vector>
#include <list>

#include "Shader/shader.h"

namespace Adollib {
	class Texture;
	class Gameobject;
	class Camera_component;

	class Posteffect_base {

	protected:
		Shader shader;

	protected:
		void Load_VS(const char* cso_name) { shader.Load_VS(cso_name); };
		void Load_PS(const char* cso_name) { shader.Load_PS(cso_name); };
		void Load_GS(const char* cso_name) { shader.Load_GS(cso_name); };
		void Load_HS(const char* cso_name) { shader.Load_HS(cso_name); };
		void Load_DS(const char* cso_name) { shader.Load_DS(cso_name); };

	public:
		Camera_component* camera = nullptr;
		std::list<std::shared_ptr<Posteffect_base>>::iterator this_itr;

	public:
		virtual void awake();
		virtual void render(const Texture* color_tex, const  Texture* normal_tex, const Texture* depth_tex) = 0;



	};


}