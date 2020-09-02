
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
struct Constraint
{
    float3 axis;        //拘束軸
    float jacDiagInv;   //拘束式の分母	
    float rhs;          //初期拘束力
    float tangent_rhs;  //拘束軸が法線の面上での拘束力
    float lowerlimit;   //拘束力の下限
    float upperlimit;   //拘束力の上限
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
    int contact_num;
	float friction;			  //摩擦力
	Contactpoint c_points[4]; //衝突点情報
};

struct Pair
{
	int solv_num[2]; //solverbody配列の何番目に情報が入っているか
	Contact contacts;
};

struct Solverbody
{
	float3 delta_LinearVelocity; // 並進速度差分
	float3 delta_AngulaVelocity; // 回転速度差分
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
	float3 delta_AngulaVelocity; // 回転速度差分
};


//::::::::::::::::::::::::::::::::::::::::::

//input_pair
struct CS_input_pair
{
	Pair pair;
};
//input_solverbody
struct CS_input_solvbody
{
	Solverbody solverbody;
};
//上記がそれぞれいくつあるのか
struct CS_input_max
{
	int pair_max;
};

struct CS_out
{
    Solverbody solverbody0;
    Solverbody solverbody1;
    float accuminpulse[4][3];
    
    float3 offset_delta_AngulaVelocity0;
    float3 offset_delta_LinearVelocity0;
  
    float3 offset_delta_AngulaVelocity1;
    float3 offset_delta_LinearVelocity1;
};

//:::::::::::::::::::::::::::::::::::::::::::::

void function(in float friction, in Contactpoint cp, inout Solverbody out_solvb0, inout Solverbody out_solvb1, out float ac_norm, out float ac_tan1, out float ac_tan2)
{
    ac_norm = 0;
    ac_tan1 = 0;
    ac_tan2 = 0;
    
    float3 rA = vector3_be_rotated_by_quat(cp.c_point[0], out_solvb0.orientation);
    float3 rB = vector3_be_rotated_by_quat(cp.c_point[1], out_solvb1.orientation);

	{
        Constraint constraint = cp.constraint[0];
        float delta_impulse = constraint.rhs;
        float3 delta_velocity[2];
        delta_velocity[0] = out_solvb0.delta_LinearVelocity + cross(out_solvb0.delta_AngulaVelocity, rA);
        delta_velocity[1] = out_solvb1.delta_LinearVelocity + cross(out_solvb1.delta_AngulaVelocity, rB);
        delta_impulse -= constraint.jacDiagInv * dot(constraint.axis, delta_velocity[0] - delta_velocity[1]);
        float old_impulse = constraint.accuminpulse;
        constraint.accuminpulse = old_impulse + delta_impulse > constraint.lowerlimit ? (old_impulse + delta_impulse < constraint.upperlimit ? old_impulse + delta_impulse : constraint.upperlimit) : constraint.lowerlimit;
        delta_impulse = constraint.accuminpulse - old_impulse;
        out_solvb0.delta_LinearVelocity += delta_impulse * out_solvb0.inv_mass * constraint.axis;
        out_solvb0.delta_AngulaVelocity += mul(float4(cross(rA, constraint.axis * delta_impulse), 0), (out_solvb0.inv_inertia)).xyz;
        out_solvb1.delta_LinearVelocity -= delta_impulse * out_solvb1.inv_mass * constraint.axis;
        out_solvb1.delta_AngulaVelocity -= mul(float4(cross(rB, constraint.axis * delta_impulse), 0), (out_solvb1.inv_inertia)).xyz;
       
        ac_norm = cp.constraint[0].accuminpulse;
    }
    
    float max_friction = friction * abs(ac_norm);
    cp.constraint[1].lowerlimit = -max_friction;
    cp.constraint[1].upperlimit = +max_friction;
    cp.constraint[2].lowerlimit = -max_friction;
    cp.constraint[2].upperlimit = +max_friction;

		{
        Constraint constraint = cp.constraint[1];
        float delta_impulse = constraint.rhs;
        float3 delta_velocity[2];
        delta_velocity[0] = out_solvb0.delta_LinearVelocity + cross(out_solvb0.delta_AngulaVelocity, rA);
        delta_velocity[1] = out_solvb1.delta_LinearVelocity + cross(out_solvb1.delta_AngulaVelocity, rB);
        delta_impulse -= constraint.jacDiagInv * dot(constraint.axis, delta_velocity[0] - delta_velocity[1]);
        float old_impulse = constraint.accuminpulse;
        constraint.accuminpulse = old_impulse + delta_impulse > constraint.lowerlimit ? (old_impulse + delta_impulse < constraint.upperlimit ? old_impulse + delta_impulse : constraint.upperlimit) : constraint.lowerlimit;
        delta_impulse = constraint.accuminpulse - old_impulse;
        out_solvb0.delta_LinearVelocity += delta_impulse * out_solvb0.inv_mass * constraint.axis;
        out_solvb0.delta_AngulaVelocity += mul(float4(cross(rA, constraint.axis * delta_impulse), 0), (out_solvb0.inv_inertia));
        out_solvb1.delta_LinearVelocity -= delta_impulse * out_solvb1.inv_mass * constraint.axis;
        out_solvb1.delta_AngulaVelocity -= mul(float4(cross(rB, constraint.axis * delta_impulse), 0), (out_solvb1.inv_inertia));
    }
	{
        Constraint constraint = cp.constraint[2];
        float delta_impulse = constraint.rhs;
        float3 delta_velocity[2];
        delta_velocity[0] = out_solvb0.delta_LinearVelocity + cross(out_solvb0.delta_AngulaVelocity, rA);
        delta_velocity[1] = out_solvb1.delta_LinearVelocity + cross(out_solvb1.delta_AngulaVelocity, rB);
        delta_impulse -= constraint.jacDiagInv * dot(constraint.axis, delta_velocity[0] - delta_velocity[1]);
        float old_impulse = constraint.accuminpulse;
        constraint.accuminpulse = old_impulse + delta_impulse > constraint.lowerlimit ? (old_impulse + delta_impulse < constraint.upperlimit ? old_impulse + delta_impulse : constraint.upperlimit) : constraint.lowerlimit;
        delta_impulse = constraint.accuminpulse - old_impulse;
        out_solvb0.delta_LinearVelocity += delta_impulse * out_solvb0.inv_mass * constraint.axis;
        out_solvb0.delta_AngulaVelocity += mul(float4(cross(rA, constraint.axis * delta_impulse), 0), out_solvb0.inv_inertia);
        out_solvb1.delta_LinearVelocity -= delta_impulse * out_solvb1.inv_mass * constraint.axis;
        out_solvb1.delta_AngulaVelocity -= mul(float4(cross(rB, constraint.axis * delta_impulse), 0), out_solvb1.inv_inertia);
    }

    ac_norm = cp.constraint[0].accuminpulse;
    ac_tan1 = cp.constraint[1].accuminpulse;
    ac_tan2 = cp.constraint[2].accuminpulse;
    


}

