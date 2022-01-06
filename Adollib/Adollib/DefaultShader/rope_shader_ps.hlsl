#include "rope_shader.hlsli"

//	�s�N�Z���V�F�[�_�[
PSOutput main(PSInput input)
{
    PSOutput Out = (PSOutput) 0;

    float A = (dot(input.wNormal, -right_dir) + 0) * 0.2 + 0.8; //�e
    float4 Color = input.Color * DiffuseTexture.Sample(DecalSampler, input.Tex) * A;
    Color.w = input.Color.w;

    float3 ShadowPos = 0;
    ShadowPos = GetShadowTex(shadow_viewprojection, input.wPos.xyz);
    //�V���h�[�}�b�v����[�x�擾�Ɣ�r
    float shadow_pow = 0;
    shadow_pow += GetShadow(ShadowTexture, DecalSampler, ShadowPos, 0.00008f);

    Color.xyz *= Color.xyz * 0.8f * shadow_pow + float3(1, 1, 1) * (1 - shadow_pow);

    Out.Color = Color;
    Out.Normal = float4(input.wNormal, 1.0f);
    Out.SVPosiiton = input.Position;

    return Out;
}