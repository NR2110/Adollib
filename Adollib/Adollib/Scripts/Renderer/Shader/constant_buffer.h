#pragma once

#include <DirectXMath.h>
#include "../Shader/light_types.h"

namespace Adollib {
	namespace ConstantBuffer {
		// light (b4)
		struct ConstantBufferPerLight {
			DirectX::XMFLOAT4	LightColor = DirectX::XMFLOAT4(1,1,1,1);		//ライトの色
			DirectX::XMFLOAT4	LightDir = DirectX::XMFLOAT4(1, 1, 1, 1);		//ライトの方向
			DirectX::XMFLOAT4	AmbientColor = DirectX::XMFLOAT4(1, 1, 1, 1);	//環境光

			POINTLIGHT  PointLight[POINTMAX];//ポイントライト
			SPOTLIGHT  SpotLight[SPOTMAX];	 //スポットライト
		};
		// camera (b1)
		struct ConstantBufferPerCamera {
			DirectX::XMFLOAT4X4 View;
			DirectX::XMFLOAT4 Eyepos = DirectX::XMFLOAT4(1, 1, 1, 1);
		};
		// system (b2)
		struct ConstantBufferPerSystem {
			DirectX::XMFLOAT4X4 Projection;
		};
		// gameobject (b0)
		struct ConstantBufferPerGO {
			DirectX::XMFLOAT4X4 world;
		};
		// Mesh (b3)
		struct ConstantBufferPerMesh {
			DirectX::XMFLOAT4X4 Mesh_world;
		};
		// material (b5)
		static constexpr int MAX_BONES = 4;
		struct ConstantBufferPerMaterial
		{
			DirectX::XMFLOAT4X4 boneTransforms[MAX_BONES];

			DirectX::XMFLOAT4 materialColor = DirectX::XMFLOAT4(1, 1, 1, 1);
			DirectX::XMFLOAT4 ambientColor = DirectX::XMFLOAT4(1, 1, 1, 1);

			float shininess = 1;  //スペキュラ係数

			//float intensity;
		};
		// Sprite (b6)
		struct ConstantBufferPerSprite
		{
			DirectX::XMFLOAT3 Pos;	    //位置
			DirectX::XMFLOAT3 Normal;   //法線
			DirectX::XMFLOAT2 Tex;	    //UV座標
			DirectX::XMFLOAT4 Color;	//頂点色
		};
		// shadow (b7)
		struct ConstantBufferPerShadow
		{
			DirectX::XMFLOAT4X4 shadow_viewprojection;
		};
	}
}
