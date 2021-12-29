
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

template<class T>
T* map_buffer(Microsoft::WRL::ComPtr<ID3D11Buffer>& buffer) {
	HRESULT hr = S_OK;
	const D3D11_MAP map = D3D11_MAP_WRITE_DISCARD;
	D3D11_MAPPED_SUBRESOURCE mappedBuffer = {};
	hr = Systems::DeviceContext->Map(buffer.Get(), 0, map, 0, &mappedBuffer);

	if (FAILED(hr))
	{
		assert(0 && "failed Map InstanceBuffer dynamic(RenderManager)");
		return nullptr;
	}
	return static_cast<T*>(mappedBuffer.pData);
}
void unmap_buffer(Microsoft::WRL::ComPtr<ID3D11Buffer>& buffer) {
	Systems::DeviceContext->Unmap(buffer.Get(), 0);
}


void Croth_renderer::init() {

	//	���_�o�b�t�@�쐬
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

	// index�o�b�t�@�쐬
	{
		u_int indices[3] = { 0,1,2 };
		D3D11_BUFFER_DESC indexDesc = {};
		indexDesc.ByteWidth = 3 * sizeof(u_int);          // �o�b�t�@�̃T�C�Y
		indexDesc.Usage = D3D11_USAGE_IMMUTABLE;	          // �o�b�t�@�̓ǂݏ����@
		indexDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;    // �p�C�v���C���ɂǂ��o�C���h���邩�w��
		indexDesc.CPUAccessFlags = 0;                    // CPU�̃A�N�Z�X�t���O�@0�ŃA�N�Z�X���Ȃ�
		indexDesc.MiscFlags = 0;                           // ���̑��̃t���O
		indexDesc.StructureByteStride = 0;                 // �o�b�t�@�\���̂̏ꍇ�̗v�f��

		D3D11_SUBRESOURCE_DATA indexSubResource = {};
		indexSubResource.pSysMem = &indices[0];   // �������f�[�^�̃|�C���^
		indexSubResource.SysMemPitch = 0;        // ���_�o�b�t�@�ł͎g��Ȃ�
		indexSubResource.SysMemSlicePitch = 0;   // ���_�o�b�t�@�ł͎g��Ȃ�

		Systems::Device->CreateBuffer(&indexDesc, &indexSubResource, indexBuffer.ReleaseAndGetAddressOf());

	}

	// computeshader��load
	{
		compute_shader = std::make_shared<ComputeShader>();
		compute_shader->Load("./DefaultShader/croth_calculate_cs.cso");
	}


	// ���ł�mesh�����݂���� ����mesh��vertex��marge����`��load������
	{
		if (meshes != nullptr) {
			std::vector<Mesh::mesh>* no_marge_vertex_mesh;
			ResourceManager::CreateModelFromFBX(&no_marge_vertex_mesh, (*meshes).at(0).file_pass.c_str(), true);
			meshes = no_marge_vertex_mesh;
		}
	}

	// ����maerial�̐ݒ�
	{
		material = Material_manager::find_material("croth_material");

		if (material == nullptr) {

			material = Material_manager::create_material("croth_material");

			material->Load_VS("./DefaultShader/croth_shader_vs.cso");
			material->Load_PS("./DefaultShader/croth_shader_ps.cso");

			material->RS_state = State_manager::RStypes::RS_CULL_NONE;
		}
	}

	is_use_instancing = false;
}


void Croth_renderer::render(const Frustum_data& frustum_data) {
	if (material == nullptr) return;

	Systems::DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// CB : ConstantBufferPerOBJ
	// GO��transform�̏���ConstantBuffer�փZ�b�g����
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
		// frustum_culling���s��
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
			// texture��SRV�ɃZ�b�g
			material->get_texture()->Set(0);

			// �`��
			Systems::DeviceContext->DrawIndexed(subset.indexCount, subset.indexStart, 0);

		}
	}

}

