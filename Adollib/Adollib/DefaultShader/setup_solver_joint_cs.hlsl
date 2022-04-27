
#include "defines.hlsli"

cbuffer CB : register(b0)
{
    float inverse_duration;
    //float3 dummy;
}

struct Joint_Read
{
    float3 position[2]; //6
    float3 joint_pos[2]; //6
    float3 relativeVelocity; //3
    float inv_mass[2]; //2
    row_major float3x3 inv_tensor[2]; //9
    float shrink_bias; // �S���̋����̒����l(�k) 1
    float stretch_bias; // �S���̋����̒����l(�L) 1
    float slop ; // �S���̋��e�덷 1
    float offset ; //�S���̏ꏊ�̂��� (ex. �z��anchor = 0,0 offset = ���_�̋��� �Ŏ������Ă���) 1
    float2 dummy2;
};

struct Joint_Write
{
    float3 axis; //�S����
    float jacDiagInv; //�S�����̕���
    float rhs; //�����S����
    float tangent_rhs; //�S�������@���̖ʏ�ł̍S����
    float lowerlimit; //�S���͂̉���
    float upperlimit; //�S���͂̏��
    float accuminpulse; //�ݐς����S����
    float3 dummy3;
};


StructuredBuffer<Joint_Read> _JointBufferRead : register(t0);
RWStructuredBuffer<Joint_Write> _JointBufferWrite : register(u0);

[numthreads(512, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    uint thread_ID = DTid.x; // ���ݏ������Ă���ID
#define JointRead (_JointBufferRead[thread_ID])
#define JointWrite (_JointBufferWrite[thread_ID])

    //Joint_Read P_position = _JointBufferRead[thread_ID];
    //Joint_Write P_density = _JointBufferWrite[thread_ID];

    float3 position[2];
				//anchor���ꂼ���world���W
    position[0] = JointRead.joint_pos[0] + JointRead.position[0];
    position[1] = JointRead.joint_pos[1] + JointRead.position[1];

    float3 direction = position[1] - position[0];
    float distance = length(direction);

    float rhs_pow = 0;
	{
        float dis = abs(distance - JointRead.offset);
        if (0.0f < dis - JointRead.slop)
        {
            int sign = (distance - JointRead.offset > 0) ? +1 : -1;

			// �S���_��Collider�̍��W�̍����̓��� �� offset���l������stretch��shrink�𔻒f����
            float bias = (dot(direction, JointRead.position[1] - JointRead.position[0]) * sign < 0) ? JointRead.stretch_bias : JointRead.shrink_bias;

            rhs_pow = 0.9f * bias * sign * (dis - JointRead.slop) * inverse_duration; // position error
        }
    }

    if (
		    abs(distance - JointRead.offset) < FLT_EPSILON || //offset���l�������l
		    rhs_pow == 0 //bias��0�̎��Ȃ�
		)
    {
					//�ƂĂ��߂��ʒu�ɂ��� -> ��������Ȃ�
        JointWrite.jacDiagInv = 0.0f;
        JointWrite.rhs = 0.0f;
        JointWrite.lowerlimit = -FLT_MAX;
        JointWrite.upperlimit = +FLT_MAX;
        JointWrite.axis = float3(0, 0, 0);
        return;
    }

    // �P�ʉ�
    direction = (direction / distance);

    float term1 = JointRead.inv_mass[0];
    float term2 = JointRead.inv_tensor[1];
    float3 tA, tB;

    float term3, term4, denominator;
    float3 axis;
    axis = direction;
    tA = cross(JointRead.joint_pos[0], axis);
    tB = cross(JointRead.joint_pos[1], axis);
    tA = mul(tA, JointRead.inv_tensor[0]);
    tB = mul(tB, JointRead.inv_tensor[1]);
    tA = cross(tA, JointRead.joint_pos[0]);
    tB = cross(tB, JointRead.joint_pos[1]);
    term3 = dot(axis, tA);
    term4 = dot(axis, tB);
    denominator = term1 + term2 + term3 + term4;


    JointWrite.jacDiagInv = 1.0f / denominator;

    JointWrite.rhs = -dot(JointRead.relativeVelocity, direction); // velocity error
    JointWrite.rhs += rhs_pow; //��Ōv�Z���Ă���
    JointWrite.rhs *= JointWrite.jacDiagInv;

    JointWrite.lowerlimit = -FLT_MAX;
    JointWrite.upperlimit = +FLT_MAX;
    JointWrite.axis = direction;
    JointWrite.accuminpulse = 0.0f;



}