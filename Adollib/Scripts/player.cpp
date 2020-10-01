
#include "../Adollib/Adollib.h"
#include "../Adollib/gameobject_manager.h"

#include "player.h"

using namespace Adollib;

	// ��������V�[���̏��������Ɉ�x�����Ă΂��
void Player_s::awake() {
	collider = gameobject->add_collider<Sphere>();
	collider->tag = std::string("player");
	collider->No_hit_tag.push_back(std::string("player"));
	collider->inertial_mass = 1;
	collider->fall = false;

	pos = &gameobject->transform->local_pos;
	r = &gameobject->transform->local_scale.x;
}

void Player_s::start()
{

}

	// ���t���[���Ă΂��X�V����
void Player_s::update()
{
	gameobject->transform->local_scale = vector3(*r, *r, *r);

	//�{�[���ǂ������������
	for (auto sp : springs) {
		if (sp == nullptr)continue;
		vector3 n = (*sp->pos - *pos).unit_vect();

		float dis = fabsf((*pos - *sp->pos).norm_sqr()) - (*r + *sp->r);

		if(dis < 0)
			collider->add_force((-n  * (fabsf(dis) + 1)) * 500);	//�{�[���ǂ������������

		//else {
		//	float pow = fabsf(dis) * 1;
		//	if (pow > max_pow * 0.1f) pow = max_pow * 0.1f;

		//	collider->add_force((n * pow));	//�{�[���ǂ������������

		//}
	}


	//g�Ɍ�������
	{
		vector3 n = (*g_pos - *pos).unit_vect();

		float dis = fabsf((*pos - *g_pos).norm_sqr()) - (*r + 5);
		if (dis < 0)return;
		float pow = fabsf(dis) * accel;
		if (pow > max_pow) pow = max_pow;

		collider->add_force((n * pow));
	}

	collider->linear_velocity *= 0.96;
	

}

	// ���̃X�N���v�g���A�^�b�`����Ă���GO��activeSelf��true�ɂȂ������Ă΂��
void Player_s::onEnable()
{

}

	// ���̃X�N���v�g���A�^�b�`����Ă���GO��activeSelf��false�ɂȂ������Ă΂��
void Player_s::onDisable()
{

}


