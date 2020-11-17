
#include "material_for_collider.h"
#include "../Main/systems.h"
#include "../Main/resource_manager.h"

using namespace Adollib;
using namespace Physics_function;

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
		//ResourceManager::CreateModelFromFBX(&meshes[ALP_Collider_shape::Mesh], "../Adollib/DefaultModel/cylinder.fbx", "");
	}

void Collider_renderer::render_collider(const Physics_function::ALP_Collider& R) {
	if (!(
		R.shape == ALP_Collider_shape::BOX ||
		R.shape == ALP_Collider_shape::Sphere ||
		R.shape == ALP_Collider_shape::Mesh
		))return;

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

	//Vector4 color16[16];
	int color_num = 0;
	//for (int i = 0; i < 16; i ++ ) {
	//	color16[i] = Al_Global::get_gaming(i, 16);
	//}

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
//		cb.materialColor = Al_Global::get_gaming(Al_Global::second_per_game * 60, 800).get_XM4();
		cb.materialColor = Al_Global::get_gaming((Al_Global::second_per_game + color_num * 2) * 60, 600).get_XM4();
		//cb.materialColor = color16[color_num].get_XM4();
		Systems::DeviceContext->UpdateSubresource(Mat_cb.Get(), 0, NULL, &cb, 0, 0);
		Systems::DeviceContext->VSSetConstantBuffers(4, 1, Mat_cb.GetAddressOf());
		Systems::DeviceContext->PSSetConstantBuffers(4, 1, Mat_cb.GetAddressOf());

		for (auto& subset : mesh.subsets)
		{
			Systems::DeviceContext->PSSetShaderResources(0, 1, subset.diffuse.shaderResourceVirw.GetAddressOf());

			// 描画
			Systems::DeviceContext->DrawIndexed(subset.indexCount, subset.indexStart, 0);

		}


		color_num++;
	}


}

#include "../Physics/collider.h"
#include "../Main/states_manager.h"

