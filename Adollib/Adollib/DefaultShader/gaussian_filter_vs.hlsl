#include "gaussian_filter.hlsli"
//--------------------------------------------
//	頂点シェーダー
//--------------------------------------------

PSInput main(VSInput input)
{
    PSInput output = (PSInput) 0;

	// 出力値設定.
    output.Position = float4(input.Position, 1);
    output.Color = materialColor;
    output.Tex = input.Tex;

    return output;
}

