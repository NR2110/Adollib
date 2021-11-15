#include "head.hlsli"

#define BUFFER_SIZE 256
cbuffer CBPerFrame2 : register(b6)
{
    float4 Weight[BUFFER_SIZE];
    float KarnelSize;
    float2 texcel;
    float dummy_CBPerFrame2;
};


struct PSInput
{
    float4 Position : SV_POSITION;
    float2 Tex : TEXCOORD;
    float4 Color : COLOR;
};


struct PSOutput
{
    float4 Color : SV_TARGET0;
};