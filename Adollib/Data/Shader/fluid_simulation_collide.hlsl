#include "fluid_simulation.hlsli"

StructuredBuffer<Particle> _ParticlesBufferRead : register(t0);
StructuredBuffer<ParticleDensity> _ParticlesDensityBufferRead : register(t1);
RWStructuredBuffer<ParticleForces> _ParticlesForceBufferWrite : register(u0);
//--------------------------------------------------------------------------------------
// ���x�̎Z�o
//--------------------------------------------------------------------------------------
[numthreads(THREAD_SIZE_X, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    uint P_ID = DTid.x; // ���ݏ������Ă���p�[�e�B�N��ID

    float h_sq = _Smoothlen * 0.01;
    h_sq = h_sq * h_sq;
    float3 P_position = _ParticlesBufferRead[P_ID].position;
    float P_density = _ParticlesDensityBufferRead[P_ID].density;

	// �ߖT�T��(O(n^2))
    for (uint N_ID = 0; N_ID < _NumParticles; N_ID++)
    {
        if (N_ID == P_ID)
            continue; // ���g���v�Z�ΏۂƂ����ꍇ�X�L�b�v

        float3 N_position = _ParticlesBufferRead[N_ID].position;

        float3 diff = N_position - P_position; // ���q����
        float r_sq = dot(diff, diff); // ���q������2��

		// ���a���Ɏ��܂��Ă��Ȃ����q�͏��O
        if (r_sq < h_sq)
        {
      //      forceDT = (velocity_ix - Velocity[j * 3]) * (xij / distance)
						//+ (velocity_iy - Velocity[j * 3 + 1]) * (yij / distance)
						//+ (velocity_iz - Velocity[j * 3 + 2]) * (zij / distance);
      //      if (forceDT > 0.0)
      //      {
      //          mj = FluidDensity;
      //          forceDT *= (1.0 + e) * mi * mj / (mi + mj);
      //          velocity_ix -= (forceDT / mi) * (xij / distance);
      //          velocity_iy -= (forceDT / mi) * (yij / distance);
      //          velocity_iz -= (forceDT / mi) * (zij / distance);

            float3 normal = normalize(diff);

            float3 F = (_ParticlesBufferRead[P_ID].velocity - _ParticlesBufferRead[N_ID].velocity) * normal;
            float forceDT = F.x + F.y + F.z;
            if (forceDT > 0)
            {
                float N_density = _ParticlesDensityBufferRead[N_ID].density;
                forceDT *= (1.0 + 2.71828) * P_density * N_density / (P_density + N_density);
                _ParticlesForceBufferWrite[P_ID].acceleration -= (forceDT / P_density) * normal;
            }

        }
    }
}