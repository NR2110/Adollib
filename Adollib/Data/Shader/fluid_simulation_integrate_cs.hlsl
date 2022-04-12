#include "fluid_simulation.hlsli"

//--------------------------------------------------------------------------------------
// �Փ˔���Ȃǂ��s���āA�ʒu���X�V
//--------------------------------------------------------------------------------------
[numthreads(THREAD_SIZE_X, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    const unsigned int P_ID = DTid.x; // ���ݏ������Ă���p�[�e�B�N��ID

	// �X�V�O�̈ʒu�Ƒ��x
    float3 position = _ParticlesBufferRead[P_ID].position;
    float3 velocity = _ParticlesBufferRead[P_ID].velocity;
    float3 acceleration = _ParticlesForceBufferRead[P_ID].acceleration; // �v�Z���������x

	// �Փ˔���������Ȃ炱�� -----

	// �ǋ��E(�y�i���e�B�@)
    //float dist = dot(float3(position, 1), float3(1, 0, 0));
    //acceleration += min(dist, 0) * -_WallStiffness * float2(1, 0);

    //dist = dot(float3(position, 1), float3(0, 1, 0));
    //acceleration += min(dist, 0) * -_WallStiffness * float2(0, 1);

    //dist = dot(float3(position, 1), float3(-1, 0, _Range.x));
    //acceleration += min(dist, 0) * -_WallStiffness * float2(-1, 0);

    //dist = dot(float3(position, 1), float3(0, -1, _Range.y));
    //acceleration += min(dist, 0) * -_WallStiffness * float2(0, -1);

	// �d�͂̉��Z
    acceleration += _Gravity;

	// �O�i�I�C���[�@�Ŏ��̗��q�ʒu���X�V
    velocity += _TimeStep * acceleration;
    position += _TimeStep * velocity;

	// �p�[�e�B�N���̃o�b�t�@�X�V
    _ParticlesBufferWrite[P_ID].position = position;
    _ParticlesBufferWrite[P_ID].velocity = velocity;
}