#include "ui.hlsli"

//--------------------------------------------
//
//--------------------------------------------

float4 main(PSInput input) : SV_TARGET0
{
    float4 color = (float4) 0;
    color = DiffuseTexture.Sample(DecalSampler, input.Tex) * input.Color;

    return color;
}