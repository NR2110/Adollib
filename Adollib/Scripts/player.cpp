
#include "../Adollib/Adollib.h"
#include "../Adollib/gameobject_manager.h"

#include "player.h"

using namespace Adollib;

	// 所属するシーンの初期化時に一度だけ呼ばれる
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

	// 毎フレーム呼ばれる更新処理
void Player_s::update()
{
	gameobject->transform->local_scale = vector3(*r, *r, *r);

	//ボールどおし反発する力
	for (auto sp : springs) {
		if (sp == nullptr)continue;
		vector3 n = (*sp->pos - *pos).unit_vect();

		float dis = fabsf((*pos - *sp->pos).norm_sqr()) - (*r + *sp->r);

		if(dis < 0)
			collider->add_force((-n  * (fabsf(dis) + 1)) * 500);	//ボールどおし反発する力

		//else {
		//	float pow = fabsf(dis) * 1;
		//	if (pow > max_pow * 0.1f) pow = max_pow * 0.1f;

		//	collider->add_force((n * pow));	//ボールどおし引っ張る力

		//}
	}


	//gに向かう力
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

	// このスクリプトがアタッチされているGOのactiveSelfがtrueになった時呼ばれる
void Player_s::onEnable()
{

}

	// このスクリプトがアタッチされているGOのactiveSelfがfalseになった時呼ばれる
void Player_s::onDisable()
{

}


