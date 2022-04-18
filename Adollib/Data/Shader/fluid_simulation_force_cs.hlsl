#include "fluid_simulation.hlsli"

// 粒子位置と速度を保持するバッファ
StructuredBuffer<Particle> _ParticlesBufferRead : register(t0);
StructuredBuffer<ParticleDensity> _ParticlesDensityBufferRead : register(t1);
StructuredBuffer<ParticlePressure> _ParticlesPressureBufferRead : register(t2);

RWStructuredBuffer<ParticleForces> _ParticlesForceBufferWrite : register(u0);

//--------------------------------------------------------------------------------------
// 力(加速度)の算出
//--------------------------------------------------------------------------------------
[numthreads(THREAD_SIZE_X, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    uint P_ID = DTid.x; // 現在処理しているパーティクルID

    float3 P_position = _ParticlesBufferRead[P_ID].position;
    float3 P_velocity = _ParticlesBufferRead[P_ID].velocity;
    float P_density = _ParticlesDensityBufferRead[P_ID].density; // 先程算出した密度
    float P_pressure = _ParticlesPressureBufferRead[P_ID].pressure; // 先程算出した圧力

    const float h_sq = _Smoothlen * _Smoothlen;

	// 近傍探索(O(n^2))
    float3 press = float3(0, 0, 0);
    float3 visco = float3(0, 0, 0);
    for (uint N_ID = 0; N_ID < _NumParticles; N_ID++)
    {
        if (N_ID == P_ID)
            continue; // 自身を対象とした場合スキップ

        float3 N_position = _ParticlesBufferRead[N_ID].position;

        float3 diff = N_position - P_position;
        float r_sq = dot(diff, diff);

		// 半径内に収まっていない粒子は除外
        if (r_sq < h_sq)
        {
            float N_density = _ParticlesDensityBufferRead[N_ID].density;
            float N_pressure = _ParticlesPressureBufferRead[N_ID].pressure;
            float3 N_velocity = _ParticlesBufferRead[N_ID].velocity;
            float r = sqrt(r_sq);

			// 圧力項
            press += CalculateGradPressure(r, P_pressure, N_pressure, N_density, diff);

			// 粘性項
            visco += CalculateLapVelocity(r, P_velocity, N_velocity, N_density);
        }
    }

	// 統合
    float3 force = press + _Viscosity * visco;

	// 加速度バッファの更新
    _ParticlesForceBufferWrite[P_ID].acceleration = force / P_density;
}
