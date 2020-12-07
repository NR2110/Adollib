#pragma once

#include <DirectXMath.h>
#include "../Shader/light_types.h"

namespace Adollib {
	namespace ConstantBuffer {
		//light
		struct ConstantBufferPerLight {
			DirectX::XMFLOAT4	LightColor = DirectX::XMFLOAT4(1,1,1,1);		//ライトの色
			DirectX::XMFLOAT4	LightDir = DirectX::XMFLOAT4(1, 1, 1, 1);		//ライトの方向
			DirectX::XMFLOAT4	AmbientColor = DirectX::XMFLOAT4(1, 1, 1, 1);	//環境光

			POINTLIGHT  PointLight[POINTMAX];//ポイントライト
			SPOTLIGHT  SpotLight[SPOTMAX];	//スポットライト
		};
		//camera
		struct ConstantBufferPerCamera {
			DirectX::XMFLOAT4X4 View;
			DirectX::XMFLOAT4 Eyepos = DirectX::XMFLOAT4(1, 1, 1, 1);
		};
		//system
		struct ConstantBufferPerSystem {
			DirectX::XMFLOAT4X4 Projection;
		};
		//gameobject
		struct ConstantBufferPerGO {
			DirectX::XMFLOAT4X4 world;
		};
		//material
		struct ConstantBufferPerMaterial
		{
			DirectX::XMFLOAT4X4 boneTransforms[MAX_BONES];

			DirectX::XMFLOAT4 materialColor = DirectX::XMFLOAT4(1, 1, 1, 1);
			DirectX::XMFLOAT4 ambientColor = DirectX::XMFLOAT4(1, 1, 1, 1);

			float shininess = 1;  // スペキュラ係数

			//float intensity;
		};
	}
}
