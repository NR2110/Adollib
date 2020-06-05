
#include "../Adollib/Adollib.h"
#include "../Adollib/gameobject_manager.h"

#include "enemy.h"

using namespace Adollib;

namespace Adollib
{
	// 所属するシーンの初期化時に一度だけ呼ばれる
	void Enemy::awake()
	{
		count = 0;
	}

	void Enemy::start()
	{
		count = 0;
		player = Gameobject_manager::find_camera("camera")->transform;
		move_pow = 0.02 * Al_Global::elapsed_time ;
		rot_pow = 0.1 * Al_Global::elapsed_time ;
	}

	// 毎フレーム呼ばれる更新処理
	void Enemy::update()
	{

		//sample_move1();
		sample_move2();
		//sample_move3();

		count -= Al_Global::elapsed_time;

	}
	// 毎フレーム、update()後に呼ばれる更新処理
	void Enemy::lateUpdate()
	{
	}

	// このスクリプトがアタッチされているGOのactiveSelfがtrueになった時呼ばれる
	void Enemy::onEnable()
	{

	}

	// このスクリプトがアタッチされているGOのactiveSelfがfalseになった時呼ばれる
	void Enemy::onDisable()
	{

	}

}

namespace Adollib {

	quaternion Enemy::look_q() {
		vector3 P = quaternion_look_at(gameobject->transform->local_pos, player->local_pos).euler();
		vector3 E = gameobject->transform->local_orient.euler();

		float di = P.y - E.y;
		float max_speed = 0.1;
		if (fabsf(di) > max_speed) {
			P.y = E.y + di / fabsf(di) * max_speed;
		}

		quaternion look_P = quaternion_from_euler(0, P.y, 0);
		quaternion E_look = quaternion_from_euler(0, E.y, 0);

		quaternion look = look_P - E_look.conjugate();

		return look;
	}

