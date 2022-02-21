#include "head.hlsli"

cbuffer CBPerSprite : register(b6)
{
    float3 Pos; //�ʒu
    float3 Normal; //�@��
    float2 Tex; //UV���W
    float4 Color; //���_�F

};


struct PSInput
{
    float4 Position : SV_POSITION;
    float2 Tex : TEXCOORD;
    float4 Color : COLOR;
    float3 wPos : TEXCOORD1;

	//���[���h�@��
    float3 wNormal : TEXCOORD3;

};

struct PSOutput
{
    float4 Color : SV_TARGET0;
    float4 Normal : SV_TARGET1;
    float4 SVPosiiton : SV_TARGET2;

};