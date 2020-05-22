#include "gameobject_light.h"
#include <d3d11.h>

using namespace DirectX;

namespace Adollib {

	void Light::initialize() {

		ZeroMemory(PointLight, sizeof(POINTLIGHT) * POINTMAX);
		ZeroMemory(SpotLight, sizeof(SPOTLIGHT) * SPOTMAX);

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
	void Light::set_pointLight(int index, XMFLOAT3 pos, XMFLOAT3 color, float range)
	{
		if (index < 0) return;
		if (index >= POINTMAX)return;
		PointLight[index].index = (float)index;
		PointLight[index].range = range;
		PointLight[index].type = 1.0f;
		PointLight[index].dumy = 0.0f;
		PointLight[index].pos = XMFLOAT4(pos.x, pos.y, pos.z, 0);
		PointLight[index].color = XMFLOAT4(color.x, color.y, color.z, 0);

	}
	void Light::set_spotLight(int index, XMFLOAT3 pos, XMFLOAT3 color, XMFLOAT3 dir,
		float range, float near_area, float far_area)
	{
		if (index < 0) return;
		if (index >= SPOTMAX)return;
		float d = sqrtf(dir.x * dir.x + dir.y * dir.y + dir.z * dir.z);
		if (d > 0) {
			dir.x /= d; dir.y /= d; dir.z /= d;
		}

		SpotLight[index].index = (float)index;
		SpotLight[index].range = range;
		SpotLight[index].type = 1.0f;
		SpotLight[index].near_area = near_area;
		SpotLight[index].far_area = far_area;
		SpotLight[index].dumy0 = 0.0f;
		SpotLight[index].dumy1 = 0.0f;
		SpotLight[index].dumy2 = 0.0f;

		SpotLight[index].pos = XMFLOAT4(pos.x, pos.y, pos.z, 0);
		SpotLight[index].color = XMFLOAT4(color.x, color.y, color.z, 0);
		SpotLight[index].dir = XMFLOAT4(dir.x, dir.y, dir.z, 0);
	}


}