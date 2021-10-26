#include "component_camera.h"

#include "../Imgui/imgui_all.h"
#include "../Main/Adollib.h"
#include "../Main/systems.h"
#include "../Renderer/Shader/constant_buffer.h"
#include "../Renderer/texture.h"
#include "../Renderer/sprite.h"
#include "../Renderer/posteffect_base.h"
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
	color_texture = std::make_shared<Texture>();
	color_texture->Create(Al_Global::SCREEN_WIDTH, Al_Global::SCREEN_HEIGHT, DXGI_FORMAT_R8G8B8A8_UNORM);

	normal_texture = std::make_shared<Texture>();
	normal_texture->Create(Al_Global::SCREEN_WIDTH, Al_Global::SCREEN_HEIGHT, DXGI_FORMAT_R16G16B16A16_FLOAT);

	depth_texture = std::make_shared<Texture>();
	depth_texture->Create(Al_Global::SCREEN_WIDTH, Al_Global::SCREEN_HEIGHT, DXGI_FORMAT_R16G16B16A16_FLOAT);

	render_scene = gameobject->get_scene();
}

void Camera_component::finalize() {
	// gameobject_managerの配列から削除
	Gameobject_manager::remove_camera_component(gameobject->get_scene(), this_itr);

	//delete color_texture;
	//delete normal_texture;
	//delete depth_texture;
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
		//Systems::GetRenderTargetView(),
		color_texture->GetRenderTargetView(),
		normal_texture->GetRenderTargetView(),
		depth_texture->GetRenderTargetView()
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

void Camera_component::posteffect_render() {

	// 各posteffectのを処理していく
	for (auto& posteffect : posteffects) {
		posteffect->render(color_texture.get(), normal_texture.get(), depth_texture.get());
	}

	// mainのRTVに描画しないならここでreturn
	if (is_draw_main_RenderTargetView == false)return;
	ID3D11RenderTargetView* rtv = Systems::GetRenderTargetView();
	ID3D11DepthStencilView* dsv = Systems::GetDepthStencilView();
	Systems::DeviceContext->OMSetRenderTargets(1, &rtv, dsv);

	Sprite sprite;
	sprite.sprite_data.dh = Al_Global::SCREEN_HEIGHT;
	sprite.sprite_data.dw = Al_Global::SCREEN_WIDTH;
	sprite.sprite_data.sh = Al_Global::SCREEN_HEIGHT;
	sprite.sprite_data.sw = Al_Global::SCREEN_WIDTH;

	sprite.set_texture(color_texture);
	sprite.shader.Load_VS("./DefaultShader/sprite_vs.cso");
	sprite.shader.Load_PS("./DefaultShader/sprite_ps.cso");
	sprite.render();
}


void Camera_component::clear() {

	Systems::DeviceContext->ClearRenderTargetView(color_texture->GetRenderTargetView(), &clear_color[0]);
	Systems::DeviceContext->ClearRenderTargetView(normal_texture->GetRenderTargetView(), &clear_color[0]);
	Systems::DeviceContext->ClearRenderTargetView(depth_texture->GetRenderTargetView(), &clear_color[0]);
	Systems::DeviceContext->ClearDepthStencilView(Systems::GetDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void Camera_component::posteffect_initialize(Posteffect_base* posteffect) {

	posteffects.emplace_back(posteffect);
	std::list<Posteffect_base*>::iterator itr = posteffects.end();
	itr--;
	posteffect->this_itr = itr;
	posteffect->camera = this;
	posteffect->awake();
}