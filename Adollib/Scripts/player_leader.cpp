
#include "../Adollib/Adollib.h"
#include "../Adollib/gameobject_manager.h"

#include "player_leader.h"

using namespace Adollib;

// ��������V�[���̏��������Ɉ�x�����Ă΂��
void Playerleader_s::awake() {
	Gameobject* object = nullptr;
	object = Gameobject_manager::createSphere("");
	vector4 C = vector4(1, 0, 0, 1);
	object->material->color = C;

	object->transform->local_pos = vector3((rand() % 10) / 10.0f, (rand() % 10) / 10.0f, 0);

	collider = object->add_collider<Sphere>();
	collider->tag = std::string("player");
	collider->No_hit_tag.push_back(std::string("player"));
	collider->inertial_mass = 1;
	collider->fall = false;

	pos = &object->transform->local_pos;
	r = &object->transform->local_scale.x;
}

void Playerleader_s::start()
{

}

// ���t���[���Ă΂��X�V����
void Playerleader_s::update()
{
	{
		vector3 bp = vector3(0, 10, 0);
		vector3 n = (bp - *pos).unit_vect();

		float dis = fabsf((*pos - bp).norm_sqr()) - (*r);

		collider->add_force((n * fabsf(dis)) * 10);
	}

}

// ���̃X�N���v�g���A�^�b�`����Ă���GO��activeSelf��true�ɂȂ������Ă΂��
void Playerleader_s::onEnable()
{

}

// ���̃X�N���v�g���A�^�b�`����Ă���GO��activeSelf��false�ɂȂ������Ă΂��
void Playerleader_s::onDisable()
{

}


