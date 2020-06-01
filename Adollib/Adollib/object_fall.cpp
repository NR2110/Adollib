#include "gameobject_manager.h"

#include "object_fall.h"
#include "Adollib.h"

using namespace Adollib;

namespace Adollib
{
	// 所属するシーンの初期化時に一度だけ呼ばれる
	void object_fall::awake() {

	}

	void object_fall::start()
	{

	}

	// 毎フレーム呼ばれる更新処理
	void object_fall::update()
	{
		vector3 move_vec = vector3(0, 0, 0);
		float force = 10;
		if (input->getKeyState(Key::Up))move_vec += vector3(0, -1, 0);
		if (input->getKeyState(Key::Down))move_vec += vector3(0, +1, 0);
		if (input->getKeyState(Key::Left))move_vec += vector3(-1, 0, 0);
		if (input->getKeyState(Key::Right))move_vec += vector3(+1, 0, 0);

		float fall_pow = 0.89 * 3 * Al_Global::elapsed_time;
		//fall_pow = 0;
		vector3 fall = vector3(0, -1, 0) * fall_pow;

		collier->add_force(move_vec * force + fall);

		//collier->orientation = collier->orientation * quaternion_angle_axis(1, vector3(-1, 1, 0).unit_vect());

		//framework::debug->setString("angular_velocity : %f,%f,%f", collier->angular_velocity.x, collier->angular_velocity.y, collier->angular_velocity.z);
	}
	// 毎フレーム、update()後に呼ばれる更新処理
	void object_fall::lateUpdate()
	{


	}

	// このスクリプトがアタッチされているGOのactiveSelfがtrueになった時呼ばれる
	void object_fall::onEnable()
	{

	}

	// このスクリプトがアタッチされているGOのactiveSelfがfalseになった時呼ばれる
	void object_fall::onDisable()
	{

	}

}
