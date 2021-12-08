
#include "light_func.hlsli"
#include "light_type.hlsli"


//:::::::::::::::::::::::::::::::::::::::::::::
Texture2D DiffuseTexture : register(t0);
Texture2D ShadowTexture : register(t1);
SamplerState DecalSampler : register(s0);



//:::::::::::::::::::::::::::::::::::::::::::::
#define MAX_BONE_INFLUENCES 32
struct VSInput
{
    float3 Position : POSITION;
    float3 Normal : NORMAL;
    float2 Tex : TEXCOORD;
    float4 BoneWeights : WEIGHTS;
    uint4 BoneIndices : BONES;
};

struct VSInput_INSTANCE
{
    row_major float4x4 transform : TRANSFORM_MATRIX;
    float4 texcoord_transform : TEXCOORD_TRANSFORM;
    float4 color : COLOR;
    uint instanceID : SV_InstanceID;
};



//:::::::::::::::::::::::::::::::::::::::::::::
cbuffer CBPerObj : register(b0)
{
	// world
    row_major matrix World;
};

cbuffer CBPerCamera : register(b1)
{
	// view
    row_major matrix View;
	float4	EyePos;
    float4  Eyedir;
};

cbuffer CBPerSystem : register(b2)
{
	// projection
    row_major matrix Projection;
};

cbuffer CBPerMesh : register(b3)
{
	// mesh data
    row_major matrix Mesh_World;
};

cbuffer CBPerLight : register(b4)
{
	// light data
	float4  LightColor;		//���C�g�̐F
	float4	LightDir;		//���C�g�̕���
	float4  AmbientColor;	//����

	POINTLIGHT  PointLight[POINTMAX];	//�|�C���g���C�g�z��
	SPOTLIGHT  SpotLight[SPOTMAX];		//�X�|�b�g���C�g�z��

};

#define MAX_BONES 4
cbuffer CBPerMaterial : register(b5)
{
    row_major float4x4 BoneTransforms[MAX_BONES];

    float4 materialColor;
    float4 ambientColor; // ����

    float shininess; // �X�y�L�����W��
};

//::::::::::::::::

cbuffer CBPerShadow : register(b7)
{
    row_major matrix shadow_viewprojection;
    float3 right_dir;
    float dummy;
};

cbuffer CBPerDither : register(b8)
{
    float range = 10;
};