#include "gameobject_light.h"
#include <d3d11.h>

using namespace DirectX;

namespace Adollib {

	void Light::initialize() {

		//ZeroMemory(PointLight, sizeof(POINTLIGHT) * POINTMAX);
		//ZeroMemory(SpotLight, sizeof(SPOTLIGHT) * SPOTMAX);

		std::list <std::shared_ptr<Component>>::iterator itr = components.begin();
		itr++;
		std::list <std::shared_ptr<Component>>::iterator itr_end = components.end();

		for (; itr != itr_end; itr++) {
			itr->get()->start();
		}

	}

	void Light::update() {

		std::list <std::shared_ptr<Component>>::iterator itr = components.begin();
		itr++;
		std::list <std::shared_ptr<Component>>::iterator itr_end = components.end();

		for (; itr != itr_end; itr++) {
			itr->get()->update();
		}

	}



	void Light::set_dirLight(XMFLOAT3 dir, XMFLOAT3 color)
	{
		float d = sqrtf(dir.x * dir.x + dir.y * dir.y + dir.z * dir.z);

		if (d > 0) { dir.x /= d, dir.y /= d, dir.z /= d; }
		LightDir = vector4(dir.x, dir.y, dir.z, 0);
		DirLightColor = vector4(color.x, color.y, color.z, 1);

	}
	void Light::set_ambient(XMFLOAT3 amb)
	{
		Ambient = { amb.x,amb.y,amb.z,0 };

	}
	void Light::set_pointLight(XMFLOAT3 pos, XMFLOAT3 color, float range)
	{
		std::shared_ptr<POINTLIGHT> pointlight;
		pointlight->range = range;
		pointlight->type = 1.0f;
		pointlight->dumy = 0.0f;
		pointlight->pos = XMFLOAT4(pos.x, pos.y, pos.z, 0);
		pointlight->color = XMFLOAT4(color.x, color.y, color.z, 0);

		PointLight.push_back(pointlight);

	}
	void Light::set_spotLight(XMFLOAT3 pos, XMFLOAT3 color, XMFLOAT3 dir,
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

		spotlight->pos = XMFLOAT4(pos.x, pos.y, pos.z, 0);
		spotlight->color = XMFLOAT4(color.x, color.y, color.z, 0);
		spotlight->dir = XMFLOAT4(dir.x, dir.y, dir.z, 0);

		SpotLight.push_back(spotlight);
	}


}