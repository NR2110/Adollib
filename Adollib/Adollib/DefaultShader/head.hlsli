
#include "func.hlsli"
#include "light_type.hlsli"


//:::::::::::::::::::::::::::::::::::::::::::::
Texture2D DiffuseTexture : register(t0);
SamplerState DecalSampler : register(s0);



//:::::::::::::::::::::::::::::::::::::::::::::
#define MAX_BONE_INFLUENCES 32
struct VSInput
{
    float3 Position : POSITION;
    float3 Normal : NORMAL;
    float2 Tex : TEXCOORD;
    float4 BoneWeights : WEIGHTS;
    uint4 BoneIndices : BONES;
};



//:::::::::::::::::::::::::::::::::::::::::::::
cbuffer CBPerObj : register(b0)
{
	// world
	row_major float4x4  World;
};

cbuffer CBPerCamera : register(b1)
{
	// view
	row_major float4x4  View;
	float4	EyePos;
};

cbuffer CBPerSystem : register(b2)
{
	// projection
	row_major float4x4  Projection;
};

cbuffer CBPerMesh : register(b3)
{
	// mesh data
    row_major float4x4 Mesh_World;
};

cbuffer CBPerLight : register(b4)
{
	// light data
	float4  LightColor;		//ライトの色
	float4	LightDir;		//ライトの方向
	float4  AmbientColor;	//環境光

	POINTLIGHT  PointLight[POINTMAX];	//ポイントライト配列
	SPOTLIGHT  SpotLight[SPOTMAX];		//スポットライト配列

};

#define MAX_BONES 4
cbuffer CBPerMaterial : register(b5)
{
    row_major float4x4 BoneTransforms[MAX_BONES];

    float4 materialColor;
    float4 ambientColor; // 環境光

    float shininess; // スペキュラ係数
};

cbuffer CBPerSprite : register(b6)
{
    float3 Pos; //位置
    float3 Normal; //法線
    float2 Tex; //UV座標
    float4 Color; //頂点色

};