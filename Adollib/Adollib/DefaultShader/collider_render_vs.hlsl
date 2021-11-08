#include "collider_render.hlsli"
//--------------------------------------------
//	頂点シェーダー
//--------------------------------------------

PSInput main(VSInput input)
{
    PSInput output = (PSInput) 0;

    float4 P = float4(input.Position, 1.0);
    float4x4 W = mul(Mesh_World, World);
	// ワールド変換座標
    float4 worldPos = mul(P, W);
	// 射影空間に変換(列優先)

    row_major float4x4 matWVP = mul(W, mul(View, Projection));
    P = mul(P, matWVP);

    output.Position = P;
    output.Color = materialColor;

    return output;
}