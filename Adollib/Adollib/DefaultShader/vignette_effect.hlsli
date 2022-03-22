#include "head.hlsli"

struct PSInput
{
    float4 Position : SV_POSITION;
    float2 Tex : TEXCOORD;
    float4 Color : COLOR;
};