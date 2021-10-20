#include "default.hlsli"
#include "math.hlsli"

//--------------------------------------------
//	ピクセルシェーダー
//--------------------------------------------
PSOutput main(PSInput input)
{
    PSOutput Out = (PSOutput) 0;

	float A = (dot(input.wNormal, normalize(float3(0,1,1))) + 1) / 4 + 0.5;
    float4 Color = input.Color * A;
    Color.w = input.Color.w;

    Out.Color = Color;
    Out.Normal = float4(input.wNormal, 1.0f);
    Out.SVPosiiton = input.Position;

    return Out;
}