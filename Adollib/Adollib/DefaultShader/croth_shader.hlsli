#include "head.hlsli"
//--------------------------------------------
//	データーフォーマット
//--------------------------------------------

//struct VSInput_INSTANCE_croth
//{
//    row_major float4x4 transform : TRANSFORM_MATRIX;
//    float4 texcoord_transform : TEXCOORD_TRANSFORM;
//    float4 color : COLOR;
//    uint instanceID : SV_InstanceID;
//};

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