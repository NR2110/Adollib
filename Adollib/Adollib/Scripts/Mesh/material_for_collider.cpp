
#include "material_for_collider.h"
#include "../Main/systems.h"
#include "../Main/resource_manager.h"

#include "../Shader/constant_buffer.h"

#include "../Physics/ALP__meshcoll_data.h"
#include "../Physics/ALP_physics.h"

using namespace Adollib;
using namespace Physics_function;

Microsoft::WRL::ComPtr<ID3D11InputLayout> Collider_renderer::vertexLayout;

ComPtr<ID3D11Buffer> Collider_renderer::world_cb; //WVP行列用バッファ
ComPtr<ID3D11Buffer> Collider_renderer::Mat_cb; //material用バッファ

std::map<ALPCollider_shape_type, std::vector<Mesh::mesh>*> Collider_renderer::meshes; //mesh
Shader Collider_renderer::shader; //shader

//カプセルを描画し、ほかのollider表示をoffにする
//#define draw_cupsule_cullback

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
	meshes[ALPCollider_shape_type::BOX];
	ResourceManager::CreateModelFromFBX(&meshes[ALPCollider_shape_type::BOX], "./DefaultModel/cube.fbx", "");

	meshes[ALPCollider_shape_type::Sphere];
	ResourceManager::CreateModelFromFBX(&meshes[ALPCollider_shape_type::Sphere], "./DefaultModel/sphere.fbx", "");

	meshes[ALPCollider_shape_type::Plane];
	ResourceManager::CreateModelFromFBX(&meshes[ALPCollider_shape_type::Plane], "./DefaultModel/plane.fbx", "");

	meshes[ALPCollider_shape_type::Cylinder];
	ResourceManager::CreateModelFromFBX(&meshes[ALPCollider_shape_type::Cylinder], "./DefaultModel/cylinder.fbx", "");

	//meshes[ALP_Collider_shape::Mesh];
	//ResourceManager::CreateModelFromFBX(&meshes[ALP_Collider_shape::Mesh], "../Data/FBX/0311_collisions.fbx", "");
	//ResourceManager::CreateModelFromFBX(&meshes[ALP_Collider_shape::Mesh], "../Adollib/DefaultModel/cylinder.fbx", "");
}

void Collider_renderer::render_collider(const Physics_function::ALP_Collider* coll) {

	Vector4 color;
	for (const auto shape : coll->get_shapes()) {
		if (shape->get_ALPcollider()->get_ALPphysics()->is_sleep)color = Vector4(0, 0, 0, 1);
		else color = Vector4(-1, -1, -1, -1);
#ifndef draw_cupsule_cullback
		if (shape->get_shape_tag() == ALPCollider_shape_type::BOX)render_box(shape, color);
		if (shape->get_shape_tag() == ALPCollider_shape_type::Sphere)render_sphere(shape, color);
		if (shape->get_shape_tag() == ALPCollider_shape_type::Mesh)render_meshcoll(shape, color);
		if (shape->get_shape_tag() == ALPCollider_shape_type::Capsule)render_capsule(shape, color);

#endif // draw_cupsule_cullback
	}
}

