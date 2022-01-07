#include "rope_shader.hlsli"



//--------------------------------------------
//	���_�V�F�[�_�[
//--------------------------------------------

PSInput main(VSInput input)
{
    PSInput output = (PSInput) 0;

    float4 P = mul(float4(input.Position, 1.0), joint_rotate[input.BoneIndices.x]) + joint_position[input.BoneIndices.x];
    P.w = 1;

    float4 worldPos = mul(P, World);

	// �ˉe��Ԃɕϊ�(��D��)
    row_major float4x4 matWVP = mul(World, mul(View, Projection));
    P = mul(P, matWVP);

	// ���[���h�@���Z�o
    float3 N = mul(input.Normal, (float3x3) joint_rotate[input.BoneIndices.x]);
    N = normalize(N); //���K��

    output.Position = P;
    output.Color = materialColor;
    output.Tex = input.Tex;
    output.wPos = worldPos.xyz;
    output.wNormal = N;

    return output;
}