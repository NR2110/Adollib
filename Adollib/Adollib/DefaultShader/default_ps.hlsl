#include "default.hlsli"

#include "bayermatrix_func.hlsli"

//	ピクセルシェーダー
PSOutput main(PSInput input)
{
    //float camera_dis = dot(Eyedir.xyz, input.wPos - EyePos.xyz);
    //clip(camera_dis - Bayermatrix(input.Position.xy) * 30 + 10);


    PSOutput Out = (PSOutput) 0;

    float A = (dot(input.wNormal, -right_dir) + 0) * 0.2 + 0.8; //影
    float4 Color = input.Color * DiffuseTexture.Sample(DecalSampler, input.Tex) * A;
    Color.w = input.Color.w;

    //Color = input.Color;

    float3 ShadowPos = 0;
    ShadowPos = GetShadowTex(shadow_viewprojection, input.wPos.xyz);
    //シャドーマップから深度取得と比較


    //float3 Scolor = float3(0.5f, 0.5f, 0.5f);
    //float d = ShadowTexture.Sample(DecalSampler, ShadowPos.xy).r;
    //Scolor = (ShadowPos.z - d > 0.0008f) ? Scolor : 1.0;
    //Color.xyz *= Scolor;

    Color.xyz *= GetShadow(ShadowTexture, DecalSampler, ShadowPos, Color.xyz * 0.8f, 0.00008f);

    Out.Color = Color;
    Out.Normal = float4(input.wNormal, 1.0f);
    Out.SVPosiiton = input.Position;

    return Out;
}