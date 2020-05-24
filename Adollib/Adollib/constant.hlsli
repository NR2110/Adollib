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
cbuffer CBPerMesh : register(b0)
{
	matrix  World;
	matrix	matWVP;
};



cbuffer CBPerFrame : register(b2)
{
	float4  LightColor;		//ライトの色
	float4	LightDir;		//ライトの方向
	float4  AmbientColor;	//環境光
	float4	EyePos;			//カメラ位置
	float	param1;
	float	param2;
	float	param3;
	float	param4;

	POINTLIGHT  PointLight[POINTMAX];	//ポイントライト配列
	SPOTLIGHT  SpotLight[SPOTMAX];		//スポットライト配列

};