StructuredBuffer<CS_input_pair> cs_input_pair : register(t0); // 入力用
StructuredBuffer<CS_input_max>  cs_input_max   : register(t1); // 入力用
StructuredBuffer<CS_input_solvbody> cs_input_solve : register(t2); // 入力用

RWStructuredBuffer<CS_out> cs_out : register(u0); // 出力用

[numthreads(256, 1, 1)]
void main(const Input input)
{	
    uint index = input.dispatch.x;
      
    ///*
    if (index < cs_input_max[0].pair_max)
    {   
        
        //cs_out[].solverbodyはinoutとして使用するため　初めに入力の値を入れる
        cs_out[index].solverbody0 = cs_input_solve[cs_input_pair[index].pair.solv_num[0]].solverbody;
        cs_out[index].solverbody1 = cs_input_solve[cs_input_pair[index].pair.solv_num[1]].solverbody;
        
        for (int C_num = 0; C_num < cs_input_pair[index].pair.contacts.contact_num; C_num++)
        {
            //cs_out[index].accuminpulse[C_num][0] = cs_input_pair[index].pair.contacts.c_points[C_num].constraint[0].accuminpulse;
            //cs_out[index].accuminpulse[C_num][1] = cs_input_pair[index].pair.contacts.c_points[C_num].constraint[1].accuminpulse;
            //cs_out[index].accuminpulse[C_num][2] = cs_input_pair[index].pair.contacts.c_points[C_num].constraint[2].accuminpulse;
            
            
            function(
        cs_input_pair[index].pair.contacts.friction,
        cs_input_pair[index].pair.contacts.c_points[C_num],
        cs_out[index].solverbody0,
        cs_out[index].solverbody1,
        cs_out[index].accuminpulse[C_num][0],
        cs_out[index].accuminpulse[C_num][1],
        cs_out[index].accuminpulse[C_num][2]
             );
            
        //差分をoutputする
            cs_out[index].accuminpulse[C_num][0] -= cs_input_pair[index].pair.contacts.c_points[C_num].constraint[0].accuminpulse;
            cs_out[index].accuminpulse[C_num][1] -= cs_input_pair[index].pair.contacts.c_points[C_num].constraint[1].accuminpulse;
            cs_out[index].accuminpulse[C_num][2] -= cs_input_pair[index].pair.contacts.c_points[C_num].constraint[2].accuminpulse;
        }
       
  
    }  
    
            
     //差分をoutputする
    cs_out[index].offset_delta_LinearVelocity0 = cs_out[index].solverbody0.delta_LinearVelocity - cs_input_solve[cs_input_pair[index].pair.solv_num[0]].solverbody.delta_LinearVelocity;
    cs_out[index].offset_delta_AngulaVelocity0 = cs_out[index].solverbody0.delta_AngulaVelocity - cs_input_solve[cs_input_pair[index].pair.solv_num[0]].solverbody.delta_AngulaVelocity;
    cs_out[index].offset_delta_LinearVelocity1 = cs_out[index].solverbody1.delta_LinearVelocity - cs_input_solve[cs_input_pair[index].pair.solv_num[1]].solverbody.delta_LinearVelocity;
    cs_out[index].offset_delta_AngulaVelocity1 = cs_out[index].solverbody1.delta_AngulaVelocity - cs_input_solve[cs_input_pair[index].pair.solv_num[1]].solverbody.delta_AngulaVelocity;
     
  
}
