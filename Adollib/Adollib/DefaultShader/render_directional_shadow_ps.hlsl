#include "render_directional_shadow.hlsli"

//	ピクセルシェーダー
float4 main(PSInput input) : SV_TARGET0
{
    //PSOutput output;
    ////PSOutput Out = (PSOutput) 0;

    ////float A = (dot(input.wNormal, normalize(float3(0, 1, 1))) + 1) / 4 + 0.5; //影
    ////float4 Color = input.Color * DiffuseTexture.Sample(DecalSampler, input.Tex) * A;
    ////Color.w = input.Color.w;

    //////Color = input.Color;

    ////Out.Color = Color;
    ////Out.Normal = float4(input.wNormal, 1.0f);
    ////Out.SVPosiiton = input.Position;
    //output.Position = input.Position;

    return input.Position;
}