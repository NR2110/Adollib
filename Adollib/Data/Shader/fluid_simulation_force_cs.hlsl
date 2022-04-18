#include "fluid_simulation.hlsli"

// ���q�ʒu�Ƒ��x��ێ�����o�b�t�@
StructuredBuffer<Particle> _ParticlesBufferRead : register(t0);
StructuredBuffer<ParticleDensity> _ParticlesDensityBufferRead : register(t1);
StructuredBuffer<ParticlePressure> _ParticlesPressureBufferRead : register(t2);

RWStructuredBuffer<ParticleForces> _ParticlesForceBufferWrite : register(u0);

//--------------------------------------------------------------------------------------
// ��(�����x)�̎Z�o
//--------------------------------------------------------------------------------------
[numthreads(THREAD_SIZE_X, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    uint P_ID = DTid.x; // ���ݏ������Ă���p�[�e�B�N��ID

    float3 P_position = _ParticlesBufferRead[P_ID].position;
    float3 P_velocity = _ParticlesBufferRead[P_ID].velocity;
    float P_density = _ParticlesDensityBufferRead[P_ID].density; // ����Z�o�������x
    float P_pressure = _ParticlesPressureBufferRead[P_ID].pressure; // ����Z�o��������

    const float h_sq = _Smoothlen * _Smoothlen;

	// �ߖT�T��(O(n^2))
    float3 press = float3(0, 0, 0);
    float3 visco = float3(0, 0, 0);
    for (uint N_ID = 0; N_ID < _NumParticles; N_ID++)
    {
        if (N_ID == P_ID)
            continue; // ���g��ΏۂƂ����ꍇ�X�L�b�v

        float3 N_position = _ParticlesBufferRead[N_ID].position;

        float3 diff = N_position - P_position;
        float r_sq = dot(diff, diff);

		// ���a���Ɏ��܂��Ă��Ȃ����q�͏��O
        if (r_sq < h_sq)
        {
            float N_density = _ParticlesDensityBufferRead[N_ID].density;
            float N_pressure = _ParticlesPressureBufferRead[N_ID].pressure;
            float3 N_velocity = _ParticlesBufferRead[N_ID].velocity;
            float r = sqrt(r_sq);

			// ���͍�
            press += CalculateGradPressure(r, P_pressure, N_pressure, N_density, diff);

			// �S����
            visco += CalculateLapVelocity(r, P_velocity, N_velocity, N_density);
        }
    }

	// ����
    float3 force = press + _Viscosity * visco;

	// �����x�o�b�t�@�̍X�V
    _ParticlesForceBufferWrite[P_ID].acceleration = force / P_density;
}
