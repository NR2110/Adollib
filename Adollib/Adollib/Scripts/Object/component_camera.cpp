#include "component_camera.h"

#include "../Imgui/imgui_all.h"
#include "../Main/Adollib.h"
#include "../Main/systems.h"
#include "../Renderer/Shader/constant_buffer.h"
#include "gameobject_manager.h"

using namespace Adollib;
using namespace ConstantBuffer;

void Camera_component::awake() {

	Systems::CreateConstantBuffer(&view_cb, sizeof(ConstantBufferPerCamera));
	Systems::CreateConstantBuffer(&projection_cb, sizeof(ConstantBufferPerSystem));

	this_itr = Gameobject_manager::add_camera_component(gameobject->get_scene(), this);
	aspect = (float)Al_Global::SCREEN_WIDTH / Al_Global::SCREEN_HEIGHT;
}

void Camera_component::finalize() {
	Gameobject_manager::remove_camera_component(gameobject->get_scene(), this_itr);
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

	//CB : ConstantBufferPerCamera
	// ƒrƒ…[s—ñ
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