#pragma once

#include <DirectXMath.h>
#include "../Shader/light_types.h"

namespace Adollib {
	namespace ConstantBuffer {
		//light
		struct ConstantBufferPerLight {
			DirectX::XMFLOAT4	LightColor = DirectX::XMFLOAT4(1,1,1,1);		//���C�g�̐F
			DirectX::XMFLOAT4	LightDir = DirectX::XMFLOAT4(1, 1, 1, 1);		//���C�g�̕���
			DirectX::XMFLOAT4	AmbientColor = DirectX::XMFLOAT4(1, 1, 1, 1);	//����

			POINTLIGHT  PointLight[POINTMAX];//�|�C���g���C�g
			SPOTLIGHT  SpotLight[SPOTMAX];	//�X�|�b�g���C�g
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

			float shininess = 1;  // �X�y�L�����W��

			//float intensity;
		};
	}
}
