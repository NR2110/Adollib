#include "default.hlsli"
//--------------------------------------------
//	���_�V�F�[�_�[
//--------------------------------------------

PSInput main(VSInput input)
{
	PSInput output = (PSInput)0;

	//// BONE
	//float3 p = { 0, 0, 0 };
	//float3 n = { 0, 0, 0 };
	//int i = 0;
	//for (i = 0; i < 4; i++)
	//{
	//	p += (input.BoneWeights[i] * mul(input.Position, BoneTransforms[input.BoneIndices[i]])).xyz;
	//	n += (input.BoneWeights[i] * mul(float4(input.Normal.xyz, 0), BoneTransforms[input.BoneIndices[i]])).xyz;
	//}
	//input.Position = float4(p, 1.0f);
	//input.Normal = float4(n, 1);

	float4 P = float4(input.Position, 1.0);
	// ���[���h�ϊ����W
	float4 worldPos = mul(World, P);
	// �ˉe��Ԃɕϊ�(��D��)
	row_major float4x4 matWVP = mul(World, mul(View, Projection));
	P = mul(P, matWVP);
	// ���[���h�@���Z�o
	float3 N = mul(input.Normal, (float3x3)World);
	N = normalize(N);//���K��

	// // �����x�N�g���Z�o
	//float3 E = EyePos.xyz;				// �J�����ʒu
	//E = normalize(E - worldPos.xyz);	// ���_�����x�N�g��
	//									// ���C�g�x�N�g��
										// �o�͒l�ݒ�.
	output.Position = P;
	output.Color = materialColor;
	//output.Color = float4(1, 1, 1, 1);
	output.Tex = input.Tex;
	output.wPos = worldPos.xyz;
	//output.wEyeDir = E;
	output.wNormal = N;

	return output;
}