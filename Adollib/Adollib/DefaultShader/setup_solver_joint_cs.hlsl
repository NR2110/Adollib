



//Vector3 position = Vector3(0, 0, 0);
//Quaternion orientation = Quaternion(1, 0, 0, 0);
//const Vector3& joint_posA = joint->anchor[i].posA;
//const Vector3& joint_posB = joint->anchor[i].posB;
//float shrink_bias = 0.1f; // �S���̋����̒����l(�k)
//float stretch_bias = 0.1f; // �S���̋����̒����l(�L)
//float slop = 0; // �S���̋��e�덷
//float offset = 0; //�S���̏ꏊ�̂��� (ex. �z��anchor = 0,0 offset = ���_�̋��� �Ŏ������Ă���)




//struct Constraint
//{
//    Vector3 axis; //�S����
//    float jacDiagInv = 0; //�S�����̕���
//    float rhs = 0; //�����S����
//    float tangent_rhs = 0; //�S�������@���̖ʏ�ł̍S����
//    float lowerlimit = 0; //�S���͂̉���
//    float upperlimit = 0; //�S���͂̏��
//    float accuminpulse = 0; //�ݐς����S����
//};

[numthreads(512, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
}