
#include "UI_renderer.h"

#include "../Object/gameobject.h"
#include "renderer_manager.h"
#include "material_manager.h"

#include "../Main/systems.h"
#include "Shader/constant_buffer.h"
#include "Shader/vertex_format.h"

#include "material.h"
#include "texture.h"
#include "UI.h"

using namespace Adollib;
using namespace ConstantBuffer;

void UI_renderer::init() {

	//	���_�o�b�t�@�쐬
	{
		VertexFormat v[4];
		v[0].position = Vector3(-0.5f, -0.5f, 0);
		v[1].position = Vector3(+0.5f, -0.5f, 0);
		v[2].position = Vector3(-0.5f, +0.5f, 0);
		v[3].position = Vector3(+0.5f, +0.5f, 0);

		v[0].normal = Vector3(0, 0, 1);
		v[1].normal = Vector3(0, 0, 1);
		v[2].normal = Vector3(0, 0, 1);
		v[3].normal = Vector3(0, 0, 1);

		v[0].texcoord = Vector2(0, 1);
		v[1].texcoord = Vector2(1, 1);
		v[2].texcoord = Vector2(0, 0);
		v[3].texcoord = Vector2(1, 0);

		//	���_�o�b�t�@�쐬
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

	////�f�v�X�X�e���V���X�e�[�g
	//{
	//	D3D11_DEPTH_STENCIL_DESC depth_stencil_desc;
	//	ZeroMemory(&depth_stencil_desc, sizeof(depth_stencil_desc));
	//	depth_stencil_desc.DepthEnable = FALSE;
	//	Systems::Device->CreateDepthStencilState(&depth_stencil_desc, DepthStencilState.GetAddressOf());
	//}

	// material
	{
		material = Material_manager::find_material("ui_material");

		if (material == nullptr) {

			material = Material_manager::create_material("ui_material");

			material->Load_VS("./DefaultShader/ui_vs.cso");
			material->Load_PS("./DefaultShader/ui_ps.cso");

			material->BS_state = State_manager::BStypes::BS_ALPHA;
			material->RS_state = State_manager::RStypes::RS_CULL_NONE;
			material->DS_state = State_manager::DStypes::DS_FALSE;
		}
	}

	is_use_instancing = false;
}

void UI_renderer::render(const Frustum_data& frustum_data) {
	//if (material == nullptr) return;

	//UI ui;
	//ui.ui_data.dh = Al_Global::SCREEN_HEIGHT;
	//ui.ui_data.dw = Al_Global::SCREEN_WIDTH;
	//ui.ui_data.sh = Al_Global::SCREEN_HEIGHT;
	//ui.ui_data.sw = Al_Global::SCREEN_WIDTH;

	//D3D11_VIEWPORT vp;
	//UINT num = 1;

	//Systems::DeviceContext->RSGetViewports(&num, &vp);

	//float screen_width = vp.Width;
	//float screen_height = vp.Height;

	//material->shader_activate();

	//// ���_�f�[�^�ݒ�
	//VertexFormat data[4];

	//data[0].position.x = ui_data.dx;
	//data[0].position.y = ui_data.dy;
	//data[0].position.z = 0.0f;

	//data[1].position.x = ui_data.dx + ui_data.dw;
	//data[1].position.y = ui_data.dy;
	//data[1].position.z = 0.0f;

	//data[2].position.x = ui_data.dx;
	//data[2].position.y = ui_data.dy + ui_data.dh;
	//data[2].position.z = 0.0f;

	//data[3].position.x = ui_data.dx + ui_data.dw;
	//data[3].position.y = ui_data.dy + ui_data.dh;
	//data[3].position.z = 0.0f;

	//// ��]�̒��S
	//float workPosX = ui_data.dx + ui_data.dw * 0.5f;
	//float workPosY = ui_data.dy + ui_data.dh * 0.5f;

	//// ��]����
	//for (int i = 0; i < 4; i++) {

	//	float workX = data[i].position.x - workPosX;
	//	float workY = data[i].position.y - workPosY;
	//	data[i].position.x = workX * cosf(ui_data.angle) - workY * sinf(ui_data.angle) + workPosX;
	//	data[i].position.y = workX * sinf(ui_data.angle) + workY * cosf(ui_data.angle) + workPosY;
	//	data[i].position.z = 0.0f;
	//}

	//// ���K���f�o�C�X���W�n
	//for (int i = 0; i < 4; i++) {
	//	data[i].position.x = 2.0f * data[i].position.x / screen_width - 1.0f;
	//	data[i].position.y = 1.0f - 2.0f * data[i].position.y / screen_height;
	//	data[i].position.z = 0.0f;
	//}


	//// �e�N�X�`�����W�ݒ�
	//data[0].texcoord.x = ui_data.sx;
	//data[0].texcoord.y = ui_data.sy;
	//data[1].texcoord.x = ui_data.sx + ui_data.sw;
	//data[1].texcoord.y = ui_data.sy;
	//data[2].texcoord.x = ui_data.sx;
	//data[2].texcoord.y = ui_data.sy + ui_data.sh;
	//data[3].texcoord.x = ui_data.sx + ui_data.sw;
	//data[3].texcoord.y = ui_data.sy + ui_data.sh;

	////UV���W
	//if (material->get_texture())
	//{
	//	for (int i = 0; i < 4; i++) {
	//		data[i].texcoord.x = data[i].texcoord.x / material->get_texture()->GetWidth();
	//		data[i].texcoord.y = data[i].texcoord.y / material->get_texture()->GetHeight();
	//	}
	//}
	//// �@��
	//data[0].normal = Vector3(0, 0, 1);
	//data[1].normal = Vector3(0, 0, 1);
	//data[2].normal = Vector3(0, 0, 1);
	//data[3].normal = Vector3(0, 0, 1);

	//// ���_�f�[�^�X�V
	//Systems::DeviceContext->UpdateSubresource(VertexBuffer.Get(), 0, NULL, data, 0, 0);

	////	���_�o�b�t�@�̎w��
	//UINT strides[2] = { sizeof(VertexFormat), sizeof(Instance) };
	//UINT offsets[2] = { 0, 0 };
	//ID3D11Buffer* vbs[2] = { VertexBuffer.Get(), instance_buffer.Get() };
	//Systems::DeviceContext->IASetVertexBuffers(0, 2, vbs, strides, offsets);
	//Systems::DeviceContext->IASetIndexBuffer(IndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	//Systems::DeviceContext->IASetPrimitiveTopology(
	//	D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP
	//);

	////�e�N�X�`���̐ݒ�
	//if (material->get_texture()) material->get_texture()->Set(0);

	//Systems::DeviceContext->Draw(4, 0);

}

void UI_renderer::render_instancing(Microsoft::WRL::ComPtr<ID3D11Buffer>& instance_buffer, int bufferStart, int bufferCount) {
	if (material == nullptr) return;

	//UI ui;
	//ui.ui_data.dh = Al_Global::SCREEN_HEIGHT;
	//ui.ui_data.dw = Al_Global::SCREEN_WIDTH;
	//ui.ui_data.sh = Al_Global::SCREEN_HEIGHT;
	//ui.ui_data.sw = Al_Global::SCREEN_WIDTH;

	D3D11_VIEWPORT vp;
	UINT num = 1;

	Systems::DeviceContext->RSGetViewports(&num, &vp);

	float screen_width = vp.Width;
	float screen_height = vp.Height;

	material->shader_activate();

	//
	material->BS_state = State_manager::BStypes::BS_ALPHA;
	material->RS_state = State_manager::RStypes::RS_CULL_NONE;
	material->DS_state = State_manager::DStypes::DS_FALSE;
	Systems::SetBlendState(material->BS_state);
	Systems::SetRasterizerState(material->RS_state);
	Systems::SetDephtStencilState(material->DS_state);


	// ���_�f�[�^�ݒ�
	VertexFormat data[4];

	// �����̑傫�� �Ƃ肠�����c��100�s�N�Z��
	float x_1 = 100 / screen_width;
	float h_1 = 100 / screen_height;

	data[0].position.x = -x_1 * transform->scale.x;
	data[0].position.y = -h_1 * transform->scale.y;
	data[0].position.z = 0.0f;

	data[1].position.x = +x_1 * transform->scale.x;
	data[1].position.y = -h_1 * transform->scale.y;
	data[1].position.z = 0.0f;

	data[2].position.x = -x_1 * transform->scale.x;
	data[2].position.y = +h_1 * transform->scale.y;
	data[2].position.z = 0.0f;

	data[3].position.x = +x_1 * transform->scale.x;
	data[3].position.y = +h_1 * transform->scale.y;
	data[3].position.z = 0.0f;

	// ��]�̒��S
	//float workPosX = ui_data.dx + ui_data.dw * 0.5f;
	//float workPosY = ui_data.dy + ui_data.dh * 0.5f;

	// ��]����
	for (int i = 0; i < 4; i++) {

		float workX = data[i].position.x;
		float workY = data[i].position.y;
		data[i].position.x = workX * cosf(ui_data.angle) - workY * sinf(ui_data.angle);
		data[i].position.y = workX * sinf(ui_data.angle) + workY * cosf(ui_data.angle);
		data[i].position.z = 0.0f;
	}
	// ���W�ړ�
	for (int i = 0; i < 4; i++) {
		data[i].position += transform->position * 0.01f;
	}

	// ���K���f�o�C�X���W�n
	//for (int i = 0; i < 4; i++) {
	//	data[i].position.x = 2.0f * data[i].position.x / screen_width - 1.0f;
	//	data[i].position.y = 1.0f - 2.0f * data[i].position.y / screen_height;
	//	data[i].position.z = 0.0f;
	//}


	// �e�N�X�`�����W�ݒ�
	data[0].texcoord.x = ui_data.sx;
	data[0].texcoord.y = ui_data.sy + ui_data.sh;
	data[1].texcoord.x = ui_data.sx + ui_data.sw;
	data[1].texcoord.y = ui_data.sy + ui_data.sh;
	data[2].texcoord.x = ui_data.sx;
	data[2].texcoord.y = ui_data.sy;
	data[3].texcoord.x = ui_data.sx + ui_data.sw;
	data[3].texcoord.y = ui_data.sy;

	//UV���W
	//if (material->get_texture())
	//{
	//	for (int i = 0; i < 4; i++) {
	//		data[i].texcoord.x = data[i].texcoord.x / material->get_texture()->GetWidth();
	//		data[i].texcoord.y = data[i].texcoord.y / material->get_texture()->GetHeight();
	//	}
	//}
	// �@��
	data[0].normal = Vector3(0, 0, 1);
	data[1].normal = Vector3(0, 0, 1);
	data[2].normal = Vector3(0, 0, 1);
	data[3].normal = Vector3(0, 0, 1);

	// ���_�f�[�^�X�V
	Systems::DeviceContext->UpdateSubresource(VertexBuffer.Get(), 0, NULL, data, 0, 0);

	//	���_�o�b�t�@�̎w��
	UINT strides[2] = { sizeof(VertexFormat), sizeof(Instance) };
	UINT offsets[2] = { 0, 0 };
	ID3D11Buffer* vbs[2] = { VertexBuffer.Get(), instance_buffer.Get() };
	Systems::DeviceContext->IASetVertexBuffers(0, 2, vbs, strides, offsets);
	Systems::DeviceContext->IASetIndexBuffer(IndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	Systems::DeviceContext->IASetPrimitiveTopology(
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP
	);

	//�e�N�X�`���̐ݒ�
	if (material->get_texture()) material->get_texture()->Set(0);

	Systems::DeviceContext->Draw(4, 0);

	if (material->get_texture()) material->get_texture()->Set(0, false);

}

void UI_renderer::load_texture(const wchar_t* filename) { material->get_texture()->Load(filename); };