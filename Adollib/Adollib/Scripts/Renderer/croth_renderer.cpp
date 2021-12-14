
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

#

using namespace Adollib;
using namespace ConstantBuffer;
using namespace Compute_S;


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

		Systems::Device->CreateBuffer(&bd, &res, vertexBuffer.GetAddressOf());
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

		Systems::Device->CreateBuffer(&indexDesc, &indexSubResource, indexBuffer.GetAddressOf());

	}

	// instanceバッファ生成
	{
		constexpr int maxElements = 300000;
		HRESULT hr = S_OK;
		std::shared_ptr<Instance> instances(new Instance[maxElements]);
		{
			D3D11_BUFFER_DESC bd = {};
			bd.ByteWidth = sizeof(Instance) * maxElements;	// インスタンスバッファのサイズ
			// GPU->読み取りのみ　CPU->書き込みのみ
			bd.Usage = D3D11_USAGE_DYNAMIC;				// バッファの読み書き方法
			bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;	// バッファのパイプラインへのバインド方式
			bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;	// リソースに許可されるCPUアクセスの指定
			bd.MiscFlags = 0;
			bd.StructureByteStride = 0;

			D3D11_SUBRESOURCE_DATA initData = {};
			initData.pSysMem = instances.get();	// 頂点のアドレス
			initData.SysMemPitch = 0;		//Not use for vertex buffers.mm
			initData.SysMemSlicePitch = 0;	//Not use for vertex buffers.
			hr = Systems::Device->CreateBuffer(&bd, &initData, instanceBuffer.GetAddressOf());
			if (FAILED(hr))
			{
				assert(0 && "failed create instance buffer dynamic(render_manager)");
				return;
			}
		}
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
	g_cb.world = matrix_world(transform->scale, transform->orientation.get_rotate_matrix(), transform->position);
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

void Croth_renderer::render_instancing(Microsoft::WRL::ComPtr<ID3D11Buffer>& instance_buffer_, int bufferStart, int bufferCount) {
	if (material == nullptr) return;
	if (bufferCount == 0) return;

	/*
	//::: update_instancing ::::::::
	//{
	//	static float debug_timer_count = 0;
	//	debug_timer_count += time->deltaTime();

	//	instance_count = 0;
	//	Instance_polygon* instances = nullptr;

	//	// instance_bufferをMapする
	//	HRESULT hr = S_OK;
	//	const D3D11_MAP map = D3D11_MAP_WRITE_DISCARD;
	//	D3D11_MAPPED_SUBRESOURCE mappedBuffer = {};
	//	hr = Systems::DeviceContext->Map(instanceBuffer.Get(), 0, map, 0, &mappedBuffer);

	//	if (FAILED(hr))
	//	{
	//		assert(0 && "failed Map InstanceBuffer dynamic(RenderManager)");
	//		return;
	//	}
	//	instances = static_cast<Instance_polygon*>(mappedBuffer.pData);

	//	// meshから
	//	for (Mesh::mesh& mesh : (*meshes))
	//	{
	//		int index_count = 0;
	//		for (const auto& index : mesh.indexces) {

	//			if (index_count == 3) {
	//				index_count = 0;

	//				++instance_count;
	//			}

	//			auto& instance = instances[instance_count];

	//			instance.position[index_count] = Vector3(mesh.vertices[index].position)
	//				+ Vector3(0, 1, 0) * 0.5f * cosf(debug_timer_count + index)
	//				+ Vector3(0, 0, 1) * 0.5f * sinf(debug_timer_count + index);

	//			instance.normal[index_count] = Vector3(mesh.vertices[index].normal);
	//			instance.color = color;
	//			instance.texcoordTransform = Vector2(0, 0);

	//			++index_count;
	//		}
	//		++instance_count;

	//		// ポリゴンの途中で終わっていたらエラー
	//		if (index_count != 3) {
	//			assert(0 && "bug");
	//		}
	//	}

	//	// instance_bufferをUnmapする
	//	Systems::DeviceContext->Unmap(instanceBuffer.Get(), 0);

	//}
	*/

	{
		static float debug_timer_count = 0;
		debug_timer_count += time->deltaTime();
		instance_count = 0;

		// meshから
		for (Mesh::mesh& mesh : (*meshes))
		{
			int indexes_count = mesh.indexces.size();
			int vertices_count = mesh.vertices.size();
			int instances_count = indexes_count / 3;

			// structure bufferの作成
			compute_shader->create_StructureBuffer(sizeof(u_int),			 indexes_count, &mesh.indexces[0], computeBuf_index);
			compute_shader->create_StructureBuffer(sizeof(VertexFormat),	 vertices_count, &mesh.vertices[0], computeBuf_vertex);
			compute_shader->create_StructureBuffer(sizeof(Vector4), 1, &color, computeBuf_color);
			compute_shader->create_StructureBuffer(sizeof(Instance_polygon), instances_count, nullptr, computeBuf_result);

			compute_shader->createSRV_fromSB(computeBuf_index,  computeBufSRV_index.GetAddressOf());
			compute_shader->createSRV_fromSB(computeBuf_vertex, computeBufSRV_vertex.GetAddressOf());
			compute_shader->createSRV_fromSB(computeBuf_color,  computeBufSRV_color.GetAddressOf());
			compute_shader->createUAV_fromSB(computeBuf_result, computeBufUAV_result.GetAddressOf());

			ID3D11ShaderResourceView* pSRVs[3] = { computeBufSRV_index.Get(), computeBufSRV_vertex.Get(), computeBufSRV_color.Get() };

			int loop_count = indexes_count / 64 + 1;
			compute_shader->run(pSRVs, 3, computeBufUAV_result, loop_count, 1, 1);

			auto instance_poly = compute_shader->Read_UAV<Instance_polygon>(computeBuf_result);

			Instance_polygon* instances = nullptr;
			// instance_bufferをMapする
			HRESULT hr = S_OK;
			const D3D11_MAP map = D3D11_MAP_WRITE_DISCARD;
			D3D11_MAPPED_SUBRESOURCE mappedBuffer = {};
			hr = Systems::DeviceContext->Map(instanceBuffer.Get(), 0, map, 0, &mappedBuffer);

			if (FAILED(hr))
			{
				assert(0 && "failed Map InstanceBuffer dynamic(RenderManager)");
				return;
			}

			instances = static_cast<Instance_polygon*>(mappedBuffer.pData);
			for (int i = 0; i < instances_count; ++i) {
				instances[i + instance_count] = instance_poly[i];
			}

			// instance_bufferをUnmapする
			Systems::DeviceContext->Unmap(instanceBuffer.Get(), 0);

			instance_count += instances_count;
		}

	}

	//::: render ::::::::
	{

		Systems::DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		////
		Systems::SetBlendState(material->BS_state);
		Systems::SetRasterizerState(material->RS_state);
		Systems::SetDephtStencilState(material->DS_state);

		// TODO : 複数マテリアルの対応
		// textureをSRVにセット
		material->get_texture()->Set(0);

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


		Systems::DeviceContext->DrawIndexedInstanced(3, instance_count, 0, 0, 0);

	}
}

void Croth_renderer::load_texture(const wchar_t* filename) { material->get_texture()->Load(filename); };

bool Croth_renderer::check_frustum(const Frustum_data& frustum_data) { return true; };

void Croth_renderer::set_meshes(std::vector<Mesh::mesh>* l_meshes) {

	// meshをvertexをmargeする形でloadし直す
	if (l_meshes != nullptr) {
		std::vector<Mesh::mesh>* no_marge_vertex_mesh;
		ResourceManager::CreateModelFromFBX(&no_marge_vertex_mesh, (*l_meshes).at(0).file_pass.c_str(), true);
		meshes = no_marge_vertex_mesh;
	}

}

void Croth_renderer::instance_update(const Frustum_data& frustum_data) {

	Instance_polygon* instances = nullptr;

	// instance_bufferをMapする
	HRESULT hr = S_OK;
	const D3D11_MAP map = D3D11_MAP_WRITE_DISCARD;
	D3D11_MAPPED_SUBRESOURCE mappedBuffer = {};
	hr = Systems::DeviceContext->Map(instanceBuffer.Get(), 0, map, 0, &mappedBuffer);

	if (FAILED(hr))
	{
		assert(0 && "failed Map InstanceBuffer dynamic(RenderManager)");
		return;
	}
	instances = static_cast<Instance_polygon*>(mappedBuffer.pData);

	int instance_num = 0;
	int index_count = 0;

	// meshから
	for (Mesh::mesh& mesh : (*meshes))
	{

		for (const auto& index : mesh.indexces) {

			if (index_count == 3) {
				index_count = 0;

				++instance_num;
			}

			auto& instance = instances[instance_num];


			instance.position[index_count] = mesh.vertices[index].position /* + offser */;
			instance.color = color;
			//instance.normal = ;

			++index_count;
		}

	}

	// instance_bufferをUnmapする
	Systems::DeviceContext->Unmap(instanceBuffer.Get(), 0);

}