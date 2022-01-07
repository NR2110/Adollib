#include "head.hlsli"
//--------------------------------------------
//	データーフォーマット
//--------------------------------------------

#define MAX_JOINTS 200
cbuffer CBPerJoint : register(b9)
{
    float4 joint_position[MAX_JOINTS];
    row_major float4x4 joint_rotate[MAX_JOINTS];
};


struct PSInput
{
    float4 Position : SV_POSITION;
    float2 Tex : TEXCOORD;
    float4 Color : COLOR;
    float3 wPos : TEXCOORD1;

	//ワールド法線
    float3 wNormal : TEXCOORD3;

};


struct PSOutput
{
    float4 Color : SV_TARGET0;
    float4 Normal : SV_TARGET1;
    float4 SVPosiiton : SV_TARGET2;

};