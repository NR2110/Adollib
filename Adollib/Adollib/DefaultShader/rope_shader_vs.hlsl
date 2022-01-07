#include "rope_shader.hlsli"



//--------------------------------------------
//	頂点シェーダー
//--------------------------------------------

PSInput main(VSInput input)
{
    PSInput output = (PSInput) 0;

    float4 P = mul(float4(input.Position, 1.0), joint_rotate[input.BoneIndices.x]) + joint_position[input.BoneIndices.x];
    P.w = 1;

    float4 worldPos = mul(P, World);

	// 射影空間に変換(列優先)
    row_major float4x4 matWVP = mul(World, mul(View, Projection));
    P = mul(P, matWVP);

	// ワールド法線算出
    float3 N = mul(input.Normal, (float3x3) joint_rotate[input.BoneIndices.x]);
    N = normalize(N); //正規化

    output.Position = P;
    output.Color = materialColor;
    output.Tex = input.Tex;
    output.wPos = worldPos.xyz;
    output.wNormal = N;

    return output;
}