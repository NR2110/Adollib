
#include "material_for_collider.h"
#include "systems.h"
#include "resource_manager.h"

namespace Adollib {

	Microsoft::WRL::ComPtr<ID3D11InputLayout> Collider_renderer::vertexLayout;

	ComPtr<ID3D11Buffer> Collider_renderer::world_cb; //WVP行列用バッファ
	ComPtr<ID3D11Buffer> Collider_renderer::Mat_cb; //material用バッファ

	std::map<Collider_shape, std::vector<Mesh::mesh>*> Collider_renderer::meshes; //mesh
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
		meshes[Collider_shape::shape_box];
		ResourceManager::CreateModelFromFBX(&meshes[Collider_shape::shape_box], "./DefaultModel/cube.fbx", "");

		meshes[Collider_shape::shape_sphere];
		ResourceManager::CreateModelFromFBX(&meshes[Collider_shape::shape_sphere], "./DefaultModel/sphere.fbx", "");

		meshes[Collider_shape::shape_plane];
		ResourceManager::CreateModelFromFBX(&meshes[Collider_shape::shape_plane], "./DefaultModel/plane.fbx", "");
	}

	void Collider_renderer::render(const Collider* R) {
		static int time = 0;
		time++;
		//CB : ConstantBufferPerCO_OBJ
		ConstantBufferPerGO g_cb;
		g_cb.world = matrix_world(R->world_size * 1.0001, R->world_orientation.get_rotate_matrix(), R->world_position).get_XMFLOAT4X4();
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
		meshs = meshes[R->shape];

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
			cb.ambientColor = XMFLOAT4(0.1f, 0.1f, 0.1f, 1);
			cb.materialColor = Al_Global::get_gaming(time, 600).get_XM4();
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

}

//Gameobject* Gameobject_manager::createCube(const std::string& go_name, Scenelist Sce) {
//	std::shared_ptr <Gameobject> Value = std::make_shared<Gameobject>();
//	gameobjects[Sce].push_back(Value);
//	Value.get()->name = go_name;
//	//Value.get()->go_iterator = gameobjects[Sce].end();
//	Value.get()->this_scene = Sce;
//	Value.get()->transform = new Transfome;
//
//	Value.get()->material = new Material;
//	Value.get()->material->Load_VS("./DefaultShader/default_vs.cso");
//	Value.get()->material->Load_PS("./DefaultShader/default_ps.cso");
//	ResourceManager::CreateModelFromFBX(&Value.get()->material->meshes, "./DefaultModel/cube.fbx", "");
//	Value.get()->initialize();
//	return Value.get();
//}