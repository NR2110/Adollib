#include "gaussian_filter.hlsli"
//--------------------------------------------
//	���_�V�F�[�_�[
//--------------------------------------------

PSInput main(VSInput input)
{
    PSInput output = (PSInput) 0;

	// �o�͒l�ݒ�.
    output.Position = float4(input.Position, 1);
    output.Color = materialColor;
    output.Tex = input.Tex;

    return output;
}

