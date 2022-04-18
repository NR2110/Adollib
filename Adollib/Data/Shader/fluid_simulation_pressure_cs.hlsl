#include "fluid_simulation.hlsli"

StructuredBuffer<ParticleDensity> _ParticlesDensityBufferRead : register(t0);

RWStructuredBuffer<ParticlePressure> _ParticlesPressureBufferWrite : register(u0);



//--------------------------------------------------------------------------------------
// ���͂̎Z�o
//--------------------------------------------------------------------------------------
[numthreads(THREAD_SIZE_X, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    uint P_ID = DTid.x; // ���ݏ������Ă���p�[�e�B�N��ID

    float P_density = _ParticlesDensityBufferRead[P_ID].density; // ����Z�o�������x
    float P_pressure = CalculatePressure(P_density); // ���͂̌v�Z

	// ���̓o�b�t�@���X�V
    _ParticlesPressureBufferWrite[P_ID].pressure = P_pressure;
}