void Collider_renderer::render_box(const Collider_shape* shape, const Vector4 color) {
	//CB : ConstantBufferPerCO_OBJ
	ConstantBufferPerGO g_cb;
	g_cb.world = matrix_world(shape->world_scale() * 1.0001f, shape->world_orientation().get_rotate_matrix(), shape->world_position());
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
	meshs = meshes[shape->get_shape_tag()];

	int color_num = 0;

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
		//cb.materialColor = Al_Global::get_gaming(Al_Global::second_per_game * 60, 800).get_XM4();
		if (color.x == -1 && color.y == -1 && color.z == -1 && color.w == -1)
			cb.materialColor = Vector4(Al_Global::get_gaming((Al_Global::second_per_game + color_num * 2) * 60, 600), 1);
		else
			cb.materialColor = color;
		//cb.materialColor = color16[color_num].get_XM4();
		Systems::DeviceContext->UpdateSubresource(Mat_cb.Get(), 0, NULL, &cb, 0, 0);
		Systems::DeviceContext->VSSetConstantBuffers(4, 1, Mat_cb.GetAddressOf());
		Systems::DeviceContext->PSSetConstantBuffers(4, 1, Mat_cb.GetAddressOf());

		//CB : ConstantBufferPerMesh
		{
			ConstantBuffer::ConstantBufferPerMesh g_cb;
			g_cb.Mesh_world = matrix44_identity();
			Systems::DeviceContext->UpdateSubresource(mesh.mesh_cb.Get(), 0, NULL, &g_cb, 0, 0);
			Systems::DeviceContext->VSSetConstantBuffers(3, 1, mesh.mesh_cb.GetAddressOf());
			Systems::DeviceContext->PSSetConstantBuffers(3, 1, mesh.mesh_cb.GetAddressOf());
		}

		for (auto& subset : mesh.subsets)
		{
			Systems::DeviceContext->PSSetShaderResources(0, 1, subset.diffuse.shaderResourceVirw.GetAddressOf());

			// 描画
			Systems::DeviceContext->DrawIndexed(subset.indexCount, subset.indexStart, 0);

		}


		color_num++;
	}

}
void Collider_renderer::render_sphere(const Collider_shape* shape, const Vector4 color) {
	//CB : ConstantBufferPerCO_OBJ
	ConstantBufferPerGO g_cb;
	g_cb.world = matrix_world(shape->world_scale() * 1.0001f, shape->world_orientation().get_rotate_matrix(), shape->world_position());
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
	meshs = meshes[shape->get_shape_tag()];

	int color_num = 0;

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
		if (color.x == -1 && color.y == -1 && color.z == -1 && color.w == -1)
			cb.materialColor = Vector4(Al_Global::get_gaming((Al_Global::second_per_game + color_num * 2) * 60, 600), 1);
		else
			cb.materialColor = color;
		Systems::DeviceContext->UpdateSubresource(Mat_cb.Get(), 0, NULL, &cb, 0, 0);
		Systems::DeviceContext->VSSetConstantBuffers(4, 1, Mat_cb.GetAddressOf());
		Systems::DeviceContext->PSSetConstantBuffers(4, 1, Mat_cb.GetAddressOf());

		//CB : ConstantBufferPerMesh
		{
			ConstantBuffer::ConstantBufferPerMesh g_cb;
			g_cb.Mesh_world = matrix44_identity();
			Systems::DeviceContext->UpdateSubresource(mesh.mesh_cb.Get(), 0, NULL, &g_cb, 0, 0);
			Systems::DeviceContext->VSSetConstantBuffers(3, 1, mesh.mesh_cb.GetAddressOf());
			Systems::DeviceContext->PSSetConstantBuffers(3, 1, mesh.mesh_cb.GetAddressOf());
		}
		for (auto& subset : mesh.subsets)
		{
			Systems::DeviceContext->PSSetShaderResources(0, 1, subset.diffuse.shaderResourceVirw.GetAddressOf());

			// 描画
			Systems::DeviceContext->DrawIndexed(subset.indexCount, subset.indexStart, 0);

		}


		color_num++;
	}

}
void Collider_renderer::render_meshcoll(const Collider_shape* shape, const Vector4 color) {
	//	render_AABB(shape);

		//CB : ConstantBufferPerCO_OBJ
	ConstantBufferPerGO g_cb;
	g_cb.world = matrix_world(shape->world_scale() * 1.0001f, shape->world_orientation().get_rotate_matrix(), shape->world_position());
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
	ResourceManager::CreateModelFromFBX(&meshs, shape->get_mesh_data()->FBX_pass.c_str(), "");

	int color_num = 0;

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
		if (color.x == -1 && color.y == -1 && color.z == -1 && color.w == -1)
			cb.materialColor = Vector4(Al_Global::get_gaming((Al_Global::second_per_game + color_num * 2) * 60, 600), 1);
		else
			cb.materialColor = color;
		Systems::DeviceContext->UpdateSubresource(Mat_cb.Get(), 0, NULL, &cb, 0, 0);
		Systems::DeviceContext->VSSetConstantBuffers(4, 1, Mat_cb.GetAddressOf());
		Systems::DeviceContext->PSSetConstantBuffers(4, 1, Mat_cb.GetAddressOf());

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
			Systems::DeviceContext->PSSetShaderResources(0, 1, subset.diffuse.shaderResourceVirw.GetAddressOf());

			// 描画
			Systems::DeviceContext->DrawIndexed(subset.indexCount, subset.indexStart, 0);

		}


		color_num++;
	}
}

