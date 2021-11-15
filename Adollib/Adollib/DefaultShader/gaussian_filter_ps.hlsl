#include "gaussian_filter.hlsli"

PSOutput main(PSInput input)
{
    PSOutput output = (PSOutput) 0;

    float4 color = (float4) 0;
    color.a = input.Color.a;
    for (int i = 0; i < KarnelSize * KarnelSize; i++)
    {
        float2 offset = texcel * Weight[i].xy;
        float weight = Weight[i].z;
        color.rgb += DiffuseTexture.Sample(DecalSampler, input.Tex + offset).rgb * weight;
    }

    output.Color = color;
    return output;
}