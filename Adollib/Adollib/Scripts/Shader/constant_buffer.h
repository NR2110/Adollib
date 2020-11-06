#pragma once

#include <DirectXMath.h>
#include "../Shader/light_types.h"

namespace Adollib {
	namespace ConstantBuffer {
		//light
		struct ConstantBufferPerLight {
			DirectX::XMFLOAT4	LightColor;		//ライトの色
			DirectX::XMFLOAT4	LightDir;		//ライトの方向
			DirectX::XMFLOAT4	AmbientColor;	//環境光

			POINTLIGHT  PointLight[POINTMAX];//ポイントライト
			SPOTLIGHT  SpotLight[SPOTMAX];	//スポットライト
		};
		//camera
		struct ConstantBufferPerCamera {
			DirectX::XMFLOAT4X4 View;
			DirectX::XMFLOAT4 Eyepos;
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

			DirectX::XMFLOAT4 materialColor;
			DirectX::XMFLOAT4 ambientColor;

			float shininess;  // スペキュラ係数

			//float intensity;
		};
	}
}
