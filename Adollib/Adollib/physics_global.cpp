#include "physics_global.h"
//:::::::::::::::::::
// 衝突判定で用いるconst変数の設定
//:::::::::::::::::::

using namespace Adollib;

float physics_g::timeStep = 0.016; //delta timeもどき とりあえずeplision_time
float physics_g::gravity = 8.9;
int physics_g::accuracy = 5; //衝突解決の繰り返し回数 1～
float physics_g::bias = 0.3; //貫通解決時に使用するばねの強さ
float physics_g::slop = 0.005f; //貫通の許容誤差



