
#include "head.hlsli"

struct PSInput
{
    float4 Position : SV_POSITION;
    float4 Color : COLOR;

};


struct PSOutput
{
    float4 Color : SV_TARGET0;

};