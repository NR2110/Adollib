
#include "croth_renderer.h"

#include "../Object/gameobject.h"
#include "renderer_manager.h"

#include "../Main/systems.h"
#include "Shader/constant_buffer.h"
#include "Shader/vertex_format.h"

#include "material.h"
#include "texture.h"

#include "material_manager.h"
#include "../Main/resource_manager.h"

#include "Shader/compute_shader.h"

#include "../Imgui/work_meter.h"

#include <thread>
#include <mutex>

using namespace Adollib;
using namespace ConstantBuffer;
using namespace Compute_S;

//#define use_thread_crothrenderer
//
//template<class T>
//T* map_buffer(Microsoft::WRL::ComPtr<ID3D11Buffer>& buffer) {
//	HRESULT hr = S_OK;
//	const D3D11_MAP map = D3D11_MAP_WRITE_DISCARD;
//	D3D11_MAPPED_SUBRESOURCE mappedBuffer = {};
//	hr = Systems::DeviceContext->Map(buffer.Get(), 0, map, 0, &mappedBuffer);
//
//	if (FAILED(hr))
//	{
//		assert(0 && "failed Map InstanceBuffer dynamic(RenderManager)");
//		return nullptr;
//	}
//	return static_cast<T*>(mappedBuffer.pData);
//}
//void unmap_buffer(Microsoft::WRL::ComPtr<ID3D11Buffer>& buffer) {
//	Systems::DeviceContext->Unmap(buffer.Get(), 0);
//}


void Croth_renderer::init() {

	//	頂点バッファ作成
	{
		VertexFormat v[3];

		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(VertexFormat) * 3;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA res;
		ZeroMemory(&res, sizeof(res));
		res.pSysMem = v;

		Systems::Device->CreateBuffer(&bd, &res, vertexBuffer.ReleaseAndGetAddressOf());
	}

	// indexバッファ作成
	{
		u_int indices[3] = { 0,1,2 };
		D3D11_BUFFER_DESC indexDesc = {};
		indexDesc.ByteWidth = 3 * sizeof(u_int);          // バッファのサイズ
		indexDesc.Usage = D3D11_USAGE_IMMUTABLE;	          // バッファの読み書き法
		indexDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;    // パイプラインにどうバインドするか指定
		indexDesc.CPUAccessFlags = 0;                    // CPUのアクセスフラグ　0でアクセスしない
		indexDesc.MiscFlags = 0;                           // その他のフラグ
		indexDesc.StructureByteStride = 0;                 // バッファ構造体の場合の要素数

		D3D11_SUBRESOURCE_DATA indexSubResource = {};
		indexSubResource.pSysMem = &indices[0];   // 初期化データのポインタ
		indexSubResource.SysMemPitch = 0;        // 頂点バッファでは使わない
		indexSubResource.SysMemSlicePitch = 0;   // 頂点バッファでは使わない

		Systems::Device->CreateBuffer(&indexDesc, &indexSubResource, indexBuffer.ReleaseAndGetAddressOf());

	}

	// computeshaderのload
	{
		compute_shader = std::make_shared<ComputeShader>();
		compute_shader->Load("./DefaultShader/croth_calculate_cs.cso");
	}


	// すでにmeshが存在すれば そのmeshをvertexをmargeする形でloadし直す
	{
		if (meshes != nullptr) {
			std::vector<Mesh::mesh>* no_marge_vertex_mesh;
			ResourceManager::CreateModelFromFBX(&no_marge_vertex_mesh, (*meshes).at(0).file_pass.c_str(), true);
			meshes = no_marge_vertex_mesh;
		}
	}

	// 初期maerialの設定
	{
		material = Material_manager::find_material("croth_material");

		if (material == nullptr) {

			material = Material_manager::create_material("croth_material");

			material->Load_VS("./DefaultShader/croth_shader_vs.cso");
			material->Load_PS("./DefaultShader/croth_shader_ps.cso");

			material->BS_state = State_manager::BStypes::BS_NONE;
			material->RS_state = State_manager::RStypes::RS_CULL_NONE;
		}
	}

	is_use_instancing = false;
}

