#include "func.hlsli"
#include "light_type.hlsli"
//--------------------------------------------
//	�e�N�X�`��
//--------------------------------------------
Texture2D DiffuseTexture : register(t0);
SamplerState DecalSampler : register(s0);


//--------------------------------------------
//	�O���[�o���ϐ�
//--------------------------------------------
cbuffer CBPerMesh : register(b0)
{
	matrix  World;
	matrix	matWVP;
};



cbuffer CBPerFrame : register(b2)
{
	float4  LightColor;		//���C�g�̐F
	float4	LightDir;		//���C�g�̕���
	float4  AmbientColor;	//����
	float4	EyePos;			//�J�����ʒu
	float	param1;
	float	param2;
	float	param3;
	float	param4;

	POINTLIGHT  PointLight[POINTMAX];	//�|�C���g���C�g�z��
	SPOTLIGHT  SpotLight[SPOTMAX];		//�X�|�b�g���C�g�z��

};