#include "default.hlsli"
//--------------------------------------------
//	���_�V�F�[�_�[
//--------------------------------------------

PSInput main(VSInput input)
{
	PSInput output = (PSInput)0;

	float4 P = float4(input.Position, 1.0);
    float4x4 W = mul(Mesh_World, World);
	// ���[���h�ϊ����W
    float4 worldPos = mul(W, P);
	// �ˉe��Ԃɕϊ�(��D��)
    row_major float4x4 matWVP = mul(W, mul(View, Projection));
	P = mul(P, matWVP);
	// ���[���h�@���Z�o
    float3 N = mul(input.Normal, (float3x3) W);
	N = normalize(N);//���K��

	output.Position = P;
	output.Color = materialColor;
	output.Tex = input.Tex;
	output.wPos = worldPos.xyz;
	output.wNormal = N;

	return output;
}