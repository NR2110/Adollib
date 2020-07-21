#include "gameobject_light.h"
#include <d3d11.h>

namespace Adollib {

	void Light::initialize() {

		//ZeroMemory(PointLight, sizeof(POINTLIGHT) * POINTMAX);
		//ZeroMemory(SpotLight, sizeof(SPOTLIGHT) * SPOTMAX);

		std::list <std::shared_ptr<Component_light>>::iterator itr = components.begin();
		std::list <std::shared_ptr<Component_light>>::iterator itr_end = components.end();

		for (; itr != itr_end; itr++) {
			itr->get()->start();
		}

	}

	void Light::update() {

		std::list <std::shared_ptr<Component_light>>::iterator itr = components.begin();
		std::list <std::shared_ptr<Component_light>>::iterator itr_end = components.end();

		for (; itr != itr_end; itr++) {
			itr->get()->update();
		}

	}



	void Light::set_dirLight(vector3 dir, vector3 color)
	{
		float d = sqrtf(dir.x * dir.x + dir.y * dir.y + dir.z * dir.z);

		if (d > 0) { dir.x /= d, dir.y /= d, dir.z /= d; }
		LightDir = vector4(dir.x, dir.y, dir.z, 0);
		DirLightColor = vector4(color.x, color.y, color.z, 1);

	}
	void Light::set_ambient(vector3 amb)
	{
		Ambient = { amb.x,amb.y,amb.z,0 };

	}
	void Light::set_pointLight(vector3 pos, vector3 color, float range)
	{
		std::shared_ptr<POINTLIGHT> pointlight;
		pointlight->range = range;
		pointlight->type = 1.0f;
		pointlight->dumy = 0.0f;
		pointlight->pos = DirectX::XMFLOAT4(pos.x, pos.y, pos.z, 0);
		pointlight->color = DirectX::XMFLOAT4(color.x, color.y, color.z, 0);

		PointLight.push_back(pointlight);

	}
	void Light::set_spotLight(vector3 pos, vector3 color, vector3 dir,
		float range, float near_area, float far_area)
	{
		std::shared_ptr<SPOTLIGHT> spotlight;
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


}