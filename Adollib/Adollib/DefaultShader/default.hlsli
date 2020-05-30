#include "head.hlsli"
//--------------------------------------------
//	�f�[�^�[�t�H�[�}�b�g
//--------------------------------------------
#define MAX_BONE_INFLUENCES 32
struct VSInput
{
	float3 Position : POSITION;
	float3 Normal   : NORMAL;
	float2 Tex      : TEXCOORD;
	float4 BoneWeights : WEIGHTS;
	uint4  BoneIndices : BONES;
};

struct PSInput
{
	float4 Position : SV_POSITION;
	float2 Tex : TEXCOORD;
	float4 Color : COLOR;
	float3 wPos : TEXCOORD1;

	//���[���h�@��
	float3 wNormal:	TEXCOORD3;

};
