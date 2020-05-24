#include "func.fx"
#include "light_type.fx"
//--------------------------------------------
//	�e�N�X�`��
//--------------------------------------------
Texture2D DiffuseTexture : register(t0);
SamplerState DecalSampler : register(s0);


//--------------------------------------------
//	�O���[�o���ϐ�
//--------------------------------------------
cbuffer CBPerObj : register(b0)
{
	matrix  World;
};

cbuffer CBPerCamera : register(b1)
{
	matrix  View;
};

cbuffer CBPerSystem : register(b2)
{
	matrix  Projection;
};


cbuffer CBPerFrame : register(b3)
{
	float4  LightColor;		//���C�g�̐F
	float4	LightDir;		//���C�g�̕���
	float4  AmbientColor;	//����
	float4	EyePos;			//�J�����ʒu
	float	param1;
	float	param2;
	float	param3;
	float	param4;

	POINTLIGHT  PointLight[POINTMAX];	//�|�C���g���C�g�z��
	SPOTLIGHT  SpotLight[SPOTMAX];		//�X�|�b�g���C�g�z��

};

static const float viewOffset = 0.06f;

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
	float3 wPos : TEXCOORD1;
	//float3 wEyeDir: TEXCOORD2;

	//���[���h�@��
	float3 wNormal:	TEXCOORD3;

};



//--------------------------------------------
//	���_�V�F�[�_�[
//--------------------------------------------

PSInput VSMain(VSInput input)
{
	PSInput output = (PSInput)0;

	float4 P = float4(input.Position, 1.0);
	// ���[���h�ϊ����W
	float4 worldPos = mul(World, P);
	// �ˉe��Ԃɕϊ�(��D��)
	P = mul(matWVP, P);
	// ���[���h�@���Z�o
	float3 N = mul((float3x3)World, input.Normal);
	N = normalize(N);//���K��

	// // �����x�N�g���Z�o
	//float3 E = EyePos.xyz;				// �J�����ʒu
	//E = normalize(E - worldPos.xyz);	// ���_�����x�N�g��
	//									// ���C�g�x�N�g��

										// �o�͒l�ݒ�.
	output.Position = P;
	//	output.Color = input.Color;
	output.Color = float4(1, 1, 1, 1);
	output.Tex = input.Tex;
	output.wPos = worldPos.xyz;
	//output.wEyeDir = E;
	output.wNormal = N;

	return output;
}

//--------------------------------------------
//	�s�N�Z���V�F�[�_�[
//--------------------------------------------
float4 PSMain(PSInput input) : SV_TARGET
{

	float3 L = normalize(LightDir.xyz);
	float3 E = normalize(EyePos.xyz - input.wPos);

	// ����
	float3 Ambient = AmbientColor.rgb;//
	float4 color = DiffuseTexture.Sample(DecalSampler, input.Tex);
	float3 C = LightColor.rgb;		//���ˌ�(�F�Ƌ���)

	// �@��
	float3 N = normalize(input.wNormal);

	// �g�U����
	float3 Kd = float3(1, 1, 1);
	float3 D = Diffuse(N, L, C, Kd);

	// ���ʔ���
	float3 Ks = float3(1, 1, 1);
	float3 S = BlinnPhongSpecular(N, L, C, E, Ks, 20);


	//�|�C���g���C�g
	float3 PL;
	float3 PC;
	float influence;
	float3 PD = (float3)0;
	float3 PS = (float3)0;
	for (int i = 0; i < POINTMAX; i++) {
		if (PointLight[i].type == 0) continue;
		PL = input.wPos - PointLight[i].pos.xyz;
		float d = length(PL);

		float r = PointLight[i].range;
		if (d > r) continue;
		influence = saturate(1.0f - d / r);
		PL = normalize(PL);
		PC = PointLight[i].color.rgb;
		PD += Diffuse(N, PL, PC, Kd) * influence * influence;
		PS += BlinnPhongSpecular(N, PL, PC, E, Ks, 20) * influence * influence;
	}
	//�X�|�b�g���C�g
	float3 SL;
	float3 SC;
	float influence2;
	float3 SD = (float3)0;
	float3 SS = (float3)0;
	for (int j = 0; j < SPOTMAX; j++) {
		if (SpotLight[j].type == 0) continue;

		SL = input.wPos - SpotLight[j].pos.xyz;
		float d = length(SL);

		float r = SpotLight[j].range;
		if (d > r) continue;
		float3 SFront = normalize(SpotLight[j].dir.xyz);
		SL = normalize(SL);
		float angle = dot(SL, SFront);
		float area = SpotLight[j].inner_corn - SpotLight[j].outer_corn;
		float influence3 = SpotLight[j].inner_corn - angle;
		influence3 = saturate(1.0 - influence3 / area);
		influence2 = 1;
		SC = SpotLight[j].color.rgb;
		SD += Diffuse(N, SL, SC, Kd) * influence2 * influence2 * influence3;
		SS += BlinnPhongSpecular(N, SL, SC, E, Ks, 20) * influence2 * influence2 * influence3;
	}


	return color * input.Color * float4(Ambient + D + S + PD + PS + SD + SS , 1.0f);
}