	void Enemy::sample_move1()
	{
#if 1
		switch (moving)
		{
		case Adollib::Enemy::Move_pattern::init:
			if (count <= 0)
				moving = Move_pattern::wait_init;
			break;

		case Adollib::Enemy::Move_pattern::choice:


			break;

		case Adollib::Enemy::Move_pattern::wait_init:
			count = 2;
			moving = Move_pattern::wait_update;
			break;

		case Adollib::Enemy::Move_pattern::wait_update:
			if (count <= 0)	moving = Move_pattern::strate_init;
			break;

		case Adollib::Enemy::Move_pattern::strate_init:
			count = 3;
			moving = Move_pattern::strate_update;
			break;

		case Adollib::Enemy::Move_pattern::strate_update:
			if (count <= 0)	moving = Move_pattern::tarn_init;
			gameobject->transform->local_pos +=
				vector3_be_rotated_by_quaternion(vector3(0, 0, 1), gameobject->transform->orientation) * move_pow;
			break;

		case Adollib::Enemy::Move_pattern::tarn_init:
			count = 5;
			moving = Move_pattern::tarn_update;
			break;

		case Adollib::Enemy::Move_pattern::tarn_update:
			if (count <= 0)	moving = Move_pattern::init;
			gameobject->transform->local_orient *=
				quaternion_from_euler(0, rot_pow, 0);

			gameobject->transform->local_pos +=
				vector3_be_rotated_by_quaternion(vector3(0, 0, 1), gameobject->transform->orientation) * move_pow;
			break;

		case Adollib::Enemy::Move_pattern::move_to_p_init:
			count = 3;
			break;

		case Adollib::Enemy::Move_pattern::move_tp_p_update:
			gameobject->transform->local_orient *=
				look_q();

			gameobject->transform->local_pos +=
				vector3_be_rotated_by_quaternion(vector3(0, 0, 1), gameobject->transform->orientation) * move_pow;
			break;

		case Adollib::Enemy::Move_pattern::Neet:
			moving = Move_pattern::init;
			break;

		default:
			break;
		}

#else
		///*
		switch (moving)
		{
		case Adollib::Enemy::Move_pattern::init:
			if(count <= 0)
			moving = Move_pattern::wait_init;
			break;

		case Adollib::Enemy::Move_pattern::choice:


			break;

		case Adollib::Enemy::Move_pattern::wait_init:
			count = 2;
			moving = Move_pattern::wait_update;
			break;

		case Adollib::Enemy::Move_pattern::wait_update:
			if (count <= 0)	moving = Move_pattern::strate_init;
			break;

		case Adollib::Enemy::Move_pattern::strate_init:
			count = 3;
			moving = Move_pattern::strate_update;
			break;

		case Adollib::Enemy::Move_pattern::strate_update:
			if (count <= 0)	moving = Move_pattern::tarn_init;
			collier->add_force(
				vector3_be_rotated_by_quaternion(vector3(0, 0, 1), gameobject->transform->orientation) * move_pow);
			break;

		case Adollib::Enemy::Move_pattern::tarn_init:
			count = 5;
			moving = Move_pattern::tarn_update;
			break;

		case Adollib::Enemy::Move_pattern::tarn_update:
			if (count <= 0)	moving = Move_pattern::init;
			collier->add_torque(
				vector3(0, 1, 0) * rot_pow);

			collier->add_force(
				vector3_be_rotated_by_quaternion(vector3(0, 0, 1), gameobject->transform->orientation) * move_pow);
			break;

		case Adollib::Enemy::Move_pattern::move_to_p_init:
			break;

		case Adollib::Enemy::Move_pattern::move_tp_p_update:
			break;

		case Adollib::Enemy::Move_pattern::Neet:
			count = 10;
			moving = Move_pattern::init;
			break;

		default:
			break;
		}
		//*/

#endif // 0
	}
	void Enemy::sample_move2()
	{
		int P = 0;
		switch (moving)
		{
		case Adollib::Enemy::Move_pattern::init:
			if(count <= 0)moving = Move_pattern::choice;
			break;

		case Adollib::Enemy::Move_pattern::choice:
			P = rand() & 100;
			if (P < 50)moving = Move_pattern::wait_init;
			else if (P < 75)moving = Move_pattern::wait_init;
			else if (P < 95)moving = Move_pattern::wait_init;
			else moving = Move_pattern::move_to_p_init;

			moving = Move_pattern::move_to_p_init;
			break;

		case Adollib::Enemy::Move_pattern::wait_init:
			count = 2;
			moving = Move_pattern::wait_update;
			break;

		case Adollib::Enemy::Move_pattern::wait_update:
			if (count <= 0)	moving = Move_pattern::choice;
			break;

		case Adollib::Enemy::Move_pattern::strate_init:
			count = 3;
			moving = Move_pattern::strate_update;
			break;

		case Adollib::Enemy::Move_pattern::strate_update:
			if (count <= 0)	moving = Move_pattern::choice;
			gameobject->transform->local_pos +=
				vector3_be_rotated_by_quaternion(vector3(0, 0, 1), gameobject->transform->orientation) * move_pow;
			break;

		case Adollib::Enemy::Move_pattern::tarn_init:
			count = 5;
			moving = Move_pattern::tarn_update;
			break;

		case Adollib::Enemy::Move_pattern::tarn_update:
			if (count <= 0)	moving = Move_pattern::choice;
			gameobject->transform->local_orient *= quaternion_from_euler(0, 1 * rot_pow, 0);
			gameobject->transform->local_pos +=
				vector3_be_rotated_by_quaternion(vector3(0, 0, 1), gameobject->transform->orientation) * move_pow;
			break;

		case Adollib::Enemy::Move_pattern::move_to_p_init:
			count = 3;
			moving = Move_pattern::move_tp_p_update;
			break;

		case Adollib::Enemy::Move_pattern::move_tp_p_update:
			if (count <= 0)	moving = Move_pattern::choice;
			gameobject->transform->local_orient *=
				look_q();

			//gameobject->transform->local_pos +=
			//	vector3_be_rotated_by_quaternion(vector3(0, 0, 1), gameobject->transform->orientation) * move_pow;
			break;

		case Adollib::Enemy::Move_pattern::Neet:
			count = 3;
			moving = Move_pattern::init;
			break;

		default:
			break;
		}

	}

}
