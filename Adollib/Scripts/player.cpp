
#include "../Adollib/Adollib.h"
#include "../Adollib/gameobject_manager.h"

#include "player.h"

using namespace Adollib;

	// ��������V�[���̏��������Ɉ�x�����Ă΂��
void Player_s::awake() {
	Gameobject* object = nullptr;
	object = Gameobject_manager::createSphere("");
	vector4 C = vector4(1, 0, 0, 1);
	object->material->color = C;

	object->transform->local_pos = vector3((rand() % 10) / 10.0f, (rand() % 10) / 10.0f, 0);
	object->transform->local_scale = vector3(2,2,2);

	collider = object->add_collider<Sphere>();
	collider->tag = std::string("player");
	collider->No_hit_tag.push_back(std::string("player"));
	collider->inertial_mass = 1;
	collider->fall = false;

	pos = &object->transform->local_pos;
	r = &object->transform->local_scale.x;
}

void Player_s::start()
{

}

	// ���t���[���Ă΂��X�V����
void Player_s::update()
{

	//g�Ɍ�������
	{
		vector3 n = (*g_pos - *pos).unit_vect();

		float dis = fabsf((*pos - *g_pos).norm_sqr()) - (*r - 3);

		float pow = fabsf(dis) * 15;
		if (pow > max_speed) pow = max_speed;

		collider->add_force((n * pow));
	}
	//�{�[���ǂ������������
	for (auto sp : springs) {
		if (sp == nullptr)continue;
		vector3 n = (*sp->pos - *pos).unit_vect();

		float dis = fabsf((*pos - *sp->pos).norm_sqr()) - (*r + *sp->r);

		if(dis < 0)
			collider->add_force((-n  * fabsf(dis)) * 500);
	}

}

	// ���̃X�N���v�g���A�^�b�`����Ă���GO��activeSelf��true�ɂȂ������Ă΂��
void Player_s::onEnable()
{

}

	// ���̃X�N���v�g���A�^�b�`����Ă���GO��activeSelf��false�ɂȂ������Ă΂��
void Player_s::onDisable()
{

}


