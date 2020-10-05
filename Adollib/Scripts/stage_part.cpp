
#include "../Adollib/Adollib.h"
#include "../Adollib/gameobject_manager.h"

#include "stage_part.h"

using namespace Adollib;

// 所属するシーンの初期化時に一度だけ呼ばれる
void Stage_part_s::awake() {
	collider = gameobject->add_collider<Box>();
	collider->tag = std::string("stage");
	//collider->No_hit_tag.push_back(std::string("player"));
	collider->fall = false;
	collider->move = false;

	pos = gameobject->transform->local_pos;
	scale = gameobject->transform->local_scale;
	quat = gameobject->transform->local_orient;

}

void Stage_part_s::start()
{

}

// 毎フレーム呼ばれる更新処理
void Stage_part_s::update()
{
	gameobject->transform->local_orient = quat * *camera_orient;

	//quaternion Q = quaternion_from_to_rotate(vector3(0, 0, 1), (*camera_pos - pos).unit_vect());
	//gameobject->transform->local_orient = quat * Q;


}

// このスクリプトがアタッチされているGOのactiveSelfがtrueになった時呼ばれる
void Stage_part_s::onEnable()
{

}

// このスクリプトがアタッチされているGOのactiveSelfがfalseになった時呼ばれる
void Stage_part_s::onDisable()
{

}


