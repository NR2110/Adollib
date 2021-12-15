#include "sky_sphere.h"

#include "../Imgui/imgui_all.h"
#include "../Main/resource_manager.h"
#include "../Main/Adollib.h"
#include "../Main/systems.h"
#include "material.h"
#include "Shader/shader.h"
#include "Shader/constant_buffer.h"
#include "Shader/vertex_format.h"
#include "texture.h"

#include "../Imgui/debug.h"

using namespace Adollib;
using namespace ConstantBuffer;

void Sky_sphere::awake() {

	// コンスタントバッファーの生成
	Systems::CreateConstantBuffer(view_cb.ReleaseAndGetAddressOf(),       sizeof(ConstantBufferPerCamera));
	Systems::CreateConstantBuffer(projection_cb.ReleaseAndGetAddressOf(), sizeof(ConstantBufferPerSystem));

	// sky用textureの準備
	sky_texture = std::make_shared<Texture>();
	sky_texture->Load(L"./DefaultTexture/sky_map.png");

	shader = std::make_shared<Shader>();
	shader->Load_VS("./DefaultShader/sky_map_vs.cso");
	shader->Load_PS("./DefaultShader/default_ps_skysphere.cso");

	Systems::CreateConstantBuffer(projection_cb.ReleaseAndGetAddressOf(), sizeof(ConstantBufferPerSystem));
	Systems::CreateConstantBuffer(world_cb.ReleaseAndGetAddressOf(), sizeof(ConstantBufferPerGO));
	Systems::CreateConstantBuffer(Mat_cb.ReleaseAndGetAddressOf(), sizeof(ConstantBufferPerMaterial));

	ResourceManager::CreateModelFromFBX(&meshes, "./DefaultModel/sphere.fbx");

}
void Sky_sphere::shader_activate() { shader->Activate(); };


void Sky_sphere::render() {

	Systems::DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// CB : ConstantBufferPerOBJ
	// GOのtransformの情報をConstantBufferへセットする
	ConstantBufferPerGO g_cb;
	g_cb.world = matrix_world(Vector3(10000), quaternion_from_euler(rotate).get_rotate_matrix(), position);
	Systems::DeviceContext->UpdateSubresource(world_cb.Get(), 0, NULL, &g_cb, 0, 0);
	Systems::DeviceContext->VSSetConstantBuffers(0, 1, world_cb.GetAddressOf());
	Systems::DeviceContext->PSSetConstantBuffers(0, 1, world_cb.GetAddressOf());

	//CB : ConstantBufferPerSystem
	ConstantBufferPerSystem s_sb;
	DirectX::XMStoreFloat4x4(&s_sb.Projection, DirectX::XMMatrixPerspectiveFovLH(ToRadian(fov), aspect, nearZ, farZ));
	Systems::DeviceContext->UpdateSubresource(projection_cb.Get(), 0, NULL, &s_sb, 0, 0);
	Systems::DeviceContext->VSSetConstantBuffers(2, 1, projection_cb.GetAddressOf());
	Systems::DeviceContext->PSSetConstantBuffers(2, 1, projection_cb.GetAddressOf());

	//
	Systems::SetBlendState(State_manager::BStypes::BS_NONE);
	Systems::SetRasterizerState(State_manager::RStypes::RS_CULL_FRONT);
	Systems::SetDephtStencilState(State_manager::DStypes::DS_TRUE);

	//CB : ConstantBufferPerMaterial
	ConstantBufferPerMaterial cb;
	// specular
	cb.shininess = 1;
	cb.ambientColor = DirectX::XMFLOAT4(0.1f, 0.1f, 0.1f, 1);
	cb.materialColor = Vector4(1, 0.99f, 0.9f, 1) * 0.9f;

	Systems::DeviceContext->UpdateSubresource(Mat_cb.Get(), 0, NULL, &cb, 0, 0);
	Systems::DeviceContext->VSSetConstantBuffers(5, 1, Mat_cb.GetAddressOf());
	Systems::DeviceContext->PSSetConstantBuffers(5, 1, Mat_cb.GetAddressOf());

	for (Mesh::mesh& mesh : (*meshes))
	{
		UINT stride = sizeof(VertexFormat);
		UINT offset = 0;
		Systems::DeviceContext->IASetVertexBuffers(0, 1, mesh.vertexBuffer.GetAddressOf(), &stride, &offset);
		Systems::DeviceContext->IASetIndexBuffer(mesh.indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

		//CB : ConstantBufferPerMesh
		{
			ConstantBuffer::ConstantBufferPerMesh g_cb;
			g_cb.Mesh_world = mesh.globalTransform;
			Systems::DeviceContext->UpdateSubresource(mesh.mesh_cb.Get(), 0, NULL, &g_cb, 0, 0);
			Systems::DeviceContext->VSSetConstantBuffers(3, 1, mesh.mesh_cb.GetAddressOf());
			Systems::DeviceContext->PSSetConstantBuffers(3, 1, mesh.mesh_cb.GetAddressOf());
		}


		for (auto& subset : mesh.subsets)
		{
			// textureをSRVにセット
			sky_texture->Set(0);

			// 描画
			Systems::DeviceContext->DrawIndexed(subset.indexCount, subset.indexStart, 0);

		}
	}


}