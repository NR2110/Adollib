



//Vector3 position = Vector3(0, 0, 0);
//Quaternion orientation = Quaternion(1, 0, 0, 0);
//const Vector3& joint_posA = joint->anchor[i].posA;
//const Vector3& joint_posB = joint->anchor[i].posB;
//float shrink_bias = 0.1f; // S‘©‚Ì‹­‚³‚Ì’²®’l(k)
//float stretch_bias = 0.1f; // S‘©‚Ì‹­‚³‚Ì’²®’l(L)
//float slop = 0; // S‘©‚Ì‹–—eŒë·
//float offset = 0; //S‘©‚ÌêŠ‚Ì‚¸‚ê (ex. •z‚Íanchor = 0,0 offset = ’¸“_‚Ì‹——£ ‚ÅÀ‘•‚µ‚Ä‚¢‚é)




//struct Constraint
//{
//    Vector3 axis; //S‘©²
//    float jacDiagInv = 0; //S‘©®‚Ì•ª•ê
//    float rhs = 0; //‰ŠúS‘©—Í
//    float tangent_rhs = 0; //S‘©²‚ª–@ü‚Ì–Êã‚Å‚ÌS‘©—Í
//    float lowerlimit = 0; //S‘©—Í‚Ì‰ºŒÀ
//    float upperlimit = 0; //S‘©—Í‚ÌãŒÀ
//    float accuminpulse = 0; //—İÏ‚³‚ê‚éS‘©—Í
//};

[numthreads(512, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
}