void Croth_renderer::render(const Frustum_data& frustum_data) {
	if (material == nullptr) return;

	Systems::DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// CB : ConstantBufferPerOBJ
	// GOのtransformの情報をConstantBufferへセットする
	ConstantBufferPerGO g_cb;
	g_cb.world = matrix_world(Vector3(1), matrix33_identity(), Vector3(0));
	Systems::DeviceContext->UpdateSubresource(world_cb.Get(), 0, NULL, &g_cb, 0, 0);
	Systems::DeviceContext->VSSetConstantBuffers(0, 1, world_cb.GetAddressOf());
	Systems::DeviceContext->PSSetConstantBuffers(0, 1, world_cb.GetAddressOf());

	//
	Systems::SetBlendState(material->BS_state);
	Systems::SetRasterizerState(material->RS_state);
	Systems::SetDephtStencilState(material->DS_state);

	for (Mesh::mesh& mesh : (*meshes))
	{
		// frustum_cullingを行う
		{
			const Vector3& box_position = transform->position;
			Vector3 box_axis[3] = {
				vector3_quatrotate(Vector3(1,0,0),transform->orientation),
				vector3_quatrotate(Vector3(0,1,0),transform->orientation),
				vector3_quatrotate(Vector3(0,0,1),transform->orientation)
			};

			Vector3 dop_average = Vector3(
				fabsf(mesh.dop.min[0]) + fabsf(mesh.dop.max[0]),
				fabsf(mesh.dop.min[1]) + fabsf(mesh.dop.max[1]),
				fabsf(mesh.dop.min[2]) + fabsf(mesh.dop.max[2])
			) * 0.5f;

			Vector3 dop_center_offset = Vector3(
				mesh.dop.min[0] + mesh.dop.max[0],
				mesh.dop.min[1] + mesh.dop.max[1],
				mesh.dop.min[2] + mesh.dop.max[2]
			) * 0.5f;
			dop_center_offset = vector3_quatrotate(dop_center_offset, transform->orientation);


			bool is_contain = true;
			for (int i = 0; i < 6; i++) {

				float dis =
					fabsf(vector3_dot(frustum_data.normals[i], box_axis[0] * (dop_average[0] * transform->scale[0]))) +
					fabsf(vector3_dot(frustum_data.normals[i], box_axis[1] * (dop_average[1] * transform->scale[1]))) +
					fabsf(vector3_dot(frustum_data.normals[i], box_axis[2] * (dop_average[2] * transform->scale[2])));

				if (frustum_data.distances[i] > vector3_dot(frustum_data.normals[i], box_position + dop_center_offset) + dis) {
					is_contain = false;
					break;
				}
			}
			if (is_contain == false) continue;


		}

		UINT stride = sizeof(VertexFormat);
		UINT offset = 0;
		Systems::DeviceContext->IASetVertexBuffers(0, 1, mesh.vertexBuffer.GetAddressOf(), &stride, &offset);
		Systems::DeviceContext->IASetIndexBuffer(mesh.indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);


		//CB : ConstantBufferPerMaterial
		ConstantBufferPerMaterial cb;
		for (int i = 0; i < MAX_BONES; i++) {
			cb.boneTransforms[i] = matrix44_identity();
		}
		{
			// boneTransform
			if (mesh.skeletalAnimation.size() > 0 && mesh.skeletalAnimation[animeIndex].size() > 0)
			{
				int frame = (int)(mesh.skeletalAnimation.at(animeIndex).animation_tick / mesh.skeletalAnimation.at(animeIndex).sampling_time);
				if (frame > (int)mesh.skeletalAnimation.at(animeIndex).size() - 1)
				{
					frame = 0;
					mesh.skeletalAnimation.at(animeIndex).animation_tick = 0;
				}
				std::vector<Mesh::bone>& skeletal = mesh.skeletalAnimation.at(animeIndex).at(frame);
				size_t number_of_bones = skeletal.size();
				_ASSERT_EXPR(number_of_bones < MAX_BONES, L"'the number_of_bones' exceeds MAX_BONES.");
				for (size_t i = 0; i < number_of_bones; i++)
				{
					XMStoreFloat4x4(&cb.boneTransforms[i], DirectX::XMLoadFloat4x4(&skeletal.at(i).transform));
				}
				mesh.skeletalAnimation.at(animeIndex).animation_tick += 1;
			}
			// specular
			cb.shininess = 1;
			cb.ambientColor = DirectX::XMFLOAT4(0.1f, 0.1f, 0.1f, 1);
			cb.materialColor = material->color.get_XM4();
			//continue;
		}
		Systems::DeviceContext->UpdateSubresource(Mat_cb.Get(), 0, NULL, &cb, 0, 0);
		Systems::DeviceContext->VSSetConstantBuffers(5, 1, Mat_cb.GetAddressOf());
		Systems::DeviceContext->PSSetConstantBuffers(5, 1, Mat_cb.GetAddressOf());

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
			material->get_texture()->Set(0);

			// 描画
			Systems::DeviceContext->DrawIndexed(subset.indexCount, subset.indexStart, 0);

		}
	}

}

