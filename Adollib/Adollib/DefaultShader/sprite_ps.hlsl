#include "sprite.hlsli"

//	ピクセルシェーダー
PSOutput main(PSInput input)
{
    PSOutput Out = (PSOutput) 0;

    float4 Color = input.Color * DiffuseTexture.Sample(DecalSampler, input.Tex);
    Color.w = input.Color.w;

    //Color = input.Color;

    Out.Color = Color;
    Out.Normal = float4(input.wNormal, 1.0f);
    Out.SVPosiiton = input.Position;

    return Out;
}