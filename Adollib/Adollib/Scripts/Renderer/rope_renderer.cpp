
#include "rope_renderer.h"

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



void Rope_renderer::init() {

	// �R���X�^���g�o�b�t�@�̍쐬
	{
		Systems::CreateConstantBuffer(cb_per_rope.ReleaseAndGetAddressOf(), sizeof(ConstantBufferPerRope));
	}

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

	// ����maerial�̐ݒ�
	{
		material = Material_manager::find_material("rope_material");

		if (material == nullptr) {

			material = Material_manager::create_material("rope_material");

			//material->Load_VS("./DefaultShader/rope_shader_vs.cso");
			material->Load_VS("./DefaultShader/default_vs.cso");
			material->Load_PS("./DefaultShader/rope_shader_ps.cso");
		}
	}

	is_use_instancing = false;
}


void Rope_renderer::render(const Frustum_data& frustum_data) {
	//if (material == nullptr) return;

	//Systems::DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//// CB : ConstantBufferPerOBJ
	//// GO��transform�̏���ConstantBuffer�փZ�b�g����
	//ConstantBufferPerGO g_cb;
	//g_cb.world = matrix_world(transform->scale, transform->orientation.get_rotate_matrix(), transform->position);
	//Systems::DeviceContext->UpdateSubresource(world_cb.Get(), 0, NULL, &g_cb, 0, 0);
	//Systems::DeviceContext->VSSetConstantBuffers(0, 1, world_cb.GetAddressOf());
	//Systems::DeviceContext->PSSetConstantBuffers(0, 1, world_cb.GetAddressOf());

	////
	//Systems::SetBlendState(material->BS_state);
	//Systems::SetRasterizerState(material->RS_state);
	//Systems::SetDephtStencilState(material->DS_state);

	//for (Mesh::mesh& mesh : (*meshes))
	//{
	//	// frustum_culling���s��
	//	{
	//		const Vector3& box_position = transform->position;
	//		Vector3 box_axis[3] = {
	//			vector3_quatrotate(Vector3(1,0,0),transform->orientation),
	//			vector3_quatrotate(Vector3(0,1,0),transform->orientation),
	//			vector3_quatrotate(Vector3(0,0,1),transform->orientation)
	//		};

	//		Vector3 dop_average = Vector3(
	//			fabsf(mesh.dop.min[0]) + fabsf(mesh.dop.max[0]),
	//			fabsf(mesh.dop.min[1]) + fabsf(mesh.dop.max[1]),
	//			fabsf(mesh.dop.min[2]) + fabsf(mesh.dop.max[2])
	//		) * 0.5f;

	//		Vector3 dop_center_offset = Vector3(
	//			mesh.dop.min[0] + mesh.dop.max[0],
	//			mesh.dop.min[1] + mesh.dop.max[1],
	//			mesh.dop.min[2] + mesh.dop.max[2]
	//		) * 0.5f;
	//		dop_center_offset = vector3_quatrotate(dop_center_offset, transform->orientation);


	//		bool is_contain = true;
	//		for (int i = 0; i < 6; i++) {

	//			float dis =
	//				fabsf(vector3_dot(frustum_data.normals[i], box_axis[0] * (dop_average[0] * transform->scale[0]))) +
	//				fabsf(vector3_dot(frustum_data.normals[i], box_axis[1] * (dop_average[1] * transform->scale[1]))) +
	//				fabsf(vector3_dot(frustum_data.normals[i], box_axis[2] * (dop_average[2] * transform->scale[2])));

	//			if (frustum_data.distances[i] > vector3_dot(frustum_data.normals[i], box_position + dop_center_offset) + dis) {
	//				is_contain = false;
	//				break;
	//			}
	//		}
	//		if (is_contain == false) continue;


	//	}

	//	UINT stride = sizeof(VertexFormat);
	//	UINT offset = 0;
	//	Systems::DeviceContext->IASetVertexBuffers(0, 1, mesh.vertexBuffer.GetAddressOf(), &stride, &offset);
	//	Systems::DeviceContext->IASetIndexBuffer(mesh.indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);


	//	//CB : ConstantBufferPerMaterial
	//	ConstantBufferPerMaterial cb;
	//	for (int i = 0; i < MAX_BONES; i++) {
	//		cb.boneTransforms[i] = matrix44_identity();
	//	}
	//	{
	//		// boneTransform
	//		if (mesh.skeletalAnimation.size() > 0 && mesh.skeletalAnimation[animeIndex].size() > 0)
	//		{
	//			int frame = (int)(mesh.skeletalAnimation.at(animeIndex).animation_tick / mesh.skeletalAnimation.at(animeIndex).sampling_time);
	//			if (frame > (int)mesh.skeletalAnimation.at(animeIndex).size() - 1)
	//			{
	//				frame = 0;
	//				mesh.skeletalAnimation.at(animeIndex).animation_tick = 0;
	//			}
	//			std::vector<Mesh::bone>& skeletal = mesh.skeletalAnimation.at(animeIndex).at(frame);
	//			size_t number_of_bones = skeletal.size();
	//			_ASSERT_EXPR(number_of_bones < MAX_BONES, L"'the number_of_bones' exceeds MAX_BONES.");
	//			for (size_t i = 0; i < number_of_bones; i++)
	//			{
	//				XMStoreFloat4x4(&cb.boneTransforms[i], DirectX::XMLoadFloat4x4(&skeletal.at(i).transform));
	//			}
	//			mesh.skeletalAnimation.at(animeIndex).animation_tick += 1;
	//		}
	//		// specular
	//		cb.shininess = 1;
	//		cb.ambientColor = DirectX::XMFLOAT4(0.1f, 0.1f, 0.1f, 1);
	//		cb.materialColor = material->color.get_XM4();
	//		//continue;
	//	}
	//	Systems::DeviceContext->UpdateSubresource(Mat_cb.Get(), 0, NULL, &cb, 0, 0);
	//	Systems::DeviceContext->VSSetConstantBuffers(5, 1, Mat_cb.GetAddressOf());
	//	Systems::DeviceContext->PSSetConstantBuffers(5, 1, Mat_cb.GetAddressOf());

	//	//CB : ConstantBufferPerMesh
	//	{
	//		ConstantBuffer::ConstantBufferPerMesh g_cb;
	//		g_cb.Mesh_world = mesh.globalTransform;
	//		Systems::DeviceContext->UpdateSubresource(mesh.mesh_cb.Get(), 0, NULL, &g_cb, 0, 0);
	//		Systems::DeviceContext->VSSetConstantBuffers(3, 1, mesh.mesh_cb.GetAddressOf());
	//		Systems::DeviceContext->PSSetConstantBuffers(3, 1, mesh.mesh_cb.GetAddressOf());
	//	}


	//	for (auto& subset : mesh.subsets)
	//	{
	//		// texture��SRV�ɃZ�b�g
	//		material->get_texture()->Set(0);

	//		// �`��
	//		Systems::DeviceContext->DrawIndexed(subset.indexCount, subset.indexStart, 0);

	//	}
	//}

}

