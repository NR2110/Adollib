#pragma once

#include <vector>
#include <string>
#include "../Main/states_manager.h"
#include "../Math/vector.h"
#include "Shader/shader.h"
#include "mesh.h"

namespace Adollib {

	class Material {
	public:
		Material();

	private:
		Shader shader;

	public:

		Vector4 color;

		State_manager::DStypes DS_state;

		State_manager::RStypes RS_state;

		State_manager::BStypes BS_state;


		void Load_VS(const char* cso_name) { shader.Load_VS(cso_name); };
		void Load_PS(const char* cso_name) { shader.Load_PS(cso_name); };
		void Load_GS(const char* cso_name) { shader.Load_GS(cso_name); };
		void Load_HS(const char* cso_name) { shader.Load_HS(cso_name); };
		void Load_DS(const char* cso_name) { shader.Load_DS(cso_name); };

		void shader_activate() { shader.Activate(); };

	};

}