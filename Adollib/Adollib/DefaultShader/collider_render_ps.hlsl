#include "collider_render.hlsli"

//	ピクセルシェーダー
PSOutput main(PSInput input)
{
    PSOutput Out = (PSOutput) 0;

    Out.Color = materialColor;

    return Out;
}