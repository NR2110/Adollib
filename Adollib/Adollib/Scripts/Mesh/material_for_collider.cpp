
#include "material_for_collider.h"
#include "../Main/systems.h"
#include "../Main/resource_manager.h"

using namespace Adollib;
using namespace physics_function;

Microsoft::WRL::ComPtr<ID3D11InputLayout> Collider_renderer::vertexLayout;

ComPtr<ID3D11Buffer> Collider_renderer::world_cb; //WVP行列用バッファ
ComPtr<ID3D11Buffer> Collider_renderer::Mat_cb; //material用バッファ

std::map<ALP_Collider_shape, std::vector<Mesh::mesh>*> Collider_renderer::meshes; //mesh
Shader Collider_renderer::shader; //shader


void Collider_renderer::initialize() {
		//::: コンスタントバッファ :::
		Systems::CreateConstantBuffer(&world_cb, sizeof(ConstantBufferPerGO));    //枠確保
		Systems::CreateConstantBuffer(&Mat_cb, sizeof(ConstantBufferPerMaterial));//枠確保

		//::: vertexbuffer & shaderload :::::::
		D3D11_INPUT_ELEMENT_DESC layout[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "WEIGHTS",  0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "BONES"  ,  0, DXGI_FORMAT_R32G32B32A32_UINT,  0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		UINT numElements = ARRAYSIZE(layout);

		shader.Load_VS("./DefaultShader/default_vs.cso", &vertexLayout, layout, numElements);
		shader.Load_PS("./DefaultShader/default_ps.cso");


		//::: 描画用modelの読み込み :::::::
		meshes[ALP_Collider_shape::BOX];
		ResourceManager::CreateModelFromFBX(&meshes[ALP_Collider_shape::BOX], "./DefaultModel/cube.fbx", "");

		meshes[ALP_Collider_shape::Sphere];
		ResourceManager::CreateModelFromFBX(&meshes[ALP_Collider_shape::Sphere], "./DefaultModel/sphere.fbx", "");

		meshes[ALP_Collider_shape::Plane];
		ResourceManager::CreateModelFromFBX(&meshes[ALP_Collider_shape::Plane], "./DefaultModel/plane.fbx", "");

		meshes[ALP_Collider_shape::Mesh];
		ResourceManager::CreateModelFromFBX(&meshes[ALP_Collider_shape::Mesh], "../Data/FBX/0311_collisions.fbx", "");
	}

void Collider_renderer::render(const physics_function::ALP_Collider& R) {
	if (!(
		R.shape == ALP_Collider_shape::BOX ||
		R.shape == ALP_Collider_shape::Sphere ||
		R.shape == ALP_Collider_shape::Mesh
		))return;

	static int time = 0;
	time++;
	//CB : ConstantBufferPerCO_OBJ
	ConstantBufferPerGO g_cb;
	g_cb.world = matrix_world(R.world_scale * 1.0001, R.world_orientation.get_rotate_matrix(), R.world_position).get_XMFLOAT4X4();
	Systems::DeviceContext->UpdateSubresource(world_cb.Get(), 0, NULL, &g_cb, 0, 0);
	Systems::DeviceContext->VSSetConstantBuffers(0, 1, world_cb.GetAddressOf());
	Systems::DeviceContext->PSSetConstantBuffers(0, 1, world_cb.GetAddressOf());

	Systems::DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	Systems::DeviceContext->IASetInputLayout(vertexLayout.Get());

	shader.Activate();

	Systems::SetBlendState(State_manager::BStypes::BS_NONE);
	Systems::SetRasterizerState(State_manager::RStypes::RS_WIRE);
	Systems::SetDephtStencilState(State_manager::DStypes::DS_TRUE);

	std::vector<Mesh::mesh>* meshs;
	meshs = meshes[R.shape];

	//描画
	for (Mesh::mesh mesh : *meshs)
	{
		UINT stride = sizeof(VertexFormat);
		UINT offset = 0;
		Systems::DeviceContext->IASetVertexBuffers(0, 1, mesh.vertexBuffer.GetAddressOf(), &stride, &offset);
		Systems::DeviceContext->IASetIndexBuffer(mesh.indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

		//CB : ConstantBufferPerCoMaterial
		ConstantBufferPerMaterial cb;
		cb.shininess = 1;
		cb.ambientColor = DirectX::XMFLOAT4(0.1f, 0.1f, 0.1f, 1);
		cb.materialColor = Al_Global::get_gaming(time, 800).get_XM4();
		Systems::DeviceContext->UpdateSubresource(Mat_cb.Get(), 0, NULL, &cb, 0, 0);
		Systems::DeviceContext->VSSetConstantBuffers(4, 1, Mat_cb.GetAddressOf());
		Systems::DeviceContext->PSSetConstantBuffers(4, 1, Mat_cb.GetAddressOf());

		for (auto& subset : mesh.subsets)
		{
			Systems::DeviceContext->PSSetShaderResources(0, 1, subset.diffuse.shaderResourceVirw.GetAddressOf());

			// 描画
			Systems::DeviceContext->DrawIndexed(subset.indexCount, subset.indexStart, 0);

		}

	}


}
