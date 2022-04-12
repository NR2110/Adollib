#include "fluid_simulation.hlsli"

//--------------------------------------------------------------------------------------
// 衝突判定などを行って、位置を更新
//--------------------------------------------------------------------------------------
[numthreads(THREAD_SIZE_X, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    const unsigned int P_ID = DTid.x; // 現在処理しているパーティクルID

	// 更新前の位置と速度
    float3 position = _ParticlesBufferRead[P_ID].position;
    float3 velocity = _ParticlesBufferRead[P_ID].velocity;
    float3 acceleration = _ParticlesForceBufferRead[P_ID].acceleration; // 計算した加速度

	// 衝突判定を書くならここ -----

	// 壁境界(ペナルティ法)
    //float dist = dot(float3(position, 1), float3(1, 0, 0));
    //acceleration += min(dist, 0) * -_WallStiffness * float2(1, 0);

    //dist = dot(float3(position, 1), float3(0, 1, 0));
    //acceleration += min(dist, 0) * -_WallStiffness * float2(0, 1);

    //dist = dot(float3(position, 1), float3(-1, 0, _Range.x));
    //acceleration += min(dist, 0) * -_WallStiffness * float2(-1, 0);

    //dist = dot(float3(position, 1), float3(0, -1, _Range.y));
    //acceleration += min(dist, 0) * -_WallStiffness * float2(0, -1);

	// 重力の加算
    acceleration += _Gravity;

	// 前進オイラー法で次の粒子位置を更新
    velocity += _TimeStep * acceleration;
    position += _TimeStep * velocity;

	// パーティクルのバッファ更新
    _ParticlesBufferWrite[P_ID].position = position;
    _ParticlesBufferWrite[P_ID].velocity = velocity;
}