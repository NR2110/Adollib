#include "collider_render.hlsli"
//--------------------------------------------
//	���_�V�F�[�_�[
//--------------------------------------------

PSInput main(VSInput input)
{
    PSInput output = (PSInput) 0;

    float4 P = float4(input.Position, 1.0);
    float4x4 W = mul(Mesh_World, World);
	// ���[���h�ϊ����W
    float4 worldPos = mul(P, W);
	// �ˉe��Ԃɕϊ�(��D��)

    row_major float4x4 matWVP = mul(W, mul(View, Projection));
    P = mul(P, matWVP);

    output.Position = P;
    output.Color = materialColor;

    return output;
}