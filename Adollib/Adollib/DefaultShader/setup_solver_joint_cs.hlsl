



//Vector3 position = Vector3(0, 0, 0);
//Quaternion orientation = Quaternion(1, 0, 0, 0);
//const Vector3& joint_posA = joint->anchor[i].posA;
//const Vector3& joint_posB = joint->anchor[i].posB;
//float shrink_bias = 0.1f; // 拘束の強さの調整値(縮)
//float stretch_bias = 0.1f; // 拘束の強さの調整値(伸)
//float slop = 0; // 拘束の許容誤差
//float offset = 0; //拘束の場所のずれ (ex. 布はanchor = 0,0 offset = 頂点の距離 で実装している)




//struct Constraint
//{
//    Vector3 axis; //拘束軸
//    float jacDiagInv = 0; //拘束式の分母
//    float rhs = 0; //初期拘束力
//    float tangent_rhs = 0; //拘束軸が法線の面上での拘束力
//    float lowerlimit = 0; //拘束力の下限
//    float upperlimit = 0; //拘束力の上限
//    float accuminpulse = 0; //累積される拘束力
//};

[numthreads(512, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
}