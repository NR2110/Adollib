#include "sprite.hlsli"

//	ピクセルシェーダー
PSOutput main(PSInput input)
{
    PSOutput Out = (PSOutput) 0;

    float4 tex_color = DiffuseTexture.Sample(DecalSampler, input.Tex);
    float4 Color = input.Color * tex_color;

    clip(tex_color.w - 0.01f);

    Out.Color = Color;
    Out.Normal = float4(input.wNormal, 1.0f);
    Out.SVPosiiton = input.Position;

    return Out;
}