
#include "sprite_renderer.h"

#include "../Object/gameobject.h"
#include "renderer_manager.h"
#include "material_manager.h"

#include "../Main/systems.h"
#include "Shader/constant_buffer.h"
#include "Shader/vertex_format.h"

#include "material.h"
#include "texture.h"

using namespace Adollib;
using namespace ConstantBuffer;

void Sprite_renderer::init() {

	//	���_�o�b�t�@�쐬
	{
		VertexFormat v[4];

		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(VertexFormat) * 4;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA res;
		ZeroMemory(&res, sizeof(res));
		res.pSysMem = v;

		Systems::Device->CreateBuffer(&bd, &res, VertexBuffer.ReleaseAndGetAddressOf());
	}
	// index�o�b�t�@�쐬
	{
		u_int indices[4] = { 0,1,2,3 };
		D3D11_BUFFER_DESC indexDesc = {};
		indexDesc.ByteWidth = 4 * sizeof(u_int);          // �o�b�t�@�̃T�C�Y
		indexDesc.Usage = D3D11_USAGE_IMMUTABLE;	          // �o�b�t�@�̓ǂݏ����@
		indexDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;    // �p�C�v���C���ɂǂ��o�C���h���邩�w��
		indexDesc.CPUAccessFlags = 0;                    // CPU�̃A�N�Z�X�t���O�@0�ŃA�N�Z�X���Ȃ�
		indexDesc.MiscFlags = 0;                           // ���̑��̃t���O
		indexDesc.StructureByteStride = 0;                 // �o�b�t�@�\���̂̏ꍇ�̗v�f��

		D3D11_SUBRESOURCE_DATA indexSubResource = {};
		indexSubResource.pSysMem = &indices[0];   // �������f�[�^�̃|�C���^
		indexSubResource.SysMemPitch = 0;        // ���_�o�b�t�@�ł͎g��Ȃ�
		indexSubResource.SysMemSlicePitch = 0;   // ���_�o�b�t�@�ł͎g��Ȃ�

		Systems::Device->CreateBuffer(&indexDesc, &indexSubResource, IndexBuffer.ReleaseAndGetAddressOf());

	}


	{
		material = Material_manager::find_material("sprite_material");

		if (material == nullptr) {

			material = Material_manager::create_material("sprite_material");

			material->Load_VS("./DefaultShader/sprite_vs.cso");
			material->Load_PS("./DefaultShader/sprite_ps.cso");

			material->BS_state = State_manager::BStypes::BS_ALPHA;
		}
	}
}

void Sprite_renderer::render(const Frustum_data& frustum_data) {
	if (material == nullptr) return;

	// CB : ConstantBufferPerOBJ
	// GO��transform�̏���ConstantBuffer�փZ�b�g����
	ConstantBufferPerGO g_cb;
	g_cb.world = matrix_world(transform->scale, transform->orientation.get_rotate_matrix(), transform->position);
	Systems::DeviceContext->UpdateSubresource(world_cb.Get(), 0, NULL, &g_cb, 0, 0);
	Systems::DeviceContext->VSSetConstantBuffers(0, 1, world_cb.GetAddressOf());
	Systems::DeviceContext->PSSetConstantBuffers(0, 1, world_cb.GetAddressOf());

	//CB : ConstantBufferPerMaterial
	ConstantBufferPerMaterial cb;
	// specular
	cb.shininess = 1;
	cb.ambientColor = DirectX::XMFLOAT4(0.1f, 0.1f, 0.1f, 1);
	cb.materialColor = material->color.get_XM4();

	Systems::DeviceContext->UpdateSubresource(Mat_cb.Get(), 0, NULL, &cb, 0, 0);
	Systems::DeviceContext->VSSetConstantBuffers(5, 1, Mat_cb.GetAddressOf());
	Systems::DeviceContext->PSSetConstantBuffers(5, 1, Mat_cb.GetAddressOf());

	//
	Systems::SetBlendState(material->BS_state);
	Systems::SetRasterizerState(material->RS_state);
	Systems::SetDephtStencilState(material->DS_state);

	VertexFormat data[4];
	// ���_
	data[0].position = Vector3(-1, +1, 0);
	data[1].position = Vector3(-1, -1, 0);
	data[2].position = Vector3(+1, +1, 0);
	data[3].position = Vector3(+1, -1, 0);

	// uv���W
	data[0].texcoord = uv_pos[0];
	data[1].texcoord = uv_pos[1];
	data[2].texcoord = uv_pos[2];
	data[3].texcoord = uv_pos[3];

	// �@��
	data[0].normal = Vector3(0, 0, 1);
	data[1].normal = Vector3(0, 0, 1);
	data[2].normal = Vector3(0, 0, 1);
	data[3].normal = Vector3(0, 0, 1);

	// ���_�f�[�^�X�V
	Systems::DeviceContext->UpdateSubresource(VertexBuffer.Get(), 0, NULL, data, 0, 0);

	// ���_�o�b�t�@�̎w��
	UINT stride = sizeof(VertexFormat);
	UINT offset = 0;
	Systems::DeviceContext->IASetVertexBuffers(0, 1, VertexBuffer.GetAddressOf(), &stride, &offset);

	Systems::DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	//�e�N�X�`���̐ݒ�
	if (material->get_texture()) material->get_texture()->Set(0);

	// �`��
	Systems::DeviceContext->Draw(4, 0);

}

