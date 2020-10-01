
#include "../Adollib/Adollib.h"
#include "../Adollib/gameobject_manager.h"

#include "player_lead.h"

using namespace Adollib;

// 所属するシーンの初期化時に一度だけ呼ばれる
void Playerlead_s::awake() {
	collider = gameobject->add_collider<Sphere>();
	collider->tag = std::string("player");
	collider->No_hit_tag.push_back(std::string("player"));
	collider->inertial_mass = 1;
	collider->fall = false;

	pos = &gameobject->transform->local_pos;
	r = &gameobject->transform->local_scale.x;
}

void Playerlead_s::start()
{
	camera = Gameobject_manager::find_camera("camera");
}

// 毎フレーム呼ばれる更新処理
void Playerlead_s::update()
{
	gameobject->transform->local_scale = vector3(*r, *r, *r);
	collider->linear_velocity *= 0,00;

	vector3 dis;

	dis.x = +(input->getCursorPosX() - Al_Global::SCREEN_WIDTH / 2) * 0.209;
	dis.y = -(input->getCursorPosY() - Al_Global::SCREEN_HEIGHT / 2) * 0.209;

	vector3 moveZ = vector3_quatrotate(vector3(0, 0, 1), camera->get_world_orientate());
	vector3 moveY(0, 1, 0);
	vector3 moveX = vector3_cross(moveY, moveZ);

	vector3 cursor_pos = camera->get_world_position() + moveZ * 200 + moveX * dis.x + moveY * dis.y;

	//gに向かう力
	//if(input->getKeyState(Key::P))
	{
		vector3 n = (cursor_pos - *pos).unit_vect();

		float dis = fabsf((*pos - cursor_pos).norm_sqr()) - (*r - 2);

		float pow = fabsf(dis) * 800;
		if (pow > max_pow)
			pow = max_pow;

		collider->add_force((n * pow));
	}



}

// このスクリプトがアタッチされているGOのactiveSelfがtrueになった時呼ばれる
void Playerlead_s::onEnable()
{

}

// このスクリプトがアタッチされているGOのactiveSelfがfalseになった時呼ばれる
void Playerlead_s::onDisable()
{

}