void Croth_renderer::update() {
	if (material == nullptr) return;
	{
		int mesh_count = 0;
		for (auto& buffer : computeBuf_vertexoffset) {

			auto data = ComputeShader_function::map_buffer<Croth_VertexOffset>(buffer, Systems::DeviceContext);

			int vertex_size = meshes->at(mesh_count).vertices.size();
			int vertex_start = 0;
			for (int i = 0; i < vertex_size; ++i) {
				data[i].position = Vector3(mesh_offset->at(mesh_count)[i].first) / transform->scale;
				data[i].normal = mesh_offset->at(mesh_count)[i].second;
			}
			++mesh_count;
			ComputeShader_function::unmap_buffer(buffer, Systems::DeviceContext);
		}
	}

	// color情報の更新 (instance_countをwに入れている)
	{

		int mesh_count = 0;
		int instance_count = 0;
		for (auto& buffer : computeBuf_color) {

			auto data = ComputeShader_function::map_buffer<Vector4>(buffer, Systems::DeviceContext);

			data[0] = Vector4(color.x, color.y, color.z, instance_count);

			ComputeShader_function::unmap_buffer(buffer, Systems::DeviceContext);

			instance_count += meshes->at(mesh_count).indexces.size() / 3;
			++mesh_count;
		}

	}

	{
		instance_count = 0;

		// meshから
		int mesh_count = 0;
		for (Mesh::mesh& mesh : (*meshes))
		{
			int indexes_count = mesh.indexces.size();
			int vertices_count = mesh.vertices.size();
			int instances_count = indexes_count / 3;

			// compute shaderを走らせる
			ID3D11ShaderResourceView* pSRVs[4] = { computeBufSRV_index[mesh_count].Get(), computeBufSRV_vertex[mesh_count].Get(), computeBufSRV_vertexoffset[mesh_count].Get(), computeBufSRV_color[mesh_count].Get() };
			ID3D11UnorderedAccessView* pUAVs[1] = { computeBufUAV_result.Get() };
			int loop_count = indexes_count / 32 + 1;
			compute_shader->run(pSRVs, 4, pUAVs, 1, loop_count, 1, 1);

			instance_count += instances_count;
			++mesh_count;
		}

	}
}

void Croth_renderer::render_instancing(Microsoft::WRL::ComPtr<ID3D11Buffer>& instance_buffer_, int bufferStart, int bufferCount) {
	if (material == nullptr) return;
	if (bufferCount == 0) return;

	//::: render ::::::::
	{
		Systems::DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		////
		Systems::SetBlendState(material->BS_state);
		Systems::SetRasterizerState(material->RS_state);
		Systems::SetDephtStencilState(material->DS_state);


		UINT strides[2] = { sizeof(VertexFormat), sizeof(Instance) };
		UINT offsets[2] = { 0, 0 };
		ID3D11Buffer* vbs[2] = { vertexBuffer.Get(), instanceBuffer.Get() };
		Systems::DeviceContext->IASetVertexBuffers(0, 2, vbs, strides, offsets);
		Systems::DeviceContext->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

		// CB : ConstantBufferPerOBJ
		// GOのtransformの情報をConstantBufferへセットする
		ConstantBufferPerGO g_cb;
		g_cb.world = matrix_world(transform->scale, transform->orientation.get_rotate_matrix(), transform->position);
		Systems::DeviceContext->UpdateSubresource(world_cb.Get(), 0, NULL, &g_cb, 0, 0);
		Systems::DeviceContext->VSSetConstantBuffers(0, 1, world_cb.GetAddressOf());
		Systems::DeviceContext->PSSetConstantBuffers(0, 1, world_cb.GetAddressOf());

		ConstantBufferPerMaterial cb;
		// specular
		cb.shininess = 1;
		cb.ambientColor = DirectX::XMFLOAT4(0.1f, 0.1f, 0.1f, 1);
		cb.materialColor = material->color.get_XM4();
		Systems::DeviceContext->UpdateSubresource(Mat_cb.Get(), 0, NULL, &cb, 0, 0);
		Systems::DeviceContext->VSSetConstantBuffers(5, 1, Mat_cb.GetAddressOf());
		Systems::DeviceContext->PSSetConstantBuffers(5, 1, Mat_cb.GetAddressOf());

		// textureをSRVにセット
		material->get_texture()->Set(0);

		Systems::DeviceContext->DrawIndexedInstanced(3, instance_count, 0, 0, 0);

		material->get_texture()->Set(0, false);

	}
}

void Croth_renderer::load_texture(const wchar_t* filename) { material->get_texture()->Load(filename); };

bool Croth_renderer::check_frustum(const Frustum_data& frustum_data) { return true; };

