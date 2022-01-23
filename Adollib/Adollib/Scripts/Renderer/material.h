#pragma once

#include <vector>
#include <string>
#include <list>
#include "../Main/states_manager.h"
#include "../Math/vector.h"
#include "Shader/shader.h"
#include "mesh.h"

namespace Adollib {
	class Texture;

	class Material {
	public:
		Material();

	private:
		Shader shader;

		std::shared_ptr<Texture> texture = nullptr; //texture情報

		Microsoft::WRL::ComPtr<ID3D11Buffer> mat_cb = nullptr; //constantbuffer情報
		int constant_buffer_num = 0;
		int constant_buffer_Vector4_count = 0;

	public:
		std::string name; //materialの名前

		void set_texture(std::shared_ptr<Texture> l_texture) { texture = l_texture; };
		std::shared_ptr<Texture>  get_texture() { return texture; };

		Vector4 color;

		bool is_render_shadow = true;

		Vector4 constant_buffer_data[4];

		State_manager::DStypes DS_state;

		State_manager::RStypes RS_state;

		State_manager::BStypes BS_state;

		void Load_VS(const char* cso_name) { shader.Load_VS(cso_name); };
		void Load_PS(const char* cso_name) { shader.Load_PS(cso_name); };
		void Load_GS(const char* cso_name) { shader.Load_GS(cso_name); };
		void Load_HS(const char* cso_name) { shader.Load_HS(cso_name); };
		void Load_DS(const char* cso_name) { shader.Load_DS(cso_name); };

		void shader_activate() { shader.Activate(); };

		void create_constantbuffer(int constant_buffer_num, int constant_buffer_Vector4_count);
		void set_constantbuffer();

	};

}