void Collider_renderer::render_capsule(const Collider_shape* shape, const Vector4 color) {
	//CB : ConstantBufferPerCO_OBJ

	std::vector<Mesh::mesh>* meshs;
	meshs = meshes[ALPCollider_shape_type::Cylinder];
	{
		ConstantBufferPerGO g_cb;
		g_cb.world = matrix_world(Vector3(shape->world_scale().x, shape->world_scale().y, shape->world_scale().x) * 1.0001f, shape->world_orientation().get_rotate_matrix(), shape->world_position());
		Systems::DeviceContext->UpdateSubresource(world_cb.Get(), 0, NULL, &g_cb, 0, 0);
		Systems::DeviceContext->VSSetConstantBuffers(0, 1, world_cb.GetAddressOf());
		Systems::DeviceContext->PSSetConstantBuffers(0, 1, world_cb.GetAddressOf());

		Systems::DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		Systems::DeviceContext->IASetInputLayout(vertexLayout.Get());

		shader.Activate();

		Systems::SetBlendState(State_manager::BStypes::BS_NONE);
#ifndef draw_cupsule_cullback
		Systems::SetRasterizerState(State_manager::RStypes::RS_WIRE);
#else
		Systems::SetRasterizerState(State_manager::RStypes::RS_CULL_BACK);
#endif
		Systems::SetDephtStencilState(State_manager::DStypes::DS_TRUE);

		int color_num = 0;

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
			if (color.x == -1 && color.y == -1 && color.z == -1 && color.w == -1)
				cb.materialColor = Vector4(Al_Global::get_gaming((Al_Global::second_per_game + color_num * 2) * 60, 600), 1);
			else
				cb.materialColor = color;
			Systems::DeviceContext->UpdateSubresource(Mat_cb.Get(), 0, NULL, &cb, 0, 0);
			Systems::DeviceContext->VSSetConstantBuffers(4, 1, Mat_cb.GetAddressOf());
			Systems::DeviceContext->PSSetConstantBuffers(4, 1, Mat_cb.GetAddressOf());

			//CB : ConstantBufferPerMesh
			{
				ConstantBuffer::ConstantBufferPerMesh g_cb;
				g_cb.Mesh_world = matrix44_identity();
				Systems::DeviceContext->UpdateSubresource(mesh.mesh_cb.Get(), 0, NULL, &g_cb, 0, 0);
				Systems::DeviceContext->VSSetConstantBuffers(3, 1, mesh.mesh_cb.GetAddressOf());
				Systems::DeviceContext->PSSetConstantBuffers(3, 1, mesh.mesh_cb.GetAddressOf());
			}
			for (auto& subset : mesh.subsets)
			{
				Systems::DeviceContext->PSSetShaderResources(0, 1, subset.diffuse.shaderResourceVirw.GetAddressOf());

				// 描画
				Systems::DeviceContext->DrawIndexed(subset.indexCount, subset.indexStart, 0);

			}


			color_num++;
		}

	}

	meshs = meshes[ALPCollider_shape_type::Sphere];
	{
		ConstantBufferPerGO g_cb;
		g_cb.world = matrix_world(Vector3(shape->world_scale().x) * 1.0001f, shape->world_orientation().get_rotate_matrix(), shape->world_position() + vector3_quatrotate(Vector3(0, shape->world_scale().y, 0), shape->world_orientation()));
		Systems::DeviceContext->UpdateSubresource(world_cb.Get(), 0, NULL, &g_cb, 0, 0);
		Systems::DeviceContext->VSSetConstantBuffers(0, 1, world_cb.GetAddressOf());
		Systems::DeviceContext->PSSetConstantBuffers(0, 1, world_cb.GetAddressOf());

		Systems::DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		Systems::DeviceContext->IASetInputLayout(vertexLayout.Get());

		shader.Activate();

		Systems::SetBlendState(State_manager::BStypes::BS_NONE);
#ifndef draw_cupsule_cullback
		Systems::SetRasterizerState(State_manager::RStypes::RS_WIRE);
#else
		Systems::SetRasterizerState(State_manager::RStypes::RS_CULL_BACK);
#endif
		Systems::SetDephtStencilState(State_manager::DStypes::DS_TRUE);

		int color_num = 0;

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
			if (color.x == -1 && color.y == -1 && color.z == -1 && color.w == -1)
				cb.materialColor = Vector4(Al_Global::get_gaming((Al_Global::second_per_game + color_num * 2) * 60, 600), 1);
			else
				cb.materialColor = color;
			Systems::DeviceContext->UpdateSubresource(Mat_cb.Get(), 0, NULL, &cb, 0, 0);
			Systems::DeviceContext->VSSetConstantBuffers(4, 1, Mat_cb.GetAddressOf());
			Systems::DeviceContext->PSSetConstantBuffers(4, 1, Mat_cb.GetAddressOf());

			//CB : ConstantBufferPerMesh
			{
				ConstantBuffer::ConstantBufferPerMesh g_cb;
				g_cb.Mesh_world = matrix44_identity();
				Systems::DeviceContext->UpdateSubresource(mesh.mesh_cb.Get(), 0, NULL, &g_cb, 0, 0);
				Systems::DeviceContext->VSSetConstantBuffers(3, 1, mesh.mesh_cb.GetAddressOf());
				Systems::DeviceContext->PSSetConstantBuffers(3, 1, mesh.mesh_cb.GetAddressOf());
			}
			for (auto& subset : mesh.subsets)
			{
				Systems::DeviceContext->PSSetShaderResources(0, 1, subset.diffuse.shaderResourceVirw.GetAddressOf());

				// 描画
				Systems::DeviceContext->DrawIndexed(subset.indexCount, subset.indexStart, 0);

			}


			color_num++;
		}

	}

	meshs = meshes[ALPCollider_shape_type::Sphere];
	{
		ConstantBufferPerGO g_cb;
		g_cb.world = matrix_world(Vector3(shape->world_scale().x) * 1.0001f, shape->world_orientation().get_rotate_matrix(), shape->world_position() - vector3_quatrotate(Vector3(0, shape->world_scale().y, 0), shape->world_orientation()));
		Systems::DeviceContext->UpdateSubresource(world_cb.Get(), 0, NULL, &g_cb, 0, 0);
		Systems::DeviceContext->VSSetConstantBuffers(0, 1, world_cb.GetAddressOf());
		Systems::DeviceContext->PSSetConstantBuffers(0, 1, world_cb.GetAddressOf());

		Systems::DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		Systems::DeviceContext->IASetInputLayout(vertexLayout.Get());

		shader.Activate();

		Systems::SetBlendState(State_manager::BStypes::BS_NONE);
#ifndef draw_cupsule_cullback
		Systems::SetRasterizerState(State_manager::RStypes::RS_WIRE);
#else
		Systems::SetRasterizerState(State_manager::RStypes::RS_CULL_BACK);
#endif
		Systems::SetDephtStencilState(State_manager::DStypes::DS_TRUE);

		int color_num = 0;

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
			if (color.x == -1 && color.y == -1 && color.z == -1 && color.w == -1)
				cb.materialColor = Vector4(Al_Global::get_gaming((Al_Global::second_per_game + color_num * 2) * 60, 600), 1);
			else
				cb.materialColor = color;
			Systems::DeviceContext->UpdateSubresource(Mat_cb.Get(), 0, NULL, &cb, 0, 0);
			Systems::DeviceContext->VSSetConstantBuffers(4, 1, Mat_cb.GetAddressOf());
			Systems::DeviceContext->PSSetConstantBuffers(4, 1, Mat_cb.GetAddressOf());

			//CB : ConstantBufferPerMesh
			{
				ConstantBuffer::ConstantBufferPerMesh g_cb;
				g_cb.Mesh_world = matrix44_identity();
				Systems::DeviceContext->UpdateSubresource(mesh.mesh_cb.Get(), 0, NULL, &g_cb, 0, 0);
				Systems::DeviceContext->VSSetConstantBuffers(3, 1, mesh.mesh_cb.GetAddressOf());
				Systems::DeviceContext->PSSetConstantBuffers(3, 1, mesh.mesh_cb.GetAddressOf());
			}
			for (auto& subset : mesh.subsets)
			{
				Systems::DeviceContext->PSSetShaderResources(0, 1, subset.diffuse.shaderResourceVirw.GetAddressOf());

				// 描画
				Systems::DeviceContext->DrawIndexed(subset.indexCount, subset.indexStart, 0);

			}


			color_num++;
		}

	}
}

