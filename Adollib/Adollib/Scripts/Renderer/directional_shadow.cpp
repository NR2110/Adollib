#include "directional_shadow.h"

#include "../Imgui/imgui_all.h"
#include "../Main/Adollib.h"
#include "../Main/systems.h"
#include "Shader/shader.h"
#include "Shader/constant_buffer.h"
#include "texture.h"

#include "../Imgui/debug.h"

using namespace Adollib;
using namespace ConstantBuffer;

void Directional_shadow::awake() {

	// コンスタントバッファーの生成
	Systems::CreateConstantBuffer(&view_cb, sizeof(ConstantBufferPerCamera));
	Systems::CreateConstantBuffer(&projection_cb, sizeof(ConstantBufferPerSystem));
	Systems::CreateConstantBuffer(&shadow_viewprojection_cb, sizeof(ConstantBufferPerShadow));

	// shaodow用texの準備
	shadow_texture = std::make_shared<Texture>();
	shadow_texture->CreateDepth(10000, 10000, DXGI_FORMAT_R24G8_TYPELESS);

	gaussianshadow_texture = std::make_shared<Texture>();
	gaussianshadow_texture->CreateDepth(10000, 10000, DXGI_FORMAT_R24G8_TYPELESS);

	shader = std::make_shared<Shader>();
	shader->Load_VS("./DefaultShader/render_directional_shadow_vs.cso");
}
void Directional_shadow::shader_activate() { shader->Activate(); };

void Directional_shadow::set_ShaderResourceView() {

	Systems::DeviceContext->OMSetRenderTargets(0, nullptr, nullptr);

	shadow_texture->Set(1);

}

void Directional_shadow::setup() {

	ConstantBufferPerCamera c_cb;
	ConstantBufferPerSystem s_sb;
	ConstantBufferPerShadow shadow_sb;

	Systems::DeviceContext->ClearDepthStencilView(shadow_texture->GetDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	ID3D11DepthStencilView* dsv = shadow_texture->GetDepthStencilView();
	Systems::DeviceContext->OMSetRenderTargets(0, nullptr, dsv);

	//CB : ConstantBufferPerCamera
	// ビュー行列
	Vector3 pos = position;
	Vector3 look_pos = position + direction;

	DirectX::XMVECTOR eye = DirectX::XMVectorSet(pos.x, pos.y, pos.z, 1.0f);
	DirectX::XMVECTOR focus = DirectX::XMVectorSet(look_pos.x, look_pos.y, look_pos.z, 1.0f);
	DirectX::XMVECTOR up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f);
	XMStoreFloat4x4(&c_cb.View, DirectX::XMMatrixLookAtLH(eye, focus, up));
	c_cb.Eyepos = DirectX::XMFLOAT4(pos.x, pos.y, pos.z, 1.0f);
	Systems::DeviceContext->UpdateSubresource(view_cb.Get(), 0, NULL, &c_cb, 0, 0);
	Systems::DeviceContext->VSSetConstantBuffers(1, 1, view_cb.GetAddressOf());
	Systems::DeviceContext->PSSetConstantBuffers(1, 1, view_cb.GetAddressOf());

	//CB : ConstantBufferPerSystem
	//DirectX::XMStoreFloat4x4(&s_sb.Projection, DirectX::XMMatrixPerspectiveFovLH(ToRadian(fov), aspect, nearZ, farZ));
	DirectX::XMStoreFloat4x4(&s_sb.Projection, DirectX::XMMatrixOrthographicLH(500, 500, nearZ, farZ));
	Systems::DeviceContext->UpdateSubresource(projection_cb.Get(), 0, NULL, &s_sb, 0, 0);
	Systems::DeviceContext->VSSetConstantBuffers(2, 1, projection_cb.GetAddressOf());
	Systems::DeviceContext->PSSetConstantBuffers(2, 1, projection_cb.GetAddressOf());


	//CB : ConstantBufferPerShadow
	shadow_sb.shadow_viewprojection = Matrix44(s_sb.Projection) * Matrix44(c_cb.View);
	shadow_sb.right_dir = direction;
	Systems::DeviceContext->UpdateSubresource(shadow_viewprojection_cb.Get(), 0, NULL, &shadow_sb, 0, 0);
	Systems::DeviceContext->VSSetConstantBuffers(7, 1, shadow_viewprojection_cb.GetAddressOf());
	Systems::DeviceContext->PSSetConstantBuffers(7, 1, shadow_viewprojection_cb.GetAddressOf());
}

void Directional_shadow::gaussian_filter() {
	Systems::DeviceContext->OMSetRenderTargets(0, nullptr, nullptr);
	shadow_texture->Set(1);


}

Frustum_data Directional_shadow::calculate_frustum_data() {

	return Frustum_data::create_frustum_data(
		position,
		direction,
		nearZ,
		farZ,
		fov,
		aspect
	);
}