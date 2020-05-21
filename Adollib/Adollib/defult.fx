
#include "Func.fx"
//--------------------------------------------
//	�e�N�X�`��
//--------------------------------------------


Texture2D DiffuseTexture : register(t0);
SamplerState DecalSampler : register(s0);

//--------------------------------------------
//	�O���[�o���ϐ�
//--------------------------------------------

cbuffer CBPerMesh : register(b0)
{
	matrix  World;
	matrix	matWVP;
};


cbuffer CBPerFrame2 : register(b2)
{
	float4	LightColor;		//���C�g�̐F
	float4	LightDir;		//���C�g�̕���
	float4	AmbientColor;	//����
};


//--------------------------------------------
//	�f�[�^�[�t�H�[�}�b�g
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
//	���_�V�F�[�_�[
//--------------------------------------------

PSInput VSMain(VSInput input)
{
	PSInput output = (PSInput)0;

	float4 P = float4(input.Position, 1.0);

	// �ˉe��Ԃɕϊ�.
	P = mul(matWVP, P);

	// ���[���h�@���Z�o
	float3 N = mul((float3x3)World, input.Normal);
	N = normalize(N);//���K��

	//�@���C�g�v�Z
	float3 L = normalize(LightDir.xyz);
	//float D = max(0, dot(-L, N));
	float3 C = LightColor.rgb;
	// ���˗�
	float3 Kd = float3(1, 1, 1);
	// �g�U���ˌv�Z
	//float D = max(0, dot(-L, N));
	float3 D = Diffuse(N, L, C, Kd);




	// �o�͒l�ݒ�.
	output.Position = P;
	output.Color = input.Color;
	output.Color.rgb = D + AmbientColor.rgb;
	output.Tex = input.Tex;
	return output;
}

//--------------------------------------------
//	�s�N�Z���V�F�[�_�[
//--------------------------------------------

float4 PSMain(PSInput input) : SV_TARGET0
{


	return DiffuseTexture.Sample(DecalSampler, input.Tex) * input.Color;
}
