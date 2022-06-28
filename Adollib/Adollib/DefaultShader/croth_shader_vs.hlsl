#include "croth_shader.hlsli"
//--------------------------------------------
//	頂点シェーダー
//--------------------------------------------

PSInput main(VSInput input, VSInput_INSTANCE instance_input)
{
    PSInput output = (PSInput) 0;

    // inputlayoutを変更するのがめんどくさかったため ここで変換
    float3 position[3] = {
        instance_input.transform._11_12_13,
        instance_input.transform._14_21_22,
        instance_input.transform._23_24_31
    };

    float3 normal[3] =
    {
        instance_input.transform._32_33_34,
        instance_input.transform._41_42_43,
        float3(instance_input.transform._44, instance_input.texcoord_transform.xy)
    };

    float3 debug_normal = cross(float3(position[0] - position[1]), float3(position[0] - position[2]));
    debug_normal = normalize(debug_normal);

    float4 P = float4(position[input.vertexID], 1.0);
    //float4 P = float4(input.vertexID, input.vertexID, input.vertexID, 1);

    //float4x4 W = mul(Mesh_World, World);
    float4x4 W = World;
	// ワールド変換座標
    float4 worldPos = mul(P, W);
	// 射影空間に変換(列優先)

    //row_major float4x4 matWVP = mul(W, mul(View, Projection));
    row_major float4x4 matWVP =  mul(View, Projection);
    P = mul(P, matWVP);

	// ワールド法線算出
    float3 N = mul(normal[input.vertexID], (float3x3) W);
    //float3 N = mul(debug_normal, (float3x3) W);
    N = normalize(N); //正規化

   // N = float3(1, 0, 0);

    output.Position = P;
    output.Color = materialColor * instance_input.color;
    output.Color.w = 1;
    //output.Tex = input.Tex;
    output.Tex = float2(0, 0);
    output.wPos = worldPos.xyz;
    output.wNormal = N;

    return output;
}