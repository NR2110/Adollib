#include "head.hlsli"

cbuffer CBPerSprite : register(b6)
{
    float3 Pos; //位置
    float3 Normal; //法線
    float2 Tex; //UV座標
    float4 Color; //頂点色

};


struct PSInput
{
    float4 Position : SV_POSITION;
    float2 Tex : TEXCOORD;
    float4 Color : COLOR;
    float3 wPos : TEXCOORD1;

	//ワールド法線
    float3 wNormal : TEXCOORD3;

};

struct PSOutput
{
    float4 Color : SV_TARGET0;
    float4 Normal : SV_TARGET1;
    float4 SVPosiiton : SV_TARGET2;

};