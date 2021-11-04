#include "head.hlsli"

struct PSInput
{
    float4 Position : SV_POSITION;

};

struct PSOutput
{
    float4 Position : SV_TARGET0;
};