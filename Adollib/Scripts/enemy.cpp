
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
		player = Gameobject_manager::find("player")->transform.get();
		move_pow = 0.02 * Al_Global::second_per_frame ;
		rot_pow = 0.1 * Al_Global::second_per_frame ;
	}

	// 毎フレーム呼ばれる更新処理
	void Enemy::update()
	{
		switch (enemy_type)
		{
		case Adollib::Enemy::Enemy_type::pat_1:
			sample_move1(); //一定
			break;
		case Adollib::Enemy::Enemy_type::pat_2:
			sample_move2(); //追尾のみ
			break;
		case Adollib::Enemy::Enemy_type::pat_3:
			sample_move3(); //確率
			break;
		default:
			break;
		}

		count -= Al_Global::second_per_frame;

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

	quaternion Enemy::look_q(float max_speed) {
		vector3 P = quaternion_look_at(gameobject->transform->local_pos, player->local_pos).euler();
		vector3 E = gameobject->transform->local_orient.euler();

		float di = P.y - E.y;
		if (di > +180) di -= 360;
		if (di < -180) di += 360;

		if (fabsf(di) > max_speed) {
			P.y = E.y + di / fabsf(di) * max_speed;
		}

		quaternion look_P = quaternion_from_euler(0, P.y, 0);
		quaternion E_look = quaternion_from_euler(0, E.y, 0);

		quaternion look = look_P * E_look.conjugate();

		return look;
	}

	void Enemy::sample_move1()
	{
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
			if (count <= 0)	moving = Move_pattern::move_to_p_init;
			gameobject->transform->local_orient *=
				quaternion_from_euler(0, rot_pow, 0);

			gameobject->transform->local_pos +=
				vector3_be_rotated_by_quaternion(vector3(0, 0, 1), gameobject->transform->orientation) * move_pow;
			break;

		case Adollib::Enemy::Move_pattern::move_to_p_init:
			count = 3;
			moving = Move_pattern::move_tp_p_update;
			break;

		case Adollib::Enemy::Move_pattern::move_tp_p_update:
			if (count <= 0)moving = Move_pattern::wait_init;
			gameobject->transform->local_orient *=
				look_q(0.1);

			gameobject->transform->local_pos +=
				vector3_be_rotated_by_quaternion(vector3(0, 0, 1), gameobject->transform->orientation) * move_pow;
			break;

		case Adollib::Enemy::Move_pattern::Neet:
			moving = Move_pattern::init;
			break;

		default:
			break;
		}

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
			gameobject->transform->local_orient *=
				look_q(0.1);

			gameobject->transform->local_pos +=
				vector3_be_rotated_by_quaternion(vector3(0, 0, 1), gameobject->transform->orientation) * move_pow;
			break;

		case Adollib::Enemy::Move_pattern::Neet:
			count = 3;
			moving = Move_pattern::move_to_p_init;
			break;

		default:
			break;
		}

	}
	void Enemy::sample_move3()
	{
		int P = 0;
		switch (moving)
		{
		case Adollib::Enemy::Move_pattern::init:
			if (count <= 0)moving = Move_pattern::choice;
			break;

		case Adollib::Enemy::Move_pattern::choice:
			P = rand() & 100;
			if (P < 50)moving = Move_pattern::wait_init;
			else if (P < 75)moving = Move_pattern::wait_init;
			else if (P < 95)moving = Move_pattern::wait_init;
			else moving = Move_pattern::move_to_p_init;

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
				look_q(0.1);

			gameobject->transform->local_pos +=
				vector3_be_rotated_by_quaternion(vector3(0, 0, 1), gameobject->transform->orientation) * move_pow;
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
