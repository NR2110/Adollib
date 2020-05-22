#pragma once

#include <DirectXMath.h>
#include "light_types.h"

namespace Adollib {

	struct ConstantBufferForPerFrame
	{
		DirectX::XMFLOAT4	LightColor;		//���C�g�̐F
		DirectX::XMFLOAT4	LightDir;		//���C�g�̕���
		DirectX::XMFLOAT4	AmbientColor;	//����
		DirectX::XMFLOAT4	EyePos;			//�J����
		float	param1;			//���}�b�s���O�u�����h��
		float	param2;
		float	param3;
		float	param4;

		POINTLIGHT  PointLight[POINTMAX];//�|�C���g���C�g
		SPOTLIGHT  SpotLight[SPOTMAX];	//�X�|�b�g���C�g


	};


}