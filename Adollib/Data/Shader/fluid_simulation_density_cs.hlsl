#include "fluid_simulation.hlsli"

//--------------------------------------------------------------------------------------
// 密度の算出
//--------------------------------------------------------------------------------------
[numthreads(THREAD_SIZE_X, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    uint P_ID = DTid.x; // 現在処理しているパーティクルID

    float h_sq = _Smoothlen * _Smoothlen;
    float3 P_position = _ParticlesBufferRead[P_ID].position;

	// 近傍探索(O(n^2))
    float density = 0;
    for (uint N_ID = 0; N_ID < _NumParticles; N_ID++)
    {
        if (N_ID == P_ID)
            continue; // 自身を計算対象とした場合スキップ

        float3 N_position = _ParticlesBufferRead[N_ID].position;

        float3 diff = N_position - P_position; // 粒子距離
        float r_sq = dot(diff, diff); // 粒子距離の2乗

		// 半径内に収まっていない粒子は除外
        if (r_sq < h_sq)
        {
            density += CalculateDensity(r_sq); // わざわざルートをとる必要がない
        }
    }

	// 密度バッファを更新
    _ParticlesDensityBufferWrite[P_ID].density = density;
}