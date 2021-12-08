
#include "sprite_renderer.h"

#include "../Object/gameobject.h"
#include "renderer_manager.h"

#include "../Main/systems.h"
#include "Shader/constant_buffer.h"
#include "Shader/vertex_format.h"

#include "material.h"
#include "texture.h"

using namespace Adollib;
using namespace ConstantBuffer;

void Sprite_renderer::init() {

	//	頂点バッファ作成
	VertexFormat v[4];

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(VertexFormat) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA res;
	ZeroMemory(&res, sizeof(res));
	res.pSysMem = v;

	Systems::Device->CreateBuffer(&bd, &res, VertexBuffer.GetAddressOf());

	material->Load_VS("./DefaultShader/sprite_vs.cso");
	material->Load_PS("./DefaultShader/sprite_ps.cso");
}

void Sprite_renderer::render(const Frustum_data& frustum_data) {
	if (material == nullptr) return;

	// CB : ConstantBufferPerOBJ
	// GOのtransformの情報をConstantBufferへセットする
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
	// 頂点
	data[0].position = Vector3(-1, +1, 0);
	data[1].position = Vector3(-1, -1, 0);
	data[2].position = Vector3(+1, +1, 0);
	data[3].position = Vector3(+1, -1, 0);

	// uv座標
	data[0].texcoord = uv_pos[0];
	data[1].texcoord = uv_pos[1];
	data[2].texcoord = uv_pos[2];
	data[3].texcoord = uv_pos[3];

	// 法線
	data[0].normal = Vector3(0, 0, 1);
	data[1].normal = Vector3(0, 0, 1);
	data[2].normal = Vector3(0, 0, 1);
	data[3].normal = Vector3(0, 0, 1);

	// 頂点データ更新
	Systems::DeviceContext->UpdateSubresource(VertexBuffer.Get(), 0, NULL, data, 0, 0);

	// 頂点バッファの指定
	UINT stride = sizeof(VertexFormat);
	UINT offset = 0;
	Systems::DeviceContext->IASetVertexBuffers(0, 1, VertexBuffer.GetAddressOf(), &stride, &offset);

	Systems::DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	//テクスチャの設定
	if (material->get_texture()) material->get_texture()->Set(0);

	// 描画
	Systems::DeviceContext->Draw(4, 0);

}

void Sprite_renderer::load_texture(const wchar_t* filename) { material->get_texture()->Load(filename); };