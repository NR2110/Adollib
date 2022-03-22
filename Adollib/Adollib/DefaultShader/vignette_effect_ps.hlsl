#include "vignette_effect.hlsli"


float4 main(PSInput input) : SV_TARGET0
{
    float4 color = (float4) 0;
    color = DiffuseTexture.Sample(DecalSampler, input.Tex) * input.Color;

    color *= (1 - smoothstep(0.4f, 1.3f, length(input.Tex - float2(0.5f, 0.5f))));

    return color;
}