#include "../Physics/collider.h"
#include "../Main/states_manager.h"

void Collider_renderer::render_AABB(const  Physics_function::ALP_Collider* coll) {

	const std::vector<Mesh::mesh>* box_mesh = meshes[ALPCollider_shape_type::BOX];

	Vector3 w_pos;
	Vector3 w_scale;
	int color_num = 0;
	for (const auto& shape : coll->get_shapes()) {

		if (Systems::RS_type != State_manager::RStypes::RS_CULL_FRONT) Systems::SetRasterizerState(State_manager::RStypes::RS_CULL_FRONT);
		//Debug::dopbaseの表示
#if 0
		{
			shape.mesh->dopbase;

			w_pos = Vector3(
				(shape.mesh->dopbase.max[0] + shape.mesh->dopbase.min[0]) * 0.5f * shape.ALPcollider->world_scale().x,
				(shape.mesh->dopbase.max[1] + shape.mesh->dopbase.min[1]) * 0.5f * shape.ALPcollider->world_scale().y,
				(shape.mesh->dopbase.max[2] + shape.mesh->dopbase.min[2]) * 0.5f * shape.ALPcollider->world_scale().z
			);
			w_scale = Vector3(
				(shape.mesh->dopbase.max[0] - shape.mesh->dopbase.min[0]) * 0.5f * shape.ALPcollider->world_scale().x,
				(shape.mesh->dopbase.max[1] - shape.mesh->dopbase.min[1]) * 0.5f * shape.ALPcollider->world_scale().y,
				(shape.mesh->dopbase.max[2] - shape.mesh->dopbase.min[2]) * 0.5f * shape.ALPcollider->world_scale().z
			);

			//CB : ConstantBufferPerCO_OBJ
			ConstantBufferPerGO g_cb;
			g_cb.world = matrix_world(
				w_scale * 1.001,
				shape.ALPcollider->world_orientation().get_rotate_matrix(),
				vector3_quatrotate(w_pos, shape.ALPcollider->world_orientation()) + shape.ALPcollider->world_position()
			);

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

			int max_c = 0;
			for (const auto& vertex : *shape.mesh->vertices) {
				if (max_c > 50)break;
				max_c++;

				//CB : ConstantBufferPerCO_OBJ
				ConstantBufferPerGO g_cb;
				g_cb.world = matrix_world(
					Vector3(0.01f),
					matrix_identity(),
					(vector3_quatrotate(vertex * shape.ALPcollider->world_scale(), shape.ALPcollider->world_orientation())) + shape.ALPcollider->world_position()
				);

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
					cb.materialColor = Vector4(Al_Global::get_gaming((Al_Global::second_per_game + color_num) * 60, 600), 0.3f).get_XM4();
					//cb.materialColor = color16[color_num].get_XM4();
					Systems::DeviceContext->UpdateSubresource(Mat_cb.Get(), 0, NULL, &cb, 0, 0);
					Systems::DeviceContext->VSSetConstantBuffers(4, 1, Mat_cb.GetAddressOf());
					Systems::DeviceContext->PSSetConstantBuffers(4, 1, Mat_cb.GetAddressOf());

					//CB : ConstantBufferPerMesh
					{
						ConstantBuffer::ConstantBufferPerMesh g_cb;
						g_cb.Mesh_world = matrix_identity();
						Systems::DeviceContext->UpdateSubresource(mesh.mesh_cb.Get(), 0, NULL, &g_cb, 0, 0);
						Systems::DeviceContext->VSSetConstantBuffers(3, 1, mesh.mesh_cb.GetAddressOf());
						Systems::DeviceContext->PSSetConstantBuffers(3, 1, mesh.mesh_cb.GetAddressOf());
					}
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
#if 0
		{
			const std::vector<Mesh::mesh>* sphere_mesh = meshes[ALP_Collider_shape::Sphere];

			int cou = 0;
			for (const auto& vertex : shape.mesh->base_pos) {
				//cou++;
				//if (cou < 12)continue;
				//CB : ConstantBufferPerCO_OBJ
				ConstantBufferPerGO g_cb;
				//g_cb.world = matrix_world(
				//	w_scale * 1.001,
				//	matrix_identity(),
				//	vector3_quatrotate(w_pos, shape.ALPcollider->world_orientation()) + shape.ALPcollider->world_position()
				//);

				g_cb.world = matrix_world(
					Vector3(0.1f),
					matrix_identity(),
					(vector3_quatrotate(vertex * shape.ALPcollider->world_scale(), shape.ALPcollider->world_orientation())) + shape.ALPcollider->world_position()
				);


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

		if (Systems::RS_type != State_manager::RStypes::RS_CULL_BACK) Systems::SetRasterizerState(State_manager::RStypes::RS_CULL_BACK);


		w_pos = Vector3(
			(shape->get_DOP().max[0] + shape->get_DOP().min[0]) * 0.5f,
			(shape->get_DOP().max[1] + shape->get_DOP().min[1]) * 0.5f,
			(shape->get_DOP().max[2] + shape->get_DOP().min[2]) * 0.5f
		);
		w_scale = Vector3(
			(shape->get_DOP().max[0] - shape->get_DOP().min[0]) * 0.5f,
			(shape->get_DOP().max[1] - shape->get_DOP().min[1]) * 0.5f,
			(shape->get_DOP().max[2] - shape->get_DOP().min[2]) * 0.5f
		);

		//CB : ConstantBufferPerCO_OBJ
		ConstantBufferPerGO g_cb;
		g_cb.world = matrix_world(w_scale * 1.001f, matrix44_identity(), w_pos + shape->get_DOP().pos);

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
			cb.materialColor = Vector4(Al_Global::get_gaming((Al_Global::second_per_game + color_num) * 60, 600), 0.3f).get_XM4();
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