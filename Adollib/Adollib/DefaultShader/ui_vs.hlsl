#include "ui.hlsli"

PSInput main(VSInput input)
{
    PSInput output = (PSInput) 0;

    output.Position = float4(input.Position, 1);
    output.Color = float4(1, 1, 1, 1);
    output.Tex = input.Tex;

    return output;
}