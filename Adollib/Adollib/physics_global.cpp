#include "physics_global.h"
//:::::::::::::::::::
// 衝突判定で用いるconst変数の設定
//:::::::::::::::::::

using namespace Adollib;

float physics_g::timeStep = 0.016; //delta timeもどき とりあえずeplision_time
int physics_g::accuracy = 10; //衝突解決の繰り返し回数 1〜



