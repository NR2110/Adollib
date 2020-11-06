#pragma once

#include <DirectXMath.h>
#include "../Shader/light_types.h"

namespace Adollib {
	namespace ConstantBuffer {
		//light
		struct ConstantBufferPerLight {
			DirectX::XMFLOAT4	LightColor;		//���C�g�̐F
			DirectX::XMFLOAT4	LightDir;		//���C�g�̕���
			DirectX::XMFLOAT4	AmbientColor;	//����

			POINTLIGHT  PointLight[POINTMAX];//�|�C���g���C�g
			SPOTLIGHT  SpotLight[SPOTMAX];	//�X�|�b�g���C�g
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

			float shininess;  // �X�y�L�����W��

			//float intensity;
		};
	}
}
