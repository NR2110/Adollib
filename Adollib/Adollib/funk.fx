
//--------------------------------------------
//	�g�U���ˊ֐�
//--------------------------------------------
// N:�@��(���K���ς�)
// L:���˃x�N�g��(���K���ς�)
// C:���ˌ�(�F�E����)
// K:���˗�(0�`1.0)

//�����o�[�g�V�F�[�f�B���O
float3 Diffuse(float3 N, float3 L, float3 C, float3 K)
{
	float D = dot(N, -L);
	D = max(0, D);			// ���̒l���O�ɂ���
	return K * C * D;
}

//--------------------------------------------
//	���ʔ��ˊ֐�(�u�����E�t�H��)
//--------------------------------------------
// N:�@��(���K���ς�)
// L:���˃x�N�g��(���K���ς�)
// C:���ˌ�(�F�E����)
// E:���_�����x�N�g��(���K���ς�)
// K:���˗�(0�`1.0)
// Power:�n�C���C�g�̋���(�P���x)

float3 BlinnPhongSpecular(float3 N, float3 L, float3 C, float3 E,
	float3 K, float Power)
{
	//�n�[�t�x�N�g��
	float3 H = normalize(-L + E);

	float3 S = dot(H, N);
	S = max(0, S);
	S = pow(S, Power);
	S = S * K * C;
	return S;
}

//--------------------------------------------
//	���ʔ��ˊ֐��i�t�H���j
//--------------------------------------------
// N:�@��(���K���ς�)
// L:���˃x�N�g��(���K���ς�)
// C:���ˌ�(�F�E����)
// E:���_�����x�N�g��(���K���ς�)
// K:���˗�(0�`1.0)
// Power:�n�C���C�g�̋���(�P���x)

//�t�H���V�F�[�f�B���O
float3 PhongSpecular(float3 N, float3 L, float3 C, float3 E,
	float3 K, float Power)
{
	float3 R = reflect(L, N);
	R = normalize(R);
	float3 S = dot(R, E);
	S = max(0, S);
	S = pow(S, Power);
	S = S * K * C;
	return S;
}

//--------------------------------------------
//	�t�H�O
//--------------------------------------------
//color		:���݂̐F
//viewPos	:���_
//Pos		:�s�N�Z���̈ʒu
//fogColor	:�t�H�O�̐F
//Near		:�t�H�O�̋ߋ���
//Far		:�t�H�O�̉�����
float3 Fog(float3 color, float3 viewPos, float3 Pos, float3 fogColor, float Near, float Far)
{
	//�t�H�O�v�Z
	float Len = length(viewPos - Pos);
	float fogalpha = saturate((Len - Near) / (Far - Near));
	return color.rgb * (1.0 - fogalpha) + fogColor * fogalpha;
}


//--------------------------------------------
//	�������C�g
//--------------------------------------------

float3 HemiSphereLight(float3 N, float3 SkyColor, float3 GroundColor)
{
	float skyblend = (N.y + 1.0) * 0.5;
	float groundblend = 1.0 - skyblend;
	return  SkyColor * skyblend + GroundColor * groundblend;
}

//--------------------------------------------
//	�g�D�[���V�F�[�_�p���C�g
//--------------------------------------------
//�R�K��
float3 ToonLight(float3 N, float3 L, float3 C, float3 K)
{
	float D = dot(N, -L);
	//D = max(0, D);			// ���̒l���O�ɂ���
	if (D < 0.3) D = 0.2;
	else if (D < 0.9) D = 0.6;
	else D = 1.4;


	return K * C * D;
}


//--------------------------------------------
//	�g�D�[���V�F�[�_�p�g�U���ˊ֐�
//--------------------------------------------
//�Q�K��
float3 ToonDiffuse(float3 N, float3 L, float3 C, float3 K)
{
	float D = dot(N, -L);
	//D = max(0, D);			// ���̒l���O�ɂ���
	if (D < 0.3) D = 0.2;
	else D = 0.6;


	return K * C * D;
}
//--------------------------------------------
//	�g�D�[���V�F�[�_�p���ʔ��ˊ֐�
//--------------------------------------------
//�Q�K��

float3 ToonSpecular(float3 N, float3 L, float3 C, float3 E,
	float3 K, float Power)
{
	float3 R = reflect(L, N);
	R = normalize(R);
	float S = dot(R, E);
	S = max(0, S);
	S = pow(S, Power);
	if (S > 0.5) S = 0.8;
	else S = 0.4;
	return S * K * C;
}

//--------------------------------------------
//	�V���h�[�}�b�s���O�p�֐�
//--------------------------------------------

//���[���h���W�����C�g��ԍ��W�ɕϊ�
float3	GetShadowTex(float4x4 vp, float3 wPos)
{
	// ���K���f�o�C�X���W�n
	float4	wvpPos;
	wvpPos.xyz = wPos;
	wvpPos.w = 1;
	wvpPos = mul(vp, wvpPos);
	wvpPos /= wvpPos.w;
	// �e�N�X�`�����W�n
	wvpPos.y = -wvpPos.y;
	wvpPos.xy = 0.5f * wvpPos.xy + 0.5f;

	return wvpPos.xyz;
}
//�V���h�[�}�b�v���烉�C�g��ԍ��W�ɕϊ���Z�l��r
float3	GetShadow(Texture2D st, SamplerState ss, float3 Tex, float3 Scolor, float Bias)
{	// �V���h�E�}�b�v����[�x�����o��
	float d = st.Sample(ss, Tex.xy).r;
	// �V���h�E�}�b�v�̐[�x�l�ƌ����̐[�x�̔�r
	Scolor = (Tex.z - d > Bias) ? Scolor : 1.0;
	return Scolor;
}

