#include "component_camera.h"

#include "../Imgui/imgui_all.h"
#include "../Main/Adollib.h"
#include "../Main/systems.h"
#include "../Renderer/Shader/constant_buffer.h"
#include "../Renderer/texture.h"
#include "gameobject_manager.h"

using namespace Adollib;
using namespace ConstantBuffer;

void Camera_component::awake() {

	// コンスタントバッファーの生成
	Systems::CreateConstantBuffer(&sprite_cb, sizeof(ConstantBufferPerSprite));
	Systems::CreateConstantBuffer(&view_cb, sizeof(ConstantBufferPerCamera));
	Systems::CreateConstantBuffer(&projection_cb, sizeof(ConstantBufferPerSystem));

	// gameobject_managerの配列に登録
	this_itr = Gameobject_manager::add_camera_component(gameobject->get_scene(), this);
	aspect = (float)Al_Global::SCREEN_WIDTH / Al_Global::SCREEN_HEIGHT;

	// MRT用textureの生成
	color_texture = new Texture;
	color_texture->Create(Al_Global::SCREEN_WIDTH, Al_Global::SCREEN_HEIGHT, DXGI_FORMAT_R8G8B8A8_UNORM);

	normal_texture = new Texture;
	normal_texture->Create(Al_Global::SCREEN_WIDTH, Al_Global::SCREEN_HEIGHT, DXGI_FORMAT_R16G16B16A16_FLOAT);

	depth_texture = new Texture;
	depth_texture->Create(Al_Global::SCREEN_WIDTH, Al_Global::SCREEN_HEIGHT, DXGI_FORMAT_R16G16B16A16_FLOAT);
}

void Camera_component::finalize() {
	// gameobject_managerの配列から削除
	Gameobject_manager::remove_camera_component(gameobject->get_scene(), this_itr);

	delete color_texture;
	delete normal_texture;
	delete depth_texture;
}


void Camera_component::Update_hierarchy() {

	ImGui::DragFloat("fov", &fov, 0.1f, 0, 0, "%.2f");
	ImGui::DragFloat("aspect", &aspect, 0.1f, 0, 0, "%.2f");
	ImGui::DragFloat("nearZ", &nearZ, 0.1f, 0, 0, "%.2f");
	ImGui::DragFloat("farZ", &farZ, 0.1f, 0, 0, "%.2f");
}

void Camera_component::set_Constantbuffer() {

	ConstantBufferPerCamera c_cb;
	ConstantBufferPerSystem s_sb;

	ID3D11RenderTargetView* rtv[3] = {
		Systems::GetRenderTargetView(),
		color_texture->GetRenderTargetView(),
		normal_texture->GetRenderTargetView(),
		//depth_texture->GetRenderTargetView()
	};
	ID3D11DepthStencilView* dsv = Systems::GetDepthStencilView();
	Systems::DeviceContext->OMSetRenderTargets(3, rtv, dsv);

	//CB : ConstantBufferPerCamera
	// ビュー行列
	Vector3 pos = transform->position;
	Quaternion orient = transform->orientation;
	Vector3 look_pos = pos + vector3_quatrotate(Vector3(0, 0, 1), orient);

	DirectX::XMVECTOR eye = DirectX::XMVectorSet(pos.x, pos.y, pos.z, 1.0f);
	DirectX::XMVECTOR focus = DirectX::XMVectorSet(look_pos.x, look_pos.y, look_pos.z, 1.0f);
	DirectX::XMVECTOR up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f);
	XMStoreFloat4x4(&c_cb.View, DirectX::XMMatrixLookAtLH(eye, focus, up));
	c_cb.Eyepos = DirectX::XMFLOAT4(pos.x, pos.y, pos.z, 1.0f);
	Systems::DeviceContext->UpdateSubresource(view_cb.Get(), 0, NULL, &c_cb, 0, 0);
	Systems::DeviceContext->VSSetConstantBuffers(1, 1, view_cb.GetAddressOf());
	Systems::DeviceContext->PSSetConstantBuffers(1, 1, view_cb.GetAddressOf());

	//CB : ConstantBufferPerSystem
	DirectX::XMStoreFloat4x4(&s_sb.Projection, DirectX::XMMatrixPerspectiveFovLH(ToRadian(fov), aspect, nearZ, farZ));
	Systems::DeviceContext->UpdateSubresource(projection_cb.Get(), 0, NULL, &s_sb, 0, 0);
	Systems::DeviceContext->VSSetConstantBuffers(2, 1, projection_cb.GetAddressOf());
	Systems::DeviceContext->PSSetConstantBuffers(2, 1, projection_cb.GetAddressOf());
}

