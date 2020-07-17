
#include "math.hlsli"

//::::::::::::::::
// CSInput structure
//:::::::::::::::
struct Input {
	uint3   groupThread  : SV_GroupThreadID;   //GPU�X���b�h�̃C���f�b�N�X(3����)
	uint3   group        : SV_GroupID;         //CPU�O���[�v�̃C���f�b�N�X(3����)
	uint3   dispatch     : SV_DispatchThreadID;//CPU*GPU�̃C���f�b�N�X(3����)
	uint    groupIndex   : SV_GroupIndex;      //SV_GroupThreadID��1�����ɂ�������
};

//:::::::::::::
// CSinput buffer
//:::::::::::::
struct Constraint{
	float3 axis;		//�S����
	float jacDiagInv;	//�S�����̕���	
	float rhs;			//�����S����
	float tangent_rhs;  //�S�������@���̖ʏ�ł̍S����
	float lowerlimit;	//�S���͂̉���
	float upperlimit;	//�S���͂̏��
	float accuminpulse; //�ݐς����S����
};

struct Contactpoint
{
	float distance; //�Փː[�x
	float3 c_point[2]; //�Փ˓_
	float3 normal; //�Փ˓_�̖@���x�N�g��
	Constraint constraint[3]; //�S��
};

struct Contact
{
    uint contact_num;
	float friction;			  //���C��
	Contactpoint c_points[4]; //�Փ˓_���
};

struct Pair
{
	uint solv_num[2]; //solverbody�z��̉��Ԗڂɏ�񂪓����Ă��邩
	Contact contacts;
};

struct Solverbody
{
	float3 delta_LinearVelocity; // ���i���x����
	float3 delta_AngularVelocity; // ��]���x����
	float4 orientation; // �p��
	float4x4 inv_inertia; // �����e���\���̋t�s��
	float  inv_mass; // ���ʂ̋t��
    int num;
};

//::::::::::::::::::::
// CSout buffer
//::::::::::::::::::::

struct Out_buf
{
	float3 delta_LinearVelocity;  // ���i���x����
	float3 delta_AngularVelocity; // ��]���x����
};


//::::::::::::::::::::::::::::::::::::::::::

//�Փˌv�Z�Ɏg�p����
struct CS_input_pair
{
	Pair pair;
};
//�v�Z���ʂ��ꎞ�I�ɕۑ�����
struct CS_input_solvbody
{
	Solverbody solverbody;
};
//��L�����ꂼ�ꂢ������̂�
struct CS_input_max
{
	uint pair_max;
};

struct CS_out
{
	Out_buf o_buf;
};


//:::::::::::::::::::::::::::::::::::::::::::::

