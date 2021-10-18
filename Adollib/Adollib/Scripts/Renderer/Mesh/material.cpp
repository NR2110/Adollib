
#include "../../Main/systems.h"
#include "material.h"
#include "../../Math/matrix.h"
#include "frustum_culling.h"

#include "../Shader/constant_buffer.h"

using namespace Adollib;
using namespace ConstantBuffer;

Material::Material() {
	color = Vector4(1, 1, 1, 1);
	DS_state = State_manager::DStypes::DS_TRUE;
	RS_state = State_manager::RStypes::RS_CULL_BACK;
	BS_state = State_manager::BStypes::BS_ALPHA;
}