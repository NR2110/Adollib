#include "render_directional_shadow.hlsli"

// ���_�V�F�[�_�[
float4 main(VSInput input) : SV_Position
{
    float4 P = float4(input.Position, 1.0);
    float4x4 W = mul(Mesh_World, World);
	// ���[���h�ϊ����W
    float4 worldPos = mul(P, W);
	// �ˉe��Ԃɕϊ�(��D��)

    row_major float4x4 matWVP = mul(W, mul(View, Projection));
    P = mul(P, matWVP);

    return P;

}

//// ���_�V�F�[�_�[
//PSInput main(VSInput input) : SV_Position
//{
 //   PSInput output;

 //   float4 P = float4(input.Position, 1.0);
 //   float4x4 W = mul(Mesh_World, World);
	//// ���[���h�ϊ����W
 //   float4 worldPos = mul(P, W);
	//// �ˉe��Ԃɕϊ�(��D��)

 //   row_major float4x4 matWVP = mul(W, mul(View, Projection));
 //   P = mul(P, matWVP);

 //   output.Position = P;
 //   return output;

//}