void function(in Pair pair, inout Solverbody solverbody0, inout Solverbody solverbody1)
{
    for (int C_num = 0; C_num < pair.contacts.contact_num; C_num++)
    {
        
		//�Փ˓_�̏��
        Contactpoint cp = pair.contacts.c_points[C_num];
        float3 rA = vector3_be_rotated_by_quat(cp.c_point[0], solverbody0.orientation);
        float3 rB = vector3_be_rotated_by_quat(cp.c_point[1], solverbody1.orientation);

		{
            Constraint constraint = cp.constraint[0];
            float delta_impulse = constraint.rhs;
            float3 delta_velocity[2];
            delta_velocity[0] = solverbody0.delta_LinearVelocity + cross(solverbody0.delta_AngularVelocity, rA);
            delta_velocity[1] = solverbody1.delta_LinearVelocity + cross(solverbody1.delta_AngularVelocity, rB);
            delta_impulse -= constraint.jacDiagInv * dot(constraint.axis, delta_velocity[0] - delta_velocity[1]);
            float old_impulse = constraint.accuminpulse;
            constraint.accuminpulse = old_impulse + delta_impulse > constraint.lowerlimit ? (old_impulse + delta_impulse < constraint.upperlimit ? old_impulse + delta_impulse : constraint.upperlimit) : constraint.lowerlimit;
            delta_impulse = constraint.accuminpulse - old_impulse;
            solverbody0.delta_LinearVelocity += delta_impulse * solverbody0.inv_mass * constraint.axis;
            solverbody0.delta_AngularVelocity += mul(float4(cross(rA, constraint.axis * delta_impulse), 0), solverbody0.inv_inertia);
            solverbody1.delta_LinearVelocity -= delta_impulse * solverbody1.inv_mass * constraint.axis;
            solverbody1.delta_AngularVelocity -= mul(float4(cross(rB, constraint.axis * delta_impulse), 0), solverbody1.inv_inertia);
        }

        float max_friction = pair.contacts.friction * abs(cp.constraint[0].accuminpulse);
        cp.constraint[1].lowerlimit = -max_friction;
        cp.constraint[1].upperlimit = +max_friction;
        cp.constraint[2].lowerlimit = -max_friction;
        cp.constraint[2].upperlimit = +max_friction;

		{
            Constraint constraint = cp.constraint[1];
            float delta_impulse = constraint.rhs;
            float3 delta_velocity[2];
            delta_velocity[0] = solverbody0.delta_LinearVelocity + cross(solverbody0.delta_AngularVelocity, rA);
            delta_velocity[1] = solverbody1.delta_LinearVelocity + cross(solverbody1.delta_AngularVelocity, rB);
            delta_impulse -= constraint.jacDiagInv * dot(constraint.axis, delta_velocity[0] - delta_velocity[1]);
            float old_impulse = constraint.accuminpulse;
            constraint.accuminpulse = old_impulse + delta_impulse > constraint.lowerlimit ? (old_impulse + delta_impulse < constraint.upperlimit ? old_impulse + delta_impulse : constraint.upperlimit) : constraint.lowerlimit;
            delta_impulse = constraint.accuminpulse - old_impulse;
            solverbody0.delta_LinearVelocity += delta_impulse * solverbody0.inv_mass * constraint.axis;
            solverbody0.delta_AngularVelocity += mul(float4(cross(rA, constraint.axis * delta_impulse), 0), solverbody0.inv_inertia);
            solverbody1.delta_LinearVelocity -= delta_impulse * solverbody1.inv_mass * constraint.axis;
            solverbody1.delta_AngularVelocity -= mul(float4(cross(rB, constraint.axis * delta_impulse), 0), solverbody1.inv_inertia);
        }
		{
            Constraint constraint = cp.constraint[2];
            float delta_impulse = constraint.rhs;
            float3 delta_velocity[2];
            delta_velocity[0] = solverbody0.delta_LinearVelocity + cross(solverbody0.delta_AngularVelocity, rA);
            delta_velocity[1] = solverbody1.delta_LinearVelocity + cross(solverbody1.delta_AngularVelocity, rB);
            delta_impulse -= constraint.jacDiagInv * dot(constraint.axis, delta_velocity[0] - delta_velocity[1]);
            float old_impulse = constraint.accuminpulse;
            constraint.accuminpulse = old_impulse + delta_impulse > constraint.lowerlimit ? (old_impulse + delta_impulse < constraint.upperlimit ? old_impulse + delta_impulse : constraint.upperlimit) : constraint.lowerlimit;
            delta_impulse = constraint.accuminpulse - old_impulse;
            solverbody0.delta_LinearVelocity += delta_impulse * solverbody0.inv_mass * constraint.axis;
            solverbody0.delta_AngularVelocity += mul(float4(cross(rA, constraint.axis * delta_impulse), 0), solverbody0.inv_inertia);
            solverbody1.delta_LinearVelocity -= delta_impulse * solverbody1.inv_mass * constraint.axis;
            solverbody1.delta_AngularVelocity -= mul(float4(cross(rB, constraint.axis * delta_impulse), 0), solverbody1.inv_inertia);
        }
	
	
    }
}

StructuredBuffer<CS_input_pair> cs_input_pair : register(t0); // ���͗p
StructuredBuffer<CS_input_max> cs_input_max   : register(t1); // ���͗p
RWStructuredBuffer<CS_input_solvbody> cs_input_sb : register(u0); // ���o�͗p

[numthreads(1024, 1, 1)]
void main(const Input input)
{	
    uint index = input.groupThread.x;  

    function(cs_input_pair[index].pair, cs_input_sb[cs_input_pair[index].pair.solv_num[0]].solverbody, cs_input_sb[cs_input_pair[index].pair.solv_num[1]].solverbody);
    
    
}
