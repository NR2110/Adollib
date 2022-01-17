
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

void Material::create_constantbuffer(int l_constant_buffer_num, int l_constant_buffer_Vector4_count) {

	if (constant_buffer_Vector4_count != l_constant_buffer_Vector4_count) {
		Systems::CreateConstantBuffer(mat_cb.ReleaseAndGetAddressOf(), sizeof(Vector4) * l_constant_buffer_Vector4_count);
	}

	constant_buffer_num = l_constant_buffer_num;
	constant_buffer_Vector4_count = l_constant_buffer_Vector4_count;

}

void Material::set_constantbuffer() {

	//std::vector<Vector4> m_cb;
	//m_cb.resize(constant_buffer_Vector4_count);

	//for (int i = 0; i < constant_buffer_Vector4_count; ++i) {
	//	m_cb[i] = constant_buffer_data[i];
	//}

	//g_cb.Mesh_world = mesh.globalTransform;
	if (constant_buffer_Vector4_count != 0) {
		Systems::DeviceContext->UpdateSubresource(mat_cb.Get(), 0, NULL, constant_buffer_data, 0, 0);
		Systems::DeviceContext->VSSetConstantBuffers(constant_buffer_num, 1, mat_cb.GetAddressOf());
		Systems::DeviceContext->PSSetConstantBuffers(constant_buffer_num, 1, mat_cb.GetAddressOf());
	}
}




