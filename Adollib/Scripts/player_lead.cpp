
#include "../Adollib/Adollib.h"
#include "../Adollib/gameobject_manager.h"

#include "player_lead.h"

using namespace Adollib;

// 所属するシーンの初期化時に一度だけ呼ばれる
void Playerlead_s::awake() {
	Gameobject* g =  Gameobject_manager::createSphere();
	g->add_collider<Sphere>();
	pos = &(g->transform->local_pos);
}

void Playerlead_s::start()
{
	camera = Gameobject_manager::find_camera("camera");
}

// 毎フレーム呼ばれる更新処理
void Playerlead_s::update()
{
	vector3 dis;

	dis.x = +(input->getCursorPosX() - Al_Global::SCREEN_WIDTH / 2) * 0.089;
	dis.y = -(input->getCursorPosY() - Al_Global::SCREEN_HEIGHT / 2) * 0.089;

	vector3 moveZ = vector3_quatrotate(vector3(0, 0, 1), camera->get_world_orientate());
	vector3 moveY(0, 1, 0);
	vector3 moveX = vector3_cross(moveY, moveZ);

	*pos = camera->get_world_position() + moveZ * 50 + moveX * dis.x + moveY * dis.y;
}

// このスクリプトがアタッチされているGOのactiveSelfがtrueになった時呼ばれる
void Playerlead_s::onEnable()
{

}

// このスクリプトがアタッチされているGOのactiveSelfがfalseになった時呼ばれる
void Playerlead_s::onDisable()
{

}


