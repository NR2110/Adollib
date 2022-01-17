#include "dither_noshadow.hlsli"

#include "bayermatrix_func.hlsli"

//	ピクセルシェーダー
PSOutput main(PSInput input)
{
    float camera_dis = dot(Eyedir.xyz, input.wPos - EyePos.xyz);
    clip(camera_dis - (Bayermatrix(input.Position.xy) - 1) * dither_length - dither_start);

    PSOutput Out = (PSOutput) 0;

    float A = (dot(input.wNormal, -right_dir) + 0) * 0.2 + 0.8; //影
    float4 Color = input.Color * DiffuseTexture.Sample(DecalSampler, input.Tex) * A;
    Color.w = input.Color.w;

    //Color = input.Color;

    float3 ShadowPos = 0;
    ShadowPos = GetShadowTex(shadow_viewprojection, input.wPos.xyz);

    Out.Color = Color;
    Out.Normal = float4(input.wNormal, 1.0f);
    Out.SVPosiiton = input.Position;

    return Out;
}