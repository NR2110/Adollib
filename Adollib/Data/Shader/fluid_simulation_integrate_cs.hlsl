#include "fluid_simulation.hlsli"

// 粒子位置と速度を保持するバッファ
//StructuredBuffer<Particle> _ParticlesBufferRead : register(t0);
StructuredBuffer<ParticleForces> _ParticlesForceBufferRead : register(t0);

RWStructuredBuffer<Particle> _ParticlesBufferWrite : register(u0);


//--------------------------------------------------------------------------------------
// 衝突判定などを行って、位置を更新
//--------------------------------------------------------------------------------------
[numthreads(THREAD_SIZE_X, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    const unsigned int P_ID = DTid.x; // 現在処理しているパーティクルID

	// 更新前の位置と速度
    float3 position = _ParticlesBufferWrite[P_ID].position;
    float3 velocity = _ParticlesBufferWrite[P_ID].velocity;
    float3 acceleration = _ParticlesForceBufferRead[P_ID].acceleration; // 計算した加速度

	// 衝突判定を書くならここ -----

	// 壁境界(ペナルティ法)
    float axis01_value = dot(position, _normal_01);
    float axis02_value = dot(position, _normal_02);
    float axis03_value = dot(position, _normal_03);

    acceleration += max(_Range_start.x - axis01_value, 0) * _WallStiffness * +_normal_01;
    acceleration += max(axis01_value - _Range_end.x, 0) * _WallStiffness * -_normal_01;
    acceleration += max(_Range_start.y - axis02_value, 0) * _WallStiffness * +_normal_02;
    acceleration += max(axis02_value - _Range_end.y, 0) * _WallStiffness * -_normal_02;

    //acceleration += max(_Range_start.z - axis03_value, 0) * _WallStiffness * +_normal_03;
    //acceleration += max(axis03_value - _Range_end.z, 0) * _WallStiffness * -_normal_03;


    //float dist = dot(position, float3(1, 0, 0));
    //acceleration += min(dist, 0) * -_WallStiffness * float3(1, 0, 0);

    //dist = dot(position, float3(0, 1, 0));
    //acceleration += min(dist, 0) * -_WallStiffness * float3(0, 1, 0);

    //dist = dot(position, float3(-1, 0, _Range.x));
    //acceleration += min(dist, 0) * -_WallStiffness * float3(-1, 0, 0);

    //dist = dot(position, float3(0, -1, _Range.y));
    //acceleration += min(dist, 0) * -_WallStiffness * float3(0, -1, 0);

	// 重力の加算
    acceleration += _Gravity;

	// 前進オイラー法で次の粒子位置を更新
    velocity += _TimeStep * acceleration;
    position += _TimeStep * velocity;

	// パーティクルのバッファ更新
    _ParticlesBufferWrite[P_ID].position = position;
    //_ParticlesBufferWrite[P_ID].position = _ParticlesBufferWrite[P_ID].position + float3(0, 1, 0);
    //float3 debug_pos_a = _ParticlesBufferWrite[P_ID].position;
    //_ParticlesBufferWrite[P_ID].position = float3(0, 1, 0) * _TimeStep;
    //float3 debug_pos_b = _ParticlesBufferWrite[P_ID].position;

    _ParticlesBufferWrite[P_ID].velocity = velocity;
    //_ParticlesBufferWrite[P_ID].velocity = debug_pos_a + debug_pos_b + velocity;
}