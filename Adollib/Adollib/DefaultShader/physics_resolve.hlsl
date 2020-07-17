
#include "math.hlsli"

//::::::::::::::::
// CSInput structure
//:::::::::::::::
struct Input {
	uint3   groupThread  : SV_GroupThreadID;   //GPUスレッドのインデックス(3次元)
	uint3   group        : SV_GroupID;         //CPUグループのインデックス(3次元)
	uint3   dispatch     : SV_DispatchThreadID;//CPU*GPUのインデックス(3次元)
	uint    groupIndex   : SV_GroupIndex;      //SV_GroupThreadIDを1次元にしたもの
};

//:::::::::::::
// CSinput buffer
//:::::::::::::
struct Constraint{
	float3 axis;		//拘束軸
	float jacDiagInv;	//拘束式の分母	
	float rhs;			//初期拘束力
	float tangent_rhs;  //拘束軸が法線の面上での拘束力
	float lowerlimit;	//拘束力の下限
	float upperlimit;	//拘束力の上限
	float accuminpulse; //累積される拘束力
};

struct Contactpoint
{
	float distance; //衝突深度
	float3 c_point[2]; //衝突点
	float3 normal; //衝突点の法線ベクトル
	Constraint constraint[3]; //拘束
};

struct Contact
{
    uint contact_num;
	float friction;			  //摩擦力
	Contactpoint c_points[4]; //衝突点情報
};

struct Pair
{
	uint solv_num[2]; //solverbody配列の何番目に情報が入っているか
	Contact contacts;
};

struct Solverbody
{
	float3 delta_LinearVelocity; // 並進速度差分
	float3 delta_AngularVelocity; // 回転速度差分
	float4 orientation; // 姿勢
	float4x4 inv_inertia; // 慣性テンソルの逆行列
	float  inv_mass; // 質量の逆数
    int num;
};

//::::::::::::::::::::
// CSout buffer
//::::::::::::::::::::

struct Out_buf
{
	float3 delta_LinearVelocity;  // 並進速度差分
	float3 delta_AngularVelocity; // 回転速度差分
};


//::::::::::::::::::::::::::::::::::::::::::

//衝突計算に使用する
struct CS_input_pair
{
	Pair pair;
};
//計算結果を一時的に保存する
struct CS_input_solvbody
{
	Solverbody solverbody;
};
//上記がそれぞれいくつあるのか
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
        
		//衝突点の情報
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

StructuredBuffer<CS_input_pair> cs_input_pair : register(t0); // 入力用
StructuredBuffer<CS_input_max> cs_input_max   : register(t1); // 入力用
RWStructuredBuffer<CS_input_solvbody> cs_input_sb : register(u0); // 入出力用

[numthreads(1024, 1, 1)]
void main(const Input input)
{	
    uint index = input.groupThread.x;  

    function(cs_input_pair[index].pair, cs_input_sb[cs_input_pair[index].pair.solv_num[0]].solverbody, cs_input_sb[cs_input_pair[index].pair.solv_num[1]].solverbody);
    
    
}
