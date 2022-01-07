
#include "head.hlsli"

struct PSInput
{
    float4 Position : SV_POSITION;
    float4 Color : COLOR;
    float4 Worldpos : TEXCOORD1;
};


struct PSOutput
{
    float4 Color : SV_TARGET0;

};