void Croth_renderer::render_instancing(Microsoft::WRL::ComPtr<ID3D11Buffer>& instance_buffer_, int bufferStart, int bufferCount) {
	if (material == nullptr) return;
	if (bufferCount == 0) return;

	{
		int mesh_count = 0;
		for (auto& buffer : computeBuf_vertexoffset) {

			auto data = map_buffer<VertexOffset>(buffer);

			int vertex_size = meshes->at(mesh_count).vertices.size();
			int vertex_start = 0;
			for (int i = 0; i < vertex_size; ++i) {
				data[i].position = mesh_offset->at(mesh_count)[i].first;
				data[i].normal = mesh_offset->at(mesh_count)[i].second;
			}
			++mesh_count;
			unmap_buffer(buffer);
		}
	}

	// color���̍X�V (instance_count��w�ɓ���Ă���)
	{

		int mesh_count = 0;
		int instance_count = 0;
		for (auto& buffer : computeBuf_color) {

			auto data = map_buffer<Vector4>(buffer);

			data[0] = Vector4(color.x, color.y, color.z, instance_count);

			unmap_buffer(buffer);

			instance_count += meshes->at(mesh_count).indexces.size() / 3;
			++mesh_count;
		}

	}

	{
		instance_count = 0;

		// mesh����
		int mesh_count = 0;
		for (Mesh::mesh& mesh : (*meshes))
		{
			int indexes_count = mesh.indexces.size();
			int vertices_count = mesh.vertices.size();
			int instances_count = indexes_count / 3;

			// compute shader�𑖂点��
			ID3D11ShaderResourceView* pSRVs[4] = { computeBufSRV_index[mesh_count].Get(), computeBufSRV_vertex[mesh_count].Get(), computeBufSRV_vertexoffset[mesh_count].Get(), computeBufSRV_color[mesh_count].Get() };

			int loop_count = indexes_count / 32 + 1;
			compute_shader->run(pSRVs, 4, computeBufUAV_result, loop_count, 1, 1);

			instance_count += instances_count;
			++mesh_count;
		}

	}

	//::: render ::::::::
	{

		Systems::DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		////
		Systems::SetBlendState(material->BS_state);
		Systems::SetRasterizerState(material->RS_state);
		Systems::SetDephtStencilState(material->DS_state);

		// TODO : �����}�e���A���̑Ή�
		// texture��SRV�ɃZ�b�g
		material->get_texture()->Set(0);

		UINT strides[2] = { sizeof(VertexFormat), sizeof(Instance) };
		UINT offsets[2] = { 0, 0 };
		ID3D11Buffer* vbs[2] = { vertexBuffer.Get(), instanceBuffer.Get() };
		Systems::DeviceContext->IASetVertexBuffers(0, 2, vbs, strides, offsets);
		Systems::DeviceContext->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

		// CB : ConstantBufferPerOBJ
		// GO��transform�̏���ConstantBuffer�փZ�b�g����
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

	// mesh��vertex��marge����`��load������
	if (l_meshes != nullptr) {
		std::vector<Mesh::mesh>* no_marge_vertex_mesh;
		ResourceManager::CreateModelFromFBX(&no_marge_vertex_mesh, (*l_meshes).at(0).file_pass.c_str(), true);

		// �A�^�b�`����mesh�����łɂ�����̂ƈ�����ꍇ
		if (meshes != no_marge_vertex_mesh) {
			meshes = no_marge_vertex_mesh;

			// buffer�̍쐬
			{
				// �O�̂��̂��폜
				for (auto& buf : computeBuf_index)buf.Reset();
				for (auto& buf : computeBuf_vertex)buf.Reset();
				for (auto& buf : computeBuf_vertexoffset)buf.Reset();
				for (auto& buf : computeBuf_color)buf.Reset();

				// �A�^�b�`����mesh�̃T�C�Y�����m��
				int mesh_size = meshes->size();
				computeBuf_index.resize(mesh_size);
				computeBuf_vertex.resize(mesh_size);
				computeBuf_vertexoffset.resize(mesh_size);
				computeBuf_color.resize(mesh_size);

				// Buffer�̍쐬
				// computeBuf_vertex, computeBuf_index�͂����ł����ύX���Ȃ�
				int sum_instance = 0;
				int mesh_count = 0;
				for (Mesh::mesh& mesh : (*meshes))
				{
					int indexes_count = mesh.indexces.size();
					int vertices_count = mesh.vertices.size();

					compute_shader->create_StructureBuffer(sizeof(u_int), indexes_count, &mesh.indexces[0], computeBuf_index[mesh_count], false);
					compute_shader->create_StructureBuffer(sizeof(VertexFormat), vertices_count, &mesh.vertices[0], computeBuf_vertex[mesh_count], false);
					compute_shader->create_StructureBuffer(sizeof(VertexOffset), vertices_count, nullptr, computeBuf_vertexoffset[mesh_count], true);
					compute_shader->create_StructureBuffer(sizeof(Vector4), 1, nullptr, computeBuf_color[mesh_count], true);

					++mesh_count;
					sum_instance += indexes_count / 3;
				}
				compute_shader->create_StructureBuffer(sizeof(Instance_polygon), sum_instance, nullptr, instanceBuffer, false);
			}

			// SRV�̍쐬
			{
				// �O�̂��̂��폜
				for (auto& buf : computeBufSRV_index)buf.Reset();
				for (auto& buf : computeBufSRV_vertex)buf.Reset();
				for (auto& buf : computeBufSRV_vertexoffset)buf.Reset();
				for (auto& buf : computeBufSRV_color)buf.Reset();

				// �A�^�b�`����mesh�̃T�C�Y�����m��
				int mesh_size = meshes->size();
				computeBufSRV_index.resize(mesh_size);
				computeBufSRV_vertex.resize(mesh_size);
				computeBufSRV_vertexoffset.resize(mesh_size);
				computeBufSRV_color.resize(mesh_size);

				int mesh_count = 0;
				for (Mesh::mesh& mesh : (*meshes))
				{
					compute_shader->createSRV_fromSB(computeBuf_index[mesh_count], computeBufSRV_index[mesh_count]);
					compute_shader->createSRV_fromSB(computeBuf_vertex[mesh_count], computeBufSRV_vertex[mesh_count]);
					compute_shader->createSRV_fromSB(computeBuf_vertexoffset[mesh_count], computeBufSRV_vertexoffset[mesh_count]);
					compute_shader->createSRV_fromSB(computeBuf_color[mesh_count], computeBufSRV_color[mesh_count]);
					++mesh_count;
				}
				compute_shader->createUAV_fromSB(instanceBuffer, computeBufUAV_result);
			}

			// offset�p�z��̏���
			mesh_offset = std::make_shared<std::vector<std::vector<std::pair<Vector3, Vector3>>>>();
			//mesh_offset->resize(meshes->size());
			std::pair<Vector3, Vector3> zero_format;
			zero_format.first = Vector3(0);
			zero_format.second = Vector3(0);

			for (auto& meshess : (*meshes)) {
				std::vector<std::pair<Vector3, Vector3>> initial_mesh;

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

void Croth_renderer::instance_update(const Frustum_data& frustum_data) {

	Instance_polygon* instances = nullptr;

	// instance_buffer��Map����
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

	// mesh����
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

	// instance_buffer��Unmap����
	Systems::DeviceContext->Unmap(instanceBuffer.Get(), 0);

}