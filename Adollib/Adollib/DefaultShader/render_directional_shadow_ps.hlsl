#include "render_directional_shadow.hlsli"

//	�s�N�Z���V�F�[�_�[
PSOutput main(PSInput input)
{
    PSOutput output;
    //PSOutput Out = (PSOutput) 0;

    //float A = (dot(input.wNormal, normalize(float3(0, 1, 1))) + 1) / 4 + 0.5; //�e
    //float4 Color = input.Color * DiffuseTexture.Sample(DecalSampler, input.Tex) * A;
    //Color.w = input.Color.w;

    ////Color = input.Color;

    //Out.Color = Color;
    //Out.Normal = float4(input.wNormal, 1.0f);
    //Out.SVPosiiton = input.Position;
    output.Position = input.Position;

    return output;
}