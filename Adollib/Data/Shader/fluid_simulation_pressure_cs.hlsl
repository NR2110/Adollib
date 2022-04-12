#include "fluid_simulation.hlsli"

//--------------------------------------------------------------------------------------
// 圧力の算出
//--------------------------------------------------------------------------------------
[numthreads(THREAD_SIZE_X, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    uint P_ID = DTid.x; // 現在処理しているパーティクルID

    float P_density = _ParticlesDensityBufferRead[P_ID].density; // 先程算出した密度
    float P_pressure = CalculatePressure(P_density); // 圧力の計算

	// 圧力バッファを更新
    _ParticlesPressureBufferWrite[P_ID].pressure = P_pressure;
}