#include "physics_global.h"
//:::::::::::::::::::
// �Փ˔���ŗp����const�ϐ��̐ݒ�
//:::::::::::::::::::

using namespace Adollib;

float physics_g::timeStep = 0.016; //delta time���ǂ� �Ƃ肠����eplision_time
float physics_g::gravity = 8.9;
int physics_g::accuracy = 5; //�Փˉ����̌J��Ԃ��� 1�`
float physics_g::bias = 0.3; //�ђʉ������Ɏg�p����΂˂̋���
float physics_g::slop = 0.005f; //�ђʂ̋��e�덷



