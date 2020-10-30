
#include "../Adollib/Scripts/Main/Adollib.h"
#include "../Adollib/Scripts/Object/gameobject_manager.h"

#include "player.h"

namespace Adollib
{
	// 所属するシーンの初期化時に一度だけ呼ばれる
	void player::awake() {

	}

	void player::start()
	{

	}

	// 毎フレーム呼ばれる更新処理
	void player::update()
	{
		Vector3 move_vec = Vector3(0, 0, 0);
		float move_force = 0.1;
		if (input->getKeyState(Key::Down))move_vec += Vector3(0, 0, -1);
		if (input->getKeyState(Key::Up))move_vec += Vector3(0, 0, +1);
		collier->add_force(vector3_quatrotate(move_vec * move_force,gameobject->get_world_orientate()));

		Vector3 rot_vec = Vector3(0, 0, 0);
		float rot_force = 0.2;
		if (input->getKeyState(Key::Left))rot_vec += Vector3(0, -1, 0);
		if (input->getKeyState(Key::Right))rot_vec += Vector3(0, +1, 0);
		collier->add_torque(rot_vec * rot_force);
		//	collier->orientation * quaternion_angle_axis(1, vector3(-1, 1, 0).unit_vect());

		//framework::debug->setString("angular_velocity : %f,%f,%f", collier->angular_velocity.x, collier->angular_velocity.y, collier->angular_velocity.z);
	}

	// このスクリプトがアタッチされているGOのactiveSelfがtrueになった時呼ばれる
	void player::onEnable()
	{

	}

	// このスクリプトがアタッチされているGOのactiveSelfがfalseになった時呼ばれる
	void player::onDisable()
	{

	}

}
