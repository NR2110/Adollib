
#include "sprite.h"

#include "texture.h"
#include "vertex_format.h"
#include "Shader/constant_buffer.h"
#include "../Main/systems.h"

using namespace Adollib;
using namespace ConstantBuffer;

Sprite::Sprite() {

	VertexFormat v[4];
	v[0].position = Vector3(-0.5f, +0.5f, 0);
	v[1].position = Vector3(+0.5f, +0.5f, 0);
	v[2].position = Vector3(-0.5f, -0.5f, 0);
	v[3].position = Vector3(+0.5f, -0.5f, 0);

	v[0].normal = Vector3(0, 0, 1);
	v[1].normal = Vector3(0, 0, 1);
	v[2].normal = Vector3(0, 0, 1);
	v[3].normal = Vector3(0, 0, 1);

	v[0].texcoord = Vector2(0, 0);
	v[1].texcoord = Vector2(1, 0);
	v[2].texcoord = Vector2(0, 1);
	v[3].texcoord = Vector2(1, 1);

	//	���_�o�b�t�@�쐬
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(VertexFormat) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA res;
	ZeroMemory(&res, sizeof(res));
	res.pSysMem = v;

	Systems::Device->CreateBuffer(&bd, &res, VertexBuffer.GetAddressOf());


	//�f�v�X�X�e���V���X�e�[�g
	D3D11_DEPTH_STENCIL_DESC depth_stencil_desc;
	ZeroMemory(&depth_stencil_desc, sizeof(depth_stencil_desc));
	depth_stencil_desc.DepthEnable = FALSE;
	Systems::Device->CreateDepthStencilState(&depth_stencil_desc, DepthStencilState.GetAddressOf());

	//shader.Load_VS("./DefaultShader/sprite_vs.cso");
	//shader.Load_PS("./DefaultShader/sprite_ps.cso");
	shader.Load_VS("./DefaultShader/sprite_vs.cso");
	shader.Load_PS("./DefaultShader/sprite_ps.cso");
}

void Sprite::set_texture(Texture* tex) {
	texture = tex;
}

void Sprite::render() {
	D3D11_VIEWPORT vp;
	UINT num = 1;

	Systems::DeviceContext->RSGetViewports(&num, &vp);

	float screen_width = vp.Width;
	float screen_height = vp.Height;

	shader.Activate();
	//���_�f�[�^�ݒ�
	VertexFormat data[4];

	data[0].position.x = sprite_data.dx;
	data[0].position.y = sprite_data.dy;
	data[0].position.z = 0.0f;

	data[1].position.x = sprite_data.dx + sprite_data.dw;
	data[1].position.y = sprite_data.dy;
	data[1].position.z = 0.0f;

	data[2].position.x = sprite_data.dx;
	data[2].position.y = sprite_data.dy + sprite_data.dh;
	data[2].position.z = 0.0f;

	data[3].position.x = sprite_data.dx + sprite_data.dw;
	data[3].position.y = sprite_data.dy + sprite_data.dh;
	data[3].position.z = 0.0f;

	//��]�̒��S
	float workPosX = sprite_data.dx + sprite_data.dw * 0.5f;
	float workPosY = sprite_data.dy + sprite_data.dh * 0.5f;

	//��]����
	for (int i = 0; i < 4; i++) {

		float workX = data[i].position.x - workPosX;
		float workY = data[i].position.y - workPosY;
		data[i].position.x = workX * cosf(sprite_data.angle) - workY * sinf(sprite_data.angle) + workPosX;
		data[i].position.y = workX * sinf(sprite_data.angle) + workY * cosf(sprite_data.angle) + workPosY;
		data[i].position.z = 0.0f;
	}

	// ���K���f�o�C�X���W�n
	for (int i = 0; i < 4; i++) {
		data[i].position.x = 2.0f * data[i].position.x / screen_width - 1.0f;
		data[i].position.y = 1.0f - 2.0f * data[i].position.y / screen_height;
		data[i].position.z = 0.0f;
	}


	//�e�N�X�`�����W�ݒ�
	data[0].texcoord.x = sprite_data.sx;
	data[0].texcoord.y = sprite_data.sy;
	data[1].texcoord.x = sprite_data.sx + sprite_data.sw;
	data[1].texcoord.y = sprite_data.sy;
	data[2].texcoord.x = sprite_data.sx;
	data[2].texcoord.y = sprite_data.sy + sprite_data.sh;
	data[3].texcoord.x = sprite_data.sx + sprite_data.sw;
	data[3].texcoord.y = sprite_data.sy + sprite_data.sh;

	//UV���W
	if (texture)
	{
		for (int i = 0; i < 4; i++) {
			data[i].texcoord.x = data[i].texcoord.x / texture->GetWidth();
			data[i].texcoord.y = data[i].texcoord.y / texture->GetHeight();
		}
	}
	//�@��
	data[0].normal = Vector3(0, 0, 1);
	data[1].normal = Vector3(0, 0, 1);
	data[2].normal = Vector3(0, 0, 1);
	data[3].normal = Vector3(0, 0, 1);

	//���_�f�[�^�X�V
	Systems::DeviceContext->UpdateSubresource(VertexBuffer.Get(), 0, NULL, data, 0, 0);

	//	���_�o�b�t�@�̎w��
	UINT stride = sizeof(VertexFormat);
	UINT offset = 0;
	Systems::DeviceContext->IASetVertexBuffers(
		0, 1, VertexBuffer.GetAddressOf(), // �X���b�g, ��, �o�b�t�@
		&stride,		// �P���_�̃T�C�Y
		&offset			// �J�n�ʒu
	);
	Systems::DeviceContext->IASetPrimitiveTopology(
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP
	);

	//�e�N�X�`���̐ݒ�
	if (texture) texture->Set(0);

	Systems::DeviceContext->Draw(4, 0);

}