void Collider_renderer::render_AABB(const Physics_function::ALP_Collider& coll) {

	const std::vector<Mesh::mesh>* box_mesh = meshes[ALP_Collider_shape::BOX];

	Vector3 w_pos;
	Vector3 w_scale;
	int color_num = 0;
	for (const auto& coll_mesh : coll.collider_meshes) {

		if (Systems::RS_type != State_manager::RStypes::RS_CULL_FRONT) Systems::SetRasterizerState(State_manager::RStypes::RS_CULL_FRONT);
//Debug::dopbaseの表示
#if 1
		{
			coll_mesh.mesh->dopbase;

			w_pos = Vector3(
				(coll_mesh.mesh->dopbase.max[0] + coll_mesh.mesh->dopbase.min[0]) * 0.5f * coll_mesh.ALPcollider->world_scale.x,
				(coll_mesh.mesh->dopbase.max[1] + coll_mesh.mesh->dopbase.min[1]) * 0.5f * coll_mesh.ALPcollider->world_scale.y,
				(coll_mesh.mesh->dopbase.max[2] + coll_mesh.mesh->dopbase.min[2]) * 0.5f * coll_mesh.ALPcollider->world_scale.z
			);
			w_scale = Vector3(
				(coll_mesh.mesh->dopbase.max[0] - coll_mesh.mesh->dopbase.min[0]) * 0.5f * coll_mesh.ALPcollider->world_scale.x,
				(coll_mesh.mesh->dopbase.max[1] - coll_mesh.mesh->dopbase.min[1]) * 0.5f * coll_mesh.ALPcollider->world_scale.y,
				(coll_mesh.mesh->dopbase.max[2] - coll_mesh.mesh->dopbase.min[2]) * 0.5f * coll_mesh.ALPcollider->world_scale.z
			);

			//CB : ConstantBufferPerCO_OBJ
			ConstantBufferPerGO g_cb;
			g_cb.world = matrix_world(
				w_scale * 1.001,
				coll_mesh.ALPcollider->world_orientation.get_rotate_matrix(),
				vector3_quatrotate(w_pos, coll_mesh.ALPcollider->world_orientation) + coll_mesh.ALPcollider->world_position
			).get_XMFLOAT4X4();

			Systems::DeviceContext->UpdateSubresource(world_cb.Get(), 0, NULL, &g_cb, 0, 0);
			Systems::DeviceContext->VSSetConstantBuffers(0, 1, world_cb.GetAddressOf());
			Systems::DeviceContext->PSSetConstantBuffers(0, 1, world_cb.GetAddressOf());

			Systems::DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			Systems::DeviceContext->IASetInputLayout(vertexLayout.Get());

			for (const auto& mesh : *box_mesh) {

				UINT stride = sizeof(VertexFormat);
				UINT offset = 0;
				Systems::DeviceContext->IASetVertexBuffers(0, 1, mesh.vertexBuffer.GetAddressOf(), &stride, &offset);
				Systems::DeviceContext->IASetIndexBuffer(mesh.indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

				//CB : ConstantBufferPerCoMaterial
				ConstantBufferPerMaterial cb;
				cb.shininess = 1;
				cb.ambientColor = DirectX::XMFLOAT4(0.1f, 0.1f, 0.1f, 1);
				cb.materialColor = Vector4(Al_Global::get_gaming((Al_Global::second_per_game + color_num * 2) * 60, 600).xyz, 1).get_XM4();
				//cb.materialColor = color16[color_num].get_XM4();
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
#endif

		if (Systems::RS_type != State_manager::RStypes::RS_CULL_BACK) Systems::SetRasterizerState(State_manager::RStypes::RS_CULL_BACK);
//Debug::頂点の表示
#if 0
		{
			const std::vector<Mesh::mesh>* sphere_mesh = meshes[ALP_Collider_shape::Sphere];

			for (const auto& vertex : *coll_mesh.mesh->vertices) {
				//CB : ConstantBufferPerCO_OBJ
				ConstantBufferPerGO g_cb;
				g_cb.world = matrix_world(
					Vector3(0.01f),
					matrix_identity(), 
					(vector3_quatrotate(vertex * coll_mesh.ALPcollider->world_scale, coll_mesh.ALPcollider->world_orientation)) + coll_mesh.ALPcollider->world_position
				).get_XMFLOAT4X4();

				Systems::DeviceContext->UpdateSubresource(world_cb.Get(), 0, NULL, &g_cb, 0, 0);
				Systems::DeviceContext->VSSetConstantBuffers(0, 1, world_cb.GetAddressOf());
				Systems::DeviceContext->PSSetConstantBuffers(0, 1, world_cb.GetAddressOf());

				Systems::DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
				Systems::DeviceContext->IASetInputLayout(vertexLayout.Get());

				for (const auto& mesh : *sphere_mesh) {

					UINT stride = sizeof(VertexFormat);
					UINT offset = 0;
					Systems::DeviceContext->IASetVertexBuffers(0, 1, mesh.vertexBuffer.GetAddressOf(), &stride, &offset);
					Systems::DeviceContext->IASetIndexBuffer(mesh.indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

					//CB : ConstantBufferPerCoMaterial
					ConstantBufferPerMaterial cb;
					cb.shininess = 1;
					cb.ambientColor = DirectX::XMFLOAT4(0.1f, 0.1f, 0.1f, 1);
					cb.materialColor = Vector4(Al_Global::get_gaming((Al_Global::second_per_game + color_num * 2) * 60, 600).xyz, 1).get_XM4();
					//cb.materialColor = color16[color_num].get_XM4();
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
#endif

//Debug::baseposの表示
#if 1
		{
			const std::vector<Mesh::mesh>* sphere_mesh = meshes[ALP_Collider_shape::Sphere];

			int cou = 0;
			for (const auto& vertex : coll_mesh.mesh->base_pos) {
				//cou++;
				//if (cou < 12)continue;
				//CB : ConstantBufferPerCO_OBJ
				ConstantBufferPerGO g_cb;
				//g_cb.world = matrix_world(
				//	w_scale * 1.001,
				//	matrix_identity(),
				//	vector3_quatrotate(w_pos, coll_mesh.ALPcollider->world_orientation) + coll_mesh.ALPcollider->world_position
				//).get_XMFLOAT4X4();

				g_cb.world = matrix_world(
					Vector3(0.1f), 
					matrix_identity(),
					(vector3_quatrotate(vertex * coll_mesh.ALPcollider->world_scale, coll_mesh.ALPcollider->world_orientation)) + coll_mesh.ALPcollider->world_position
				).get_XMFLOAT4X4();


				Systems::DeviceContext->UpdateSubresource(world_cb.Get(), 0, NULL, &g_cb, 0, 0);
				Systems::DeviceContext->VSSetConstantBuffers(0, 1, world_cb.GetAddressOf());
				Systems::DeviceContext->PSSetConstantBuffers(0, 1, world_cb.GetAddressOf());

				Systems::DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
				Systems::DeviceContext->IASetInputLayout(vertexLayout.Get());

				for (const auto& mesh : *sphere_mesh) {

					UINT stride = sizeof(VertexFormat);
					UINT offset = 0;
					Systems::DeviceContext->IASetVertexBuffers(0, 1, mesh.vertexBuffer.GetAddressOf(), &stride, &offset);
					Systems::DeviceContext->IASetIndexBuffer(mesh.indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

					//CB : ConstantBufferPerCoMaterial
					ConstantBufferPerMaterial cb;
					cb.shininess = 1;
					cb.ambientColor = DirectX::XMFLOAT4(0.1f, 0.1f, 0.1f, 1);
					cb.materialColor = Vector4(Al_Global::get_gaming((Al_Global::second_per_game + color_num * 2) * 60, 600).xyz, 1).get_XM4();
					//cb.materialColor = color16[color_num].get_XM4();
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
#endif

		if (Systems::RS_type != State_manager::RStypes::RS_CULL_FRONT) Systems::SetRasterizerState(State_manager::RStypes::RS_CULL_FRONT);


		w_pos = Vector3(
			(coll_mesh.dop14.max[0] + coll_mesh.dop14.min[0]) * 0.5f,
			(coll_mesh.dop14.max[1] + coll_mesh.dop14.min[1]) * 0.5f,
			(coll_mesh.dop14.max[2] + coll_mesh.dop14.min[2]) * 0.5f
		);
		w_scale = Vector3(
			(coll_mesh.dop14.max[0] - coll_mesh.dop14.min[0]) * 0.5f,
			(coll_mesh.dop14.max[1] - coll_mesh.dop14.min[1]) * 0.5f,
			(coll_mesh.dop14.max[2] - coll_mesh.dop14.min[2]) * 0.5f
		);

		//CB : ConstantBufferPerCO_OBJ
		ConstantBufferPerGO g_cb;
		g_cb.world = matrix_world(w_scale * 1.001, matrix_identity(), w_pos + coll_mesh.dop14.pos).get_XMFLOAT4X4();

		Systems::DeviceContext->UpdateSubresource(world_cb.Get(), 0, NULL, &g_cb, 0, 0);
		Systems::DeviceContext->VSSetConstantBuffers(0, 1, world_cb.GetAddressOf());
		Systems::DeviceContext->PSSetConstantBuffers(0, 1, world_cb.GetAddressOf());

		Systems::DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		Systems::DeviceContext->IASetInputLayout(vertexLayout.Get());

		for (const auto& mesh : *box_mesh) {

			UINT stride = sizeof(VertexFormat);
			UINT offset = 0;
			Systems::DeviceContext->IASetVertexBuffers(0, 1, mesh.vertexBuffer.GetAddressOf(), &stride, &offset);
			Systems::DeviceContext->IASetIndexBuffer(mesh.indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

			//CB : ConstantBufferPerCoMaterial
			ConstantBufferPerMaterial cb;
			cb.shininess = 1;
			cb.ambientColor = DirectX::XMFLOAT4(0.1f, 0.1f, 0.1f, 1);
			cb.materialColor = Vector4(Al_Global::get_gaming((Al_Global::second_per_game + color_num) * 60, 600).xyz, 0.3f).get_XM4();
			//cb.materialColor = color16[color_num].get_XM4();
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

		color_num++;

	}


}