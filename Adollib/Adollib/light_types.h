#pragma once
#include "vector.h"
namespace Adollib {

	static const int POINTMAX = 32;
	static const int SPOTMAX = 32;

	static Vector4 LightDir = Vector4(0, 0, 0, 0);
	static Vector4 DirLightColor = Vector4(1, 1, 1, 1);
	static Vector4 Ambient = Vector4(1, 1, 1, 1);

	struct SPOTLIGHT {
		float index;
		float range;//Œõ‚Ì“Í‚­”ÍˆÍ
		float type; //—LŒø‚©–³Œø‚©
		float near_area;
		float far_area;
		float dumy0;
		float dumy1;
		float dumy2;
		DirectX::XMFLOAT4 pos;
		DirectX::XMFLOAT4 color;
		DirectX::XMFLOAT4 dir;
	};

	struct POINTLIGHT {
		float index;
		float range;//Œõ‚Ì“Í‚­”ÍˆÍ
		float type; //—LŒø‚©–³Œø‚©
		float dumy;
		DirectX::XMFLOAT4 pos;
		DirectX::XMFLOAT4 color;
	};


}