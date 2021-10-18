#pragma once
#include "../../Math/vector.h"
namespace Adollib {

	static const int POINTMAX = 32;
	static const int SPOTMAX = 32;

	static Vector4 LightDir = Vector4(0, 0, 0, 0);
	static Vector4 DirLightColor = Vector4(1, 1, 1, 1);
	static Vector4 Ambient = Vector4(1, 1, 1, 1);

	struct SPOTLIGHT {
		float index = 0;
		float range = 0;//Œõ‚Ì“Í‚­”ÍˆÍ
		float type = 0; //—LŒø‚©–³Œø‚©
		float near_area = 0;
		float far_area = 0;
		float dumy0 = 0;
		float dumy1 = 0;
		float dumy2 = 0;
		DirectX::XMFLOAT4 pos = DirectX::XMFLOAT4(0, 0, 0, 1);
		DirectX::XMFLOAT4 color = DirectX::XMFLOAT4(1, 1, 1, 1);
		DirectX::XMFLOAT4 dir = DirectX::XMFLOAT4(0, 0, 1, 1);
	};

	struct POINTLIGHT {
		float index = 0;
		float range = 0;//Œõ‚Ì“Í‚­”ÍˆÍ
		float type = 0; //—LŒø‚©–³Œø‚©
		float dumy = 0;
		DirectX::XMFLOAT4 pos = DirectX::XMFLOAT4(0, 0, 0, 1);;
		DirectX::XMFLOAT4 color = DirectX::XMFLOAT4(1, 1, 1, 1);;
	};


}