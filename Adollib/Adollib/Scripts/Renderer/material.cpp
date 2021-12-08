
#include "../Main/systems.h"
#include "../Math/matrix.h"
#include "Shader/constant_buffer.h"
#include "material.h"
#include "texture.h"
#include "renderer_manager.h"

using namespace Adollib;
using namespace ConstantBuffer;

Material::Material() {
	color = Vector4(1, 1, 1, 1);
	DS_state = State_manager::DStypes::DS_TRUE;
	RS_state = State_manager::RStypes::RS_CULL_BACK;
	BS_state = State_manager::BStypes::BS_ALPHA;

	texture = std::make_shared<Texture>();
	texture->Load(L"./DefaultModel/white.png");
}






