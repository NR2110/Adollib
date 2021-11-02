#pragma once

#include <DirectXMath.h>
#include "../Shader/light_types.h"

namespace Adollib {
	namespace ConstantBuffer {
		// light (b4)
		struct ConstantBufferPerLight {
			DirectX::XMFLOAT4	LightColor = DirectX::XMFLOAT4(1,1,1,1);		//���C�g�̐F
			DirectX::XMFLOAT4	LightDir = DirectX::XMFLOAT4(1, 1, 1, 1);		//���C�g�̕���
			DirectX::XMFLOAT4	AmbientColor = DirectX::XMFLOAT4(1, 1, 1, 1);	//����

			POINTLIGHT  PointLight[POINTMAX];//�|�C���g���C�g
			SPOTLIGHT  SpotLight[SPOTMAX];	 //�X�|�b�g���C�g
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

			float shininess = 1;  //�X�y�L�����W��

			//float intensity;
		};
		// Sprite (b6)
		struct ConstantBufferPerSprite
		{
			DirectX::XMFLOAT3 Pos;	    //�ʒu
			DirectX::XMFLOAT3 Normal;   //�@��
			DirectX::XMFLOAT2 Tex;	    //UV���W
			DirectX::XMFLOAT4 Color;	//���_�F
		};
		// shadow (b7)
		struct ConstantBufferPerShadow
		{
			DirectX::XMFLOAT4X4 shadow_viewprojection;
		};
	}
}
