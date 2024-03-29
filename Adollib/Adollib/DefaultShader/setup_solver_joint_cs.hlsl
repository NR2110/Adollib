
#include "defines.hlsli"

cbuffer CB : register(b0)
{
    float inverse_duration;
}

struct Joint_Read
{
    float3 position[2];
    float3 joint_pos[2];
    float3 relativeVelocity;
    float inv_mass[2];
    row_major float3x3 inv_tensor[2];
    float shrink_bias; // 拘束の強さの調整値(縮)
    float stretch_bias; // 拘束の強さの調整値(伸)
    float slop ; // 拘束の許容誤差
    float offset ; //拘束の場所のずれ (ex. 布はanchor = 0,0 offset = 頂点の距離 で実装している)
};

struct Joint_Write
{
    float3 axis; //拘束軸
    float jacDiagInv; //拘束式の分母
    float rhs; //初期拘束力
    float tangent_rhs; //拘束軸が法線の面上での拘束力
    float lowerlimit; //拘束力の下限
    float upperlimit; //拘束力の上限
    float accuminpulse; //累積される拘束力
};


StructuredBuffer<Joint_Read> _JointBufferRead : register(t0);
RWStructuredBuffer<Joint_Write> _JointBufferWrite : register(u0);

[numthreads(512, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    uint thread_ID = DTid.x; // 現在処理しているID
#define JointRead (_JointBufferRead[thread_ID])
#define JointWrite (_JointBufferWrite[thread_ID])

    //Joint_Read P_position = _JointBufferRead[thread_ID];
    //Joint_Write P_density = _JointBufferWrite[thread_ID];

    float3 position[2];
				//anchorそれぞれのworld座標
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

			// 拘束点とColliderの座標の差分の内積 と offsetを考慮してstretchかshrinkを判断する
            float bias = (dot(direction, JointRead.position[1] - JointRead.position[0]) * sign < 0) ? JointRead.stretch_bias : JointRead.shrink_bias;

            rhs_pow = 0.9f * bias * sign * (dis - JointRead.slop) * inverse_duration; // position error
        }
    }

    if (
		    abs(distance - JointRead.offset) < FLT_EPSILON || //offsetを考慮した値
		    rhs_pow == 0 //biasが0の時など
		)
    {
					//とても近い位置にある -> 引っ張らない
        JointWrite.jacDiagInv = 0.0f;
        JointWrite.rhs = 0.0f;
        JointWrite.lowerlimit = -FLT_MAX;
        JointWrite.upperlimit = +FLT_MAX;
        JointWrite.axis = float3(0, 0, 0);
        return;
    }

    // 単位化
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
    JointWrite.rhs += rhs_pow; //上で計算している
    JointWrite.rhs *= JointWrite.jacDiagInv;

    JointWrite.lowerlimit = -FLT_MAX;
    JointWrite.upperlimit = +FLT_MAX;
    JointWrite.axis = direction;
    JointWrite.accuminpulse = 0.0f;



}