void Croth_renderer::set_meshes(std::vector<Mesh::mesh>* l_meshes) {

	// meshをvertexをmargeする形でloadし直す
	if (l_meshes != nullptr) {
		std::vector<Mesh::mesh>* no_marge_vertex_mesh;
		ResourceManager::CreateModelFromFBX(&no_marge_vertex_mesh, (*l_meshes).at(0).file_pass.c_str(), true);

		// アタッチしたmeshがすでにあるものと違った場合
		if (meshes != no_marge_vertex_mesh) {
			meshes = no_marge_vertex_mesh;

			// bufferの作成
			{
				// 前のものを削除
				for (auto& buf : computeBuf_index)buf.Reset();
				for (auto& buf : computeBuf_vertex)buf.Reset();
				for (auto& buf : computeBuf_vertexoffset)buf.Reset();
				for (auto& buf : computeBuf_color)buf.Reset();

				// アタッチするmeshのサイズだけ確保
				int mesh_size = meshes->size();
				computeBuf_index.resize(mesh_size);
				computeBuf_vertex.resize(mesh_size);
				computeBuf_vertexoffset.resize(mesh_size);
				computeBuf_color.resize(mesh_size);

				// Bufferの作成
				// computeBuf_vertex, computeBuf_indexはここでしか変更しない
				int sum_instance = 0;
				int mesh_count = 0;
				for (Mesh::mesh& mesh : (*meshes))
				{
					int indexes_count = mesh.indexces.size();
					int vertices_count = mesh.vertices.size();

					ComputeShader_function::create_StructureBuffer(sizeof(u_int), indexes_count, &mesh.indexces[0], computeBuf_index[mesh_count], false);
					ComputeShader_function::create_StructureBuffer(sizeof(VertexFormat), vertices_count, &mesh.vertices[0], computeBuf_vertex[mesh_count], false);
					ComputeShader_function::create_StructureBuffer(sizeof(Croth_VertexOffset), vertices_count, nullptr, computeBuf_vertexoffset[mesh_count], true);
					ComputeShader_function::create_StructureBuffer(sizeof(Vector4), 1, nullptr, computeBuf_color[mesh_count], true);

					++mesh_count;
					sum_instance += indexes_count / 3;
				}

				// GPUを(32,1,1)で頂点数分回すため 最大で頂点32overが考えられるため 端数を考慮して(32 / 3 + 1) = 11だけ大きくとる
				ComputeShader_function::create_StructureBuffer(sizeof(Instance_polygon), sum_instance + 11, nullptr, instanceBuffer, false);
			}

			// SRVの作成
			{
				// 前のものを削除
				for (auto& buf : computeBufSRV_index)buf.Reset();
				for (auto& buf : computeBufSRV_vertex)buf.Reset();
				for (auto& buf : computeBufSRV_vertexoffset)buf.Reset();
				for (auto& buf : computeBufSRV_color)buf.Reset();

				// アタッチするmeshのサイズだけ確保
				int mesh_size = meshes->size();
				computeBufSRV_index.resize(mesh_size);
				computeBufSRV_vertex.resize(mesh_size);
				computeBufSRV_vertexoffset.resize(mesh_size);
				computeBufSRV_color.resize(mesh_size);

				int mesh_count = 0;
				for (Mesh::mesh& mesh : (*meshes))
				{
					ComputeShader_function::createSRV_fromSB(computeBuf_index[mesh_count], computeBufSRV_index[mesh_count]);
					ComputeShader_function::createSRV_fromSB(computeBuf_vertex[mesh_count], computeBufSRV_vertex[mesh_count]);
					ComputeShader_function::createSRV_fromSB(computeBuf_vertexoffset[mesh_count], computeBufSRV_vertexoffset[mesh_count]);
					ComputeShader_function::createSRV_fromSB(computeBuf_color[mesh_count], computeBufSRV_color[mesh_count]);
					++mesh_count;
				}
				ComputeShader_function::createUAV_fromSB(instanceBuffer, computeBufUAV_result);
			}

			// offset用配列の準備
			mesh_offset = std::make_shared<std::vector<std::vector<std::pair<DirectX::XMFLOAT3, DirectX::XMFLOAT3>>>>();
			//mesh_offset->resize(meshes->size());
			std::pair<DirectX::XMFLOAT3, DirectX::XMFLOAT3> zero_format;
			zero_format.first = Vector3(0);
			zero_format.second = Vector3(0);

			for (auto& meshess : (*meshes)) {
				std::vector<std::pair<DirectX::XMFLOAT3, DirectX::XMFLOAT3>> initial_mesh;

				initial_mesh.resize(meshess.vertices.size());
				for (auto& mesh : initial_mesh) {
					mesh.first = Vector3(0);
					mesh.second = Vector3(0);
				}

				mesh_offset->emplace_back(initial_mesh);
			}
		}
	}
}