void Rope_renderer::render_instancing(Microsoft::WRL::ComPtr<ID3D11Buffer>& instance_buffer_, int bufferStart, int bufferCount) {
	if (material == nullptr) return;
	if (bufferCount == 0) return;

	// �A�^�b�`����Ă��钸�_�̐�
	const int vertex_size = vertex_offset->size();

	//::: render ::::::::
	{
		ConstantBufferPerRope rope_cb;
		for (int i = 0; i < vertex_size; ++i) {
			rope_cb.joint_position[i] = Vector4(vertex_offset->at(i).first, 1);

			//if (i - 1 < 0) {
			rope_cb.joint_rotate[i] = matrix44_identity();
			//}
		}
		Systems::DeviceContext->UpdateSubresource(cb_per_rope.Get(), 0, NULL, &rope_cb, 0, 0);
		Systems::DeviceContext->VSSetConstantBuffers(9, 1, cb_per_rope.GetAddressOf());
		Systems::DeviceContext->PSSetConstantBuffers(9, 1, cb_per_rope.GetAddressOf());

		Systems::DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		////
		Systems::SetBlendState(material->BS_state);
		Systems::SetRasterizerState(material->RS_state);
		Systems::SetDephtStencilState(material->DS_state);

		// TODO : �����}�e���A���̑Ή�
		// texture��SRV�ɃZ�b�g
		material->get_texture()->Set(0);

		UINT strides = sizeof(VertexFormat);
		UINT offsets = 0;
		ID3D11Buffer* vbs = vertexBuffer.Get();
		Systems::DeviceContext->IASetVertexBuffers(0, 1, &vbs, &strides, &offsets);
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

		const int facet_count = (vertex_size - 1) * split_count * 2 + (split_count * 2);
		const int index_count = facet_count * 3;
		Systems::DeviceContext->DrawIndexedInstanced(index_count, 1, 0, 0, 0);

	}
}

void Rope_renderer::load_texture(const wchar_t* filename) { material->get_texture()->Load(filename); };

bool Rope_renderer::check_frustum(const Frustum_data& frustum_data) { return true; };

