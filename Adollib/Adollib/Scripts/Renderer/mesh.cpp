#include <fbxsdk.h>
#include <vector>
#include <functional>
#include <sstream>
#include <iostream>

#include "../../Main/resource_manager.h"
#include "../mesh/mesh.h"
#include "../Shader/constant_buffer.h"

using namespace fbxsdk;

#pragma comment(lib,"libfbxsdk-md.lib")

namespace Adollib{

//	void Mesh::initialize() {
//		Systems::CreateConstantBuffer(&mesh_cb, sizeof(ConstantBuffer::ConstantBufferPerMesh));
//}
//	void Mesh::set_constant_buffer(const DirectX::XMFLOAT4X4& globalTransform) {
//		ConstantBuffer::ConstantBufferPerMesh g_cb;
//		g_cb.Mesh_world = globalTransform;
//		Systems::DeviceContext->UpdateSubresource(mesh_cb.Get(), 0, NULL, &g_cb, 0, 0);
//		Systems::DeviceContext->VSSetConstantBuffers(3, 1, mesh_cb.GetAddressOf());
//		Systems::DeviceContext->PSSetConstantBuffers(3, 1, mesh_cb.GetAddressOf());
//	}
}