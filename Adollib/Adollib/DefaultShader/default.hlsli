#include "head.hlsli"
//--------------------------------------------
//	�f�[�^�[�t�H�[�}�b�g
//--------------------------------------------

struct PSInput
{
	float4 Position : SV_POSITION;
	float2 Tex : TEXCOORD;
	float4 Color : COLOR;
	float3 wPos : TEXCOORD1;

	//���[���h�@��
	float3 wNormal:	TEXCOORD3;

};
