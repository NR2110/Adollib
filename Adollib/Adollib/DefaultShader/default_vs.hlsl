#include "default.hlsli"
//--------------------------------------------
//	頂点シェーダー
//--------------------------------------------

PSInput main(VSInput input)
{
	PSInput output = (PSInput)0;

	float4 P = float4(input.Position, 1.0);
    float4x4 W = mul(Mesh_World, World);
	// ワールド変換座標
    float4 worldPos = mul(W, P);
	// 射影空間に変換(列優先)
    row_major float4x4 matWVP = mul(W, mul(View, Projection));
	P = mul(P, matWVP);
	// ワールド法線算出
    float3 N = mul(input.Normal, (float3x3) W);
	N = normalize(N);//正規化

	output.Position = P;
	output.Color = materialColor;
	output.Tex = input.Tex;
	output.wPos = worldPos.xyz;
	output.wNormal = N;

	return output;
}