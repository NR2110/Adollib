#include "component_light.h"

#include "../Renderer/Shader/light_types.h"

#include "../Imgui/imgui_all.h"
#include "../Main/Adollib.h"
#include "gameobject_manager.h"

using namespace Adollib;

void Light_component::awake() {
	this_itr = Gameobject_manager::add_light_component(gameobject->get_scene(), this);
}

void Light_component::finalize() {
	Gameobject_manager::remove_light_component(gameobject->get_scene(), this_itr);

	for (auto& p : PointLight) {
		delete p;
	}
	for (auto& p : SpotLight) {
		delete p;
	}
}


void Light_component::Update_hierarchy() {

	ImGui::DragFloat3("light_dir", &LightDir[0]);
}

void Light_component::set_dirLight(Vector3 dir, Vector3 color)
{
	float d = sqrtf(dir.x * dir.x + dir.y * dir.y + dir.z * dir.z);

	if (d > 0) { dir.x /= d, dir.y /= d, dir.z /= d; }
	LightDir = Vector4(dir.x, dir.y, dir.z, 0);
	DirLightColor = Vector4(color.x, color.y, color.z, 1);

}
void Light_component::set_ambient(Vector3 amb)
{
	Ambient = { amb.x,amb.y,amb.z,0 };

}
void Light_component::set_pointLight(Vector3 pos, Vector3 color, float range)
{
	POINTLIGHT* pointlight = newD POINTLIGHT;
	pointlight->range = range;
	pointlight->type = 1.0f;
	pointlight->dumy = 0.0f;
	pointlight->pos = DirectX::XMFLOAT4(pos.x, pos.y, pos.z, 0);
	pointlight->color = DirectX::XMFLOAT4(color.x, color.y, color.z, 0);

	PointLight.push_back(pointlight);

}
void Light_component::set_spotLight(Vector3 pos, Vector3 color, Vector3 dir,
	float range, float near_area, float far_area)
{
	SPOTLIGHT* spotlight = newD SPOTLIGHT;
	float d = sqrtf(dir.x * dir.x + dir.y * dir.y + dir.z * dir.z);
	if (d > 0) {
		dir.x /= d; dir.y /= d; dir.z /= d;
	}

	spotlight->range = range;
	spotlight->type = 1.0f;
	spotlight->near_area = near_area;
	spotlight->far_area = far_area;
	spotlight->dumy0 = 0.0f;
	spotlight->dumy1 = 0.0f;
	spotlight->dumy2 = 0.0f;

	spotlight->pos = DirectX::XMFLOAT4(pos.x, pos.y, pos.z, 0);
	spotlight->color = DirectX::XMFLOAT4(color.x, color.y, color.z, 0);
	spotlight->dir = DirectX::XMFLOAT4(dir.x, dir.y, dir.z, 0);

	SpotLight.push_back(spotlight);
}