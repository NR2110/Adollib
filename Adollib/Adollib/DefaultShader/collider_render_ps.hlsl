#include "collider_render.hlsli"

//	�s�N�Z���V�F�[�_�[
PSOutput main(PSInput input)
{
    PSOutput Out = (PSOutput) 0;

    Out.Color = materialColor;

    return Out;
}