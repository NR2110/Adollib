#include "rope_shader.hlsli"



//--------------------------------------------
//	���_�V�F�[�_�[
//--------------------------------------------

PSInput main(VSInput input, VSInput_INSTANCE instance_input)
{
    PSInput output = (PSInput) 0;

    float4 P = float4(input.Position, 1.0);
    float4 worldPos = mul(P, joint_rotate[input.BoneWeights.x]) + joint_position[input.BoneWeights.x];

	// �ˉe��Ԃɕϊ�(��D��)
    P = mul(worldPos, mul(View, Projection));

	// ���[���h�@���Z�o
    float3 N = mul(input.Normal, (float3x3) joint_rotate[input.BoneWeights.x]);
    N = normalize(N); //���K��

    output.Position = P;
    output.Color = materialColor * instance_input.color;
    output.Tex = input.Tex;
    output.wPos = worldPos.xyz;
    output.wNormal = N;

    return output;
}