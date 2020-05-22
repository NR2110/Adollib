#pragma once

#include <DirectXMath.h>
#include "light_types.h"

namespace Adollib {

	struct ConstantBufferForPerFrame
	{
		DirectX::XMFLOAT4	LightColor;		//ライトの色
		DirectX::XMFLOAT4	LightDir;		//ライトの方向
		DirectX::XMFLOAT4	AmbientColor;	//環境光
		DirectX::XMFLOAT4	EyePos;			//カメラ
		float	param1;			//環境マッピングブレンド率
		float	param2;
		float	param3;
		float	param4;

		POINTLIGHT  PointLight[POINTMAX];//ポイントライト
		SPOTLIGHT  SpotLight[SPOTMAX];	//スポットライト


	};


}