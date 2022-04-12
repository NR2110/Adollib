#include "fluid_simulation.hlsli"

//--------------------------------------------------------------------------------------
// ���x�̎Z�o
//--------------------------------------------------------------------------------------
[numthreads(THREAD_SIZE_X, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    uint P_ID = DTid.x; // ���ݏ������Ă���p�[�e�B�N��ID

    float h_sq = _Smoothlen * _Smoothlen;
    float3 P_position = _ParticlesBufferRead[P_ID].position;

	// �ߖT�T��(O(n^2))
    float density = 0;
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
            density += CalculateDensity(r_sq); // �킴�킴���[�g���Ƃ�K�v���Ȃ�
        }
    }

	// ���x�o�b�t�@���X�V
    _ParticlesDensityBufferWrite[P_ID].density = density;
}