void Sprite_renderer::render_instancing(Microsoft::WRL::ComPtr<ID3D11Buffer>& instance_buffer, int bufferStart, int bufferCount) {
	if (material == nullptr) return;

	// CB : ConstantBufferPerOBJ
	// GO��transform�̏���ConstantBuffer�փZ�b�g����
	ConstantBufferPerGO g_cb;
	g_cb.world = matrix_world(transform->scale, transform->orientation.get_rotate_matrix(), transform->position);
	Systems::DeviceContext->UpdateSubresource(world_cb.Get(), 0, NULL, &g_cb, 0, 0);
	Systems::DeviceContext->VSSetConstantBuffers(0, 1, world_cb.GetAddressOf());
	Systems::DeviceContext->PSSetConstantBuffers(0, 1, world_cb.GetAddressOf());

	//CB : ConstantBufferPerMaterial
	ConstantBufferPerMaterial cb;
	// specular
	cb.shininess = 1;
	cb.ambientColor = DirectX::XMFLOAT4(0.1f, 0.1f, 0.1f, 1);
	cb.materialColor = material->color.get_XM4();

	Systems::DeviceContext->UpdateSubresource(Mat_cb.Get(), 0, NULL, &cb, 0, 0);
	Systems::DeviceContext->VSSetConstantBuffers(5, 1, Mat_cb.GetAddressOf());
	Systems::DeviceContext->PSSetConstantBuffers(5, 1, Mat_cb.GetAddressOf());

	//
	Systems::SetBlendState(material->BS_state);
	Systems::SetRasterizerState(material->RS_state);
	Systems::SetDephtStencilState(material->DS_state);

	VertexFormat data[4];
	// ���_
	data[0].position = Vector3(-1, +1, 0);
	data[1].position = Vector3(-1, -1, 0);
	data[2].position = Vector3(+1, +1, 0);
	data[3].position = Vector3(+1, -1, 0);

	// uv���W
	data[0].texcoord = uv_pos[0];
	data[1].texcoord = uv_pos[1];
	data[2].texcoord = uv_pos[2];
	data[3].texcoord = uv_pos[3];

	// �@��
	data[0].normal = Vector3(0, 0, 1);
	data[1].normal = Vector3(0, 0, 1);
	data[2].normal = Vector3(0, 0, 1);
	data[3].normal = Vector3(0, 0, 1);

	// ���_�f�[�^�X�V
	Systems::DeviceContext->UpdateSubresource(VertexBuffer.Get(), 0, NULL, data, 0, 0);

	// ���_�o�b�t�@�̎w��
	//UINT stride = sizeof(VertexFormat);
	//UINT offset = 0;
	//Systems::DeviceContext->IASetVertexBuffers(0, 1, VertexBuffer.GetAddressOf(), &stride, &offset);

	UINT strides[2] = { sizeof(VertexFormat), sizeof(Instance) };
	UINT offsets[2] = { 0, 0 };
	ID3D11Buffer* vbs[2] = { VertexBuffer.Get(), instance_buffer.Get() };
	Systems::DeviceContext->IASetVertexBuffers(0, 2, vbs, strides, offsets);
	Systems::DeviceContext->IASetIndexBuffer(IndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	Systems::DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	//�e�N�X�`���̐ݒ�
	if (material->get_texture()) material->get_texture()->Set(0);

	// �`��
	//Systems::DeviceContext->Draw(4, 0);
	Systems::DeviceContext->DrawIndexedInstanced(4, bufferCount, 0, 0, bufferStart);

	if (material->get_texture()) material->get_texture()->Set(0, false);

}

void Sprite_renderer::load_texture(const wchar_t* filename) { material->get_texture()->Load(filename); };