void Camera_component::MRT_render() {

	clear();


	Shader shader;
	shader.Load_VS("./DefaultShader/sprite_vs.cso");
	shader.Load_PS("./DefaultShader/sprite_ps.cso");
	shader.Activate();
	//{
	//	Shader shader_;
	//	//shader.Load_PS("./DefaultShader/default_ps_notex.cso");
	//	shader_.Load_PS("./DefaultShader/default_ps_debug.cso");
	//}

	//D3D11_VIEWPORT vp;
	//UINT num = 1;

	//Systems::DeviceContext->RSGetViewports(&num, &vp);
	////float screen_width = vp.Width;
	////float screen_height = vp.Height;


	//float vw = Al_Global::SCREEN_WIDTH;
	//float vh = Al_Global::SCREEN_HEIGHT;
	//float screen_width = vw;
	//float screen_height = vh;

	//shader.Activate();
	////頂点データ設定
	//ConstantBufferPerSprite data[4];

	//data[0].Pos.x = dx;
	//data[0].Pos.y = dy;
	//data[0].Pos.z = 0.0f;

	//data[1].Pos.x = dx + dw;
	//data[1].Pos.y = dy;
	//data[1].Pos.z = 0.0f;

	//data[2].Pos.x = dx;
	//data[2].Pos.y = dy + dh;
	//data[2].Pos.z = 0.0f;

	//data[3].Pos.x = dx + dw;
	//data[3].Pos.y = dy + dh;
	//data[3].Pos.z = 0.0f;

	//// 正規化デバイス座標系

	//for (int i = 0; i < 4; i++) {
	//	data[i].Pos.x = 2.0f * data[i].Pos.x / screen_width - 1.0f;
	//	data[i].Pos.y = 1.0f - 2.0f * data[i].Pos.y / screen_height;
	//	data[i].Pos.z = 0.0f;
	//}

	////テクスチャ座標設定
	//data[0].Tex.x = sx;
	//data[0].Tex.y = sy;
	//data[1].Tex.x = sx + sw;
	//data[1].Tex.y = sy;
	//data[2].Tex.x = sx;
	//data[2].Tex.y = sy + sh;
	//data[3].Tex.x = sx + sw;
	//data[3].Tex.y = sy + sh;

	////UV座標
	//for (int i = 0; i < 4; i++) {
	//	data[i].Tex.x = data[i].Tex.x / tex->GetWidth();
	//	data[i].Tex.y = data[i].Tex.y / tex->GetHeight();
	//}
	////頂点カラー
	//data[0].Color = XMFLOAT4(1, 1, 1, alpha);
	//data[1].Color = XMFLOAT4(1, 1, 1, alpha);
	//data[2].Color = XMFLOAT4(1, 1, 1, alpha);
	//data[3].Color = XMFLOAT4(1, 1, 1, alpha);
	////法線
	//data[0].Normal = XMFLOAT3(0, 0, 1);
	//data[1].Normal = XMFLOAT3(0, 0, 1);
	//data[2].Normal = XMFLOAT3(0, 0, 1);
	//data[3].Normal = XMFLOAT3(0, 0, 1);

	////頂点データ更新
	//Systems::DeviceContext->UpdateSubresource(sprite_cb.Get(), 0, NULL, data, 0, 0);

	////	頂点バッファの指定
	//UINT stride = sizeof(ConstantBufferPerSprite);
	//UINT offset = 0;
	//Systems::DeviceContext->IASetVertexBuffers(
	//	0, 1, VertexBuffer.GetAddressOf(), // スロット, 数, バッファ
	//	&stride,		// １頂点のサイズ
	//	&offset			// 開始位置
	//);
	//Systems::DeviceContext->IASetPrimitiveTopology(
	//	D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP
	//);

	//if (Systems::BS_type != State_manager::BStypes::BS_NONE)      Systems::SetBlendState(State_manager::BStypes::BS_NONE);
	//if (Systems::RS_type != State_manager::RStypes::RS_CULL_BACK) Systems::SetRasterizerState(State_manager::RStypes::RS_CULL_BACK);
	//if (Systems::DS_type != State_manager::DStypes::DS_TRUE)      Systems::SetDephtStencilState(State_manager::DStypes::DS_TRUE);

	////テクスチャの設定
	//if (tex) tex->Set(0);

	//Systems::DeviceContext->Draw(4, 0);
	////シェーダ無効か
	//shader->Inactivate();
}


void Camera_component::clear() {

	float clearColor[4] = { 0,0,0,1 };

	Systems::DeviceContext->ClearRenderTargetView(color_texture->GetRenderTargetView(), clearColor);
	Systems::DeviceContext->ClearRenderTargetView(normal_texture->GetRenderTargetView(), clearColor);
	Systems::DeviceContext->ClearRenderTargetView(depth_texture->GetRenderTargetView(), clearColor);
	Systems::DeviceContext->ClearDepthStencilView(Systems::GetDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}