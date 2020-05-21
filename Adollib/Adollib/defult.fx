
#include "Func.fx"
//--------------------------------------------
//	テクスチャ
//--------------------------------------------


Texture2D DiffuseTexture : register(t0);
SamplerState DecalSampler : register(s0);

//--------------------------------------------
//	グローバル変数
//--------------------------------------------

cbuffer CBPerMesh : register(b0)
{
	matrix  World;
	matrix	matWVP;
};


cbuffer CBPerFrame2 : register(b2)
{
	float4	LightColor;		//ライトの色
	float4	LightDir;		//ライトの方向
	float4	AmbientColor;	//環境光
};


//--------------------------------------------
//	データーフォーマット
//--------------------------------------------

struct VSInput
{
	float3 Position : POSITION;
	float3 Normal   : NORMAL;
	float2 Tex      : TEXCOORD;
	float4 Color    : COLOR;
};

struct PSInput
{
	float4 Position : SV_POSITION;
	float2 Tex : TEXCOORD;
	float4 Color : COLOR;
};

struct PSInput2
{
	float4 Position : SV_POSITION;
	float2 Tex : TEXCOORD;
	float4 Color : COLOR;
};

//--------------------------------------------
//	頂点シェーダー
//--------------------------------------------

PSInput VSMain(VSInput input)
{
	PSInput output = (PSInput)0;

	float4 P = float4(input.Position, 1.0);

	// 射影空間に変換.
	P = mul(matWVP, P);

	// ワールド法線算出
	float3 N = mul((float3x3)World, input.Normal);
	N = normalize(N);//正規化

	//　ライト計算
	float3 L = normalize(LightDir.xyz);
	//float D = max(0, dot(-L, N));
	float3 C = LightColor.rgb;
	// 反射率
	float3 Kd = float3(1, 1, 1);
	// 拡散反射計算
	//float D = max(0, dot(-L, N));
	float3 D = Diffuse(N, L, C, Kd);




	// 出力値設定.
	output.Position = P;
	output.Color = input.Color;
	output.Color.rgb = D + AmbientColor.rgb;
	output.Tex = input.Tex;
	return output;
}

//--------------------------------------------
//	ピクセルシェーダー
//--------------------------------------------

float4 PSMain(PSInput input) : SV_TARGET0
{


	return DiffuseTexture.Sample(DecalSampler, input.Tex) * input.Color;
}
