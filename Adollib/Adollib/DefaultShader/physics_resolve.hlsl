
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
struct Constraint
{
    float3 axis;        //�S����
    float jacDiagInv;   //�S�����̕���	
    float rhs;          //�����S����
    float tangent_rhs;  //�S�������@���̖ʏ�ł̍S����
    float lowerlimit;   //�S���͂̉���
    float upperlimit;   //�S���͂̏��
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
    int contact_num;
	float friction;			  //���C��
	Contactpoint c_points[4]; //�Փ˓_���
};

struct Pair
{
	int solv_num[2]; //solverbody�z��̉��Ԗڂɏ�񂪓����Ă��邩
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
	int pair_max;
};

struct CS_out
{
    Solverbody solverbody0;
    Solverbody solverbody1;
};


//:::::::::::::::::::::::::::::::::::::::::::::

void function(in Pair pair, in Solverbody in_solvb0, in Solverbody in_solvb1, inout Solverbody out_solvb0, inout Solverbody out_solvb1)
{
    out_solvb0 = in_solvb0;
    out_solvb1 = in_solvb1;
    
    for (int C_num = 0; C_num < pair.contacts.contact_num; C_num++)
    {
        
		//�Փ˓_�̏��
        Contactpoint cp = pair.contacts.c_points[C_num];
        float3 rA = vector3_be_rotated_by_quat(cp.c_point[0], out_solvb0.orientation);
        float3 rB = vector3_be_rotated_by_quat(cp.c_point[1], out_solvb1.orientation);

		{
            Constraint constraint = cp.constraint[0];
            float delta_impulse = constraint.rhs;
            float3 delta_velocity[2];
            delta_velocity[0] = out_solvb0.delta_LinearVelocity + cross(out_solvb0.delta_AngularVelocity, rA);
            delta_velocity[1] = out_solvb1.delta_LinearVelocity + cross(out_solvb1.delta_AngularVelocity, rB);
            delta_impulse -= constraint.jacDiagInv * dot(constraint.axis, delta_velocity[0] - delta_velocity[1]);
            float old_impulse = constraint.accuminpulse;
            constraint.accuminpulse = old_impulse + delta_impulse > constraint.lowerlimit ? (old_impulse + delta_impulse < constraint.upperlimit ? old_impulse + delta_impulse : constraint.upperlimit) : constraint.lowerlimit;
            delta_impulse = constraint.accuminpulse - old_impulse;
            out_solvb0.delta_LinearVelocity += delta_impulse * out_solvb0.inv_mass * constraint.axis;
            out_solvb0.delta_AngularVelocity += mul(float4(cross(rA, constraint.axis * delta_impulse), 0), out_solvb0.inv_inertia);
            out_solvb1.delta_LinearVelocity -= delta_impulse * out_solvb1.inv_mass * constraint.axis;
            out_solvb1.delta_AngularVelocity -= mul(float4(cross(rB, constraint.axis * delta_impulse), 0), out_solvb1.inv_inertia);
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
            delta_velocity[0] = out_solvb0.delta_LinearVelocity + cross(out_solvb0.delta_AngularVelocity, rA);
            delta_velocity[1] = out_solvb1.delta_LinearVelocity + cross(out_solvb1.delta_AngularVelocity, rB);
            delta_impulse -= constraint.jacDiagInv * dot(constraint.axis, delta_velocity[0] - delta_velocity[1]);
            float old_impulse = constraint.accuminpulse;
            constraint.accuminpulse = old_impulse + delta_impulse > constraint.lowerlimit ? (old_impulse + delta_impulse < constraint.upperlimit ? old_impulse + delta_impulse : constraint.upperlimit) : constraint.lowerlimit;
            delta_impulse = constraint.accuminpulse - old_impulse;
            out_solvb0.delta_LinearVelocity += delta_impulse * out_solvb0.inv_mass * constraint.axis;
            out_solvb0.delta_AngularVelocity += mul(float4(cross(rA, constraint.axis * delta_impulse), 0), out_solvb0.inv_inertia);
            out_solvb1.delta_LinearVelocity -= delta_impulse * out_solvb1.inv_mass * constraint.axis;
            out_solvb1.delta_AngularVelocity -= mul(float4(cross(rB, constraint.axis * delta_impulse), 0), out_solvb1.inv_inertia);
        }
		{
            Constraint constraint = cp.constraint[2];
            float delta_impulse = constraint.rhs;
            float3 delta_velocity[2];
            delta_velocity[0] = out_solvb0.delta_LinearVelocity + cross(out_solvb0.delta_AngularVelocity, rA);
            delta_velocity[1] = out_solvb1.delta_LinearVelocity + cross(out_solvb1.delta_AngularVelocity, rB);
            delta_impulse -= constraint.jacDiagInv * dot(constraint.axis, delta_velocity[0] - delta_velocity[1]);
            float old_impulse = constraint.accuminpulse;
            constraint.accuminpulse = old_impulse + delta_impulse > constraint.lowerlimit ? (old_impulse + delta_impulse < constraint.upperlimit ? old_impulse + delta_impulse : constraint.upperlimit) : constraint.lowerlimit;
            delta_impulse = constraint.accuminpulse - old_impulse;
            out_solvb0.delta_LinearVelocity += delta_impulse * out_solvb0.inv_mass * constraint.axis;
            out_solvb0.delta_AngularVelocity += mul(float4(cross(rA, constraint.axis * delta_impulse), 0), out_solvb0.inv_inertia);
            out_solvb1.delta_LinearVelocity -= delta_impulse * out_solvb1.inv_mass * constraint.axis;
            out_solvb1.delta_AngularVelocity -= mul(float4(cross(rB, constraint.axis * delta_impulse), 0), out_solvb1.inv_inertia);
        }
	
	
    }
    
}

StructuredBuffer<CS_input_pair> cs_input_pair : register(t0); // ���͗p
StructuredBuffer<CS_input_max>  cs_input_max   : register(t1); // ���͗p
StructuredBuffer<CS_input_solvbody> cs_input_solve : register(t2); // ���͗p

RWStructuredBuffer<CS_out> cs_out : register(u0); // �o�͗p

[numthreads(256, 1, 1)]
void main(const Input input)
{	
    uint index = input.dispatch;
    
    if (index < cs_input_max[0].pair_max)
    {   
        //cs_out[index].solverbody0 = cs_input_solve[cs_input_pair[index].pair.solv_num[0]].solverbody;
        //cs_out[index].solverbody1 = cs_input_solve[cs_input_pair[index].pair.solv_num[1]].solverbody;
    
        function(
        cs_input_pair[index].pair, 
        cs_input_solve[cs_input_pair[index].pair.solv_num[0]].solverbody,
        cs_input_solve[cs_input_pair[index].pair.solv_num[1]].solverbody,       
        cs_out[index].solverbody0,
        cs_out[index].solverbody1
        );
       

    }
}
