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
cbuffer CBPerObj : register(b0)
{
	row_major float4x4  World;
};

cbuffer CBPerCamera : register(b1)
{
	row_major float4x4  View;
	float4	EyePos;
};

cbuffer CBPerSystem : register(b2)
{
	row_major float4x4  Projection;
};


cbuffer CBPerLight : register(b3)
{
	float4  LightColor;		//���C�g�̐F
	float4	LightDir;		//���C�g�̕���
	float4  AmbientColor;	//����

	POINTLIGHT  PointLight[POINTMAX];	//�|�C���g���C�g�z��
	SPOTLIGHT  SpotLight[SPOTMAX];		//�X�|�b�g���C�g�z��

};

#define MAX_BONES 4
cbuffer CBPerMaterial : register(b4)
{
	row_major float4x4 BoneTransforms[MAX_BONES];

	float4 materialColor;
	float4 ambientColor; // ����

	float  shininess;  // �X�y�L�����W��
};