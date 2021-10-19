#include "head.hlsli"
//--------------------------------------------
//	データーフォーマット
//--------------------------------------------

struct PSInput
{
	float4 Position : SV_POSITION;
	float2 Tex : TEXCOORD;
	float4 Color : COLOR;
	float3 wPos : TEXCOORD1;

	//ワールド法線
	float3 wNormal:	TEXCOORD3;

};