void Rope_renderer::set_meshoffset(std::shared_ptr<std::vector<std::pair<Vector3, Vector3>>> off) {

	// ���łɃA�^�b�`���ꂽptr�ƈႤ�̂ł����
	if (vertex_offset != off) {
		vertex_offset = off;

		// �A�^�b�`�����offset��size����vertex_buffer�̃T�C�Y�̕ύX�Ȃǂ��s��
		int sphere_count = off->size();
		//(sphere�̐� - 1) * ������ * 2(�e�ʂ�3�p�`��2������)
		//int facet_count = (sphere_count - 1) * split_count * 2;

		//	���_�o�b�t�@�쐬
		{
			// base�̒��_���W�̌v�Z
			// �ʖ@����(1,0,0)�̉~�����
			std::vector<VertexFormat> vertex_pos;
			vertex_pos.resize(split_count);
			for (int i = 0; i < split_count; ++i)
			{
				const auto radian = PI * 2 * ((float)i / split_count);

				vertex_pos[i].normal = Vector3(0, sinf(radian), cosf(radian)).unit_vect();
				vertex_pos[i].position = vertex_pos[i].normal * radius;

			}

			// vertex_format�̍쐬
			const int vertex_fotmat_size = sphere_count * split_count + 2;
			std::vector<VertexFormat> v;
			v.resize(vertex_fotmat_size); //rope�̒[�����̒��S�_
			{
				for (int i = 0; i < vertex_fotmat_size; ++i) {
					// �[�����̓_�̏��͓K���ɓ����
					if (i == 0) {
						v[i].position = Vector3(-radius * 0.2f, 0, 0); //������ƂƂ����Ă���
						v[i].normal = Vector3(-1, 0, 0);
						v[i].bone_indices[0] = 0; //�Ή�����sphere_num������
						continue;
					}
					if (i == vertex_fotmat_size - 1) {
						v[i].position = Vector3(+radius * 0.2f, 0, 0); //������ƂƂ����Ă���
						v[i].normal = Vector3(+1, 0, 0);
						v[i].bone_indices[0] = sphere_count - 1; //�Ή�����sphere_num������
						continue;
					}
					// 0�ɂ͒[�����̏�񂪓����Ă��邽��i-1
					v[i] = vertex_pos[(i - 1) % split_count];
					v[i].bone_indices[0] = (i - 1) / split_count;
				}
			}

			D3D11_BUFFER_DESC bd;
			ZeroMemory(&bd, sizeof(bd));
			bd.Usage = D3D11_USAGE_DEFAULT;
			bd.ByteWidth = sizeof(VertexFormat) * vertex_fotmat_size;
			bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

			D3D11_SUBRESOURCE_DATA res;
			ZeroMemory(&res, sizeof(res));
			res.pSysMem = &v[0];

			Systems::Device->CreateBuffer(&bd, &res, vertexBuffer.ReleaseAndGetAddressOf());
		}

		// index�o�b�t�@�쐬
		{
			std::vector<u_int> indices;
			// (sphere�̐� - 1) * ������ * 2(�e�ʂ�3�p�`��2������)
			// �[�����̂ӂ��̕���(split_count * 2)
			const int facet_count = (sphere_count - 1) * split_count * 2 + (split_count * 2);
			const int index_count = facet_count * 3;
			indices.resize(index_count);

			int index_num = 0;
			for (int sphere_num = 0; sphere_num < sphere_count; ++sphere_num) {
				// �[�����̕���
				if (sphere_num == 0) {
					for (int vertex_num = 0; vertex_num < split_count; ++vertex_num) {
						indices[index_num + 0] = 0;
						indices[index_num + 1] = 0 + 1 + vertex_num;
						indices[index_num + 2] = 0 + 1 + (vertex_num + 1) % split_count;
						index_num += 3;
					}
					continue;
				}

				const int sphere_ID_0 = sphere_num - 1;
				const int sphere_ID_1 = sphere_num;

				for (int vertex_num = 0; vertex_num < split_count; ++vertex_num) {

					indices[index_num + 0] = 1 + sphere_ID_0 * split_count + vertex_num;
					indices[index_num + 1] = 1 + sphere_ID_1 * split_count + vertex_num;
					indices[index_num + 2] = 1 + sphere_ID_0 * split_count + (vertex_num + 1) % split_count;
					index_num += 3;

					indices[index_num + 0] = 1 + sphere_ID_0 * split_count + (vertex_num + 1) % split_count;
					indices[index_num + 1] = 1 + sphere_ID_1 * split_count + vertex_num;
					indices[index_num + 2] = 1 + sphere_ID_1 * split_count + (vertex_num + 1) % split_count;
					index_num += 3;
				}

				// �[�����̕���
				if (sphere_num == sphere_count - 1) {
					for (int vertex_num = 0; vertex_num < split_count; ++vertex_num) {
						indices[index_num + 2] = sphere_count * split_count + 2 - 1;
						indices[index_num + 1] = 1 + (sphere_count - 1) * split_count + vertex_num;
						indices[index_num + 0] = 1 + (sphere_count - 1) * split_count + (vertex_num + 1) % split_count;
						index_num += 3;
					}
				}
			}

			D3D11_BUFFER_DESC indexDesc = {};
			indexDesc.ByteWidth = index_count * sizeof(u_int);          // �o�b�t�@�̃T�C�Y
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

		//// offset�p�z��̏���
		//{
		//	vertex_offset = std::make_shared<std::vector<std::pair<Vector3, Vector3>>>();
		//	//mesh_offset->resize(meshes->size());
		//	std::pair<Vector3, Vector3> zero_format;
		//	zero_format.first = Vector3(0);
		//	zero_format.second = Vector3(0);

		//	vertex_offset->resize(sphere_count);
		//	for (auto& sphere : *vertex_offset.get()) {
		//		sphere.first = Vector3(0);
		//		sphere.second = Vector3(0);
		//	}
		//}



	}


}