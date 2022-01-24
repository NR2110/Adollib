#include "default.hlsli"
#include "math.hlsli"

//	ピクセルシェーダー
PSOutput main(PSInput input)
{
    PSOutput Out = (PSOutput) 0;

    //Color = input.Color;

    //float3 ShadowPos = 0;
    //ShadowPos = GetShadowTex(shadow_viewprojection, input.wPos.xyz);
    //シャドーマップから深度取得と比較


    //float3 Scolor = float3(0.5f, 0.5f, 0.5f);
    //float d = ShadowTexture.Sample(DecalSampler, ShadowPos.xy).r;
    //Scolor = (ShadowPos.z - d > 0.0008f) ? Scolor : 1.0;
    //Color.xyz *= Scolor;

   //Color.xyz *= GetShadow(ShadowTexture, DecalSampler, ShadowPos, Color.xyz * 0.8f, 0.0008f);

    Out.Color = input.Color;
    Out.Normal = float4(input.wNormal, 1.0f);
    Out.SVPosiiton = input.Position;

    return Out;
}