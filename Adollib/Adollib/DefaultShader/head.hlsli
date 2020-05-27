#include "func.hlsli"
#include "light_type.hlsli"
//--------------------------------------------
//	テクスチャ
//--------------------------------------------
Texture2D DiffuseTexture : register(t0);
SamplerState DecalSampler : register(s0);


//--------------------------------------------
//	グローバル変数
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
	float4  LightColor;		//ライトの色
	float4	LightDir;		//ライトの方向
	float4  AmbientColor;	//環境光

	POINTLIGHT  PointLight[POINTMAX];	//ポイントライト配列
	SPOTLIGHT  SpotLight[SPOTMAX];		//スポットライト配列

};

#define MAX_BONES 4
cbuffer CBPerMaterial : register(b4)
{
	row_major float4x4 BoneTransforms[MAX_BONES];

	float4 materialColor;
	float4 ambientColor; // 環境光

	float  shininess;  // スペキュラ係数
};