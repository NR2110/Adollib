#pragma once

#include "posteffect_base.h"

namespace Adollib {

	class Vignette_effect : public Posteffect_base {

	//protected:
	//	Shader shader;

	//protected:
	//	void Load_VS(const char* cso_name) { shader.Load_VS(cso_name); };
	//	void Load_PS(const char* cso_name) { shader.Load_PS(cso_name); };
	//	void Load_GS(const char* cso_name) { shader.Load_GS(cso_name); };
	//	void Load_HS(const char* cso_name) { shader.Load_HS(cso_name); };
	//	void Load_DS(const char* cso_name) { shader.Load_DS(cso_name); };

	//public:
	//	Camera_component* camera = nullptr;
	//	std::list<std::shared_ptr<Posteffect_base>>::iterator this_itr;

	public:
		void awake() override;
		void render(std::shared_ptr<Texture> output_tex, std::shared_ptr<Texture> color_tex, std::shared_ptr<Texture> normal_tex, std::shared_ptr<Texture> depth_tex, const UI_data& ui_data) override;



	};

}