#include "render_directional_shadow.hlsli"

// 頂点シェーダー
float4 main(VSInput input) : SV_Position
{
    float4 P = float4(input.Position, 1.0);
    float4x4 W = mul(Mesh_World, World);
	// ワールド変換座標
    float4 worldPos = mul(P, W);
	// 射影空間に変換(列優先)

    row_major float4x4 matWVP = mul(W, mul(View, Projection));
    P = mul(P, matWVP);

    return P;

}

//// 頂点シェーダー
//PSInput main(VSInput input) : SV_Position
//{
 //   PSInput output;

 //   float4 P = float4(input.Position, 1.0);
 //   float4x4 W = mul(Mesh_World, World);
	//// ワールド変換座標
 //   float4 worldPos = mul(P, W);
	//// 射影空間に変換(列優先)

 //   row_major float4x4 matWVP = mul(W, mul(View, Projection));
 //   P = mul(P, matWVP);

 //   output.Position = P;
 //   return output;

//}