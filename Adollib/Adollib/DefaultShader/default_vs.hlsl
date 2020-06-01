#include "default.hlsli"
//--------------------------------------------
//	頂点シェーダー
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
	// ワールド変換座標
	float4 worldPos = mul(World, P);
	// 射影空間に変換(列優先)
	row_major float4x4 matWVP = mul(World, mul(View, Projection));
	P = mul(P, matWVP);
	// ワールド法線算出
	float3 N = mul((float3x3)World, input.Normal);
	N = normalize(N);//正規化

	// // 視線ベクトル算出
	//float3 E = EyePos.xyz;				// カメラ位置
	//E = normalize(E - worldPos.xyz);	// 視点方向ベクトル
	//									// ライトベクトル
										// 出力値設定.
	output.Position = P;
	output.Color = materialColor;
	//output.Color = float4(1, 1, 1, 1);
	output.Tex = input.Tex;
	output.wPos = worldPos.xyz;
	//output.wEyeDir = E;
	output.wNormal = N;

	return output;
}