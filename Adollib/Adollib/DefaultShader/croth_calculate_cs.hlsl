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
struct VertexOffset
{
    float3 position;

    float3 normal;
};

StructuredBuffer<uint>         indexces : register(t0); //!< 入力用.
StructuredBuffer<VertexFormat> vertices : register(t1); //!< 入力用.
StructuredBuffer<VertexOffset> vertices_offset : register(t2); //!< 入力用.
StructuredBuffer<float4>       color    : register(t3); //!< 入力用.
RWStructuredBuffer<CS_out>     BufOut   : register(u0); //!< 出力用.


[numthreads(32, 1, 1)]
void main( const CSInput input )
{
    // mesh毎に走るためinput.dispatch.xは。  0,0,0,1,1,1,2,2,2,3,3,3,...
    const uint instance_num = floor((float) input.dispatch.x / 3);

    // そのinstance内のvertexの番号 0,1,2,0,1,2,0,1,2,0,1,2,...
    const uint vertex_num = input.dispatch.x - 3 * instance_num;

    const uint index_num = indexces[input.dispatch.x]; //index

    // color[0].wはそのmeshのinstanceのstart位置
    BufOut[color[0].w + instance_num].position[vertex_num] = vertices[index_num].position + vertices_offset[index_num].position;
    BufOut[color[0].w + instance_num].normal[vertex_num] = vertices_offset[index_num].normal;
    //BufOut[color[0].w + instance_num].normal[vertex_mum] = normalize(vertices[index_num].normal + vertices_offset[index_num].normal);
    BufOut[color[0].w + instance_num].color = float4(color[0].xyz, 1);

}