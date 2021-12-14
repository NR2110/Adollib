#include "croth_shader.hlsli"

struct CS_out
{
    float3 position[3];

    float3 normal[3];

    float2 texcoordTransform;

    float4 color;
};

// CSInput structure
struct CSInput
{
    uint3 groupThread : SV_GroupThreadID;
    uint3 group       : SV_GroupID;
    uint  groupIndex  : SV_GroupIndex;
    uint3 dispatch    : SV_DispatchThreadID;
};
struct VertexFormat
{
#define MAX_BONE_INFLUENCES (4)
    float3 position;
    float3 normal;
    float2 texcoord;
    float bone_weights[MAX_BONE_INFLUENCES];
    int bone_indices[MAX_BONE_INFLUENCES];
};

StructuredBuffer<uint>         indexces : register(t0); //!< ���͗p.
StructuredBuffer<VertexFormat> vertices : register(t1); //!< ���͗p.
StructuredBuffer<float4>       color    : register(t2); //!< ���͗p.
RWStructuredBuffer<CS_out>     BufOut   : register(u0); //!< �o�͗p.


[numthreads(64, 1, 1)]
void main( const CSInput input )
{
    uint instance_num = floor((float) input.groupIndex / 3);

    uint vertex_mum = input.groupIndex - 3 * instance_num;


    BufOut[instance_num].position[vertex_mum] = vertices[indexces[input.groupIndex]].position;
    BufOut[instance_num].normal  [vertex_mum] = vertices[indexces[input.groupIndex]].normal;
    BufOut[instance_num].color = color[0];

}