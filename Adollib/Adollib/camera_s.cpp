
#include "systems.h"
#include "transform.h"
#include "gameobject.h"
#include "camera_s.h"

namespace Adollib
{
	vector3 eular_xy(vector3 V) {

		//::: X-Yで表す(Zは0) ::::::::::::
		vector3 ret;

		vector3 Q = V;

		ret.x = vector3_angle(vector3(Q.x, 0, Q.z).unit_vect(), vector3(0, 0, 1));
		if (vector3_cross(vector3(Q.x, 0, Q.z), vector3(0, 0, 1)).y < 0) ret.y *= -1;
		Q = vector3_be_rotated_by_quaternion(Q, quaternion_angle_axis(-ret.y, vector3(0, 1, 0)));

		ret.y = vector3_angle(vector3(0, Q.y, Q.z).unit_vect(), vector3(0, 0, 1));
		if (vector3_cross(vector3(0, Q.y, Q.z), vector3(0, 0, 1)).x < 0) ret.x *= -1;
		Q = vector3_be_rotated_by_quaternion(Q, quaternion_angle_axis(-ret.x, vector3(1, 0, 0)));


		return ret;
	}

	// 所属するシーンの初期化時に一度だけ呼ばれる
	void camera_s::awake()
	{
		//input->setCursorPos(framework::SCREEN_WIDTH / 2, framework::SCREEN_HEIGHT / 2);
		rotate = vector3(0, 0, 0);
	}

	void camera_s::start()
	{
		rotate = vector3(0, 0, 0);
	}

	// 毎フレーム呼ばれる更新処理
	void camera_s::update()
	{
		//static int time = 0;
		//transform->position.x = sinf(ToRadian(time));
		//transform->position.y = cosf(ToRadian(time));
		//time++;

		static bool set_carsol_stop = true;
		if (input->getKeyTrigger(Key::P))set_carsol_stop == true ? set_carsol_stop = false : set_carsol_stop = true;

		//カメラの回転
		if (set_carsol_stop == false) {
			float rotate_pow = 0.8;
			float y_rot = input->getCursorPosX() - Systems::SCREEN_WIDTH / 2;
			float x_rot = input->getCursorPosY() - Systems::SCREEN_HEIGHT / 2;

			rotate.x += x_rot * rotate_pow;
			rotate.y += y_rot * rotate_pow;

			input->setCursorPos(Systems::SCREEN_WIDTH / 2, Systems::SCREEN_HEIGHT / 2);
		}
		else {
			vector3 rotate_vec = vector3(0, 0, 0);
			float rotate_pow = 1;
			if (input->getKeyState(Key::I))rotate_vec += vector3(0, -1, 0);
			if (input->getKeyState(Key::K))rotate_vec += vector3(0, +1, 0);
			if (input->getKeyState(Key::J))rotate_vec += vector3(-1, 0, 0);
			if (input->getKeyState(Key::L))rotate_vec += vector3(+1, 0, 0);

			rotate.x += rotate_vec.y * rotate_pow;
			rotate.y += rotate_vec.x * rotate_pow;

		}

		//カメラの移動
		{
			vector3 move_vec = vector3(0, 0, 0);
			float move_pow = 1 / Systems::elapsed_time;
			if (input->getKeyState(Key::W))
				move_vec += vector3(0, 0, +1);
			if (input->getKeyState(Key::S))move_vec += vector3(0, 0, -1);
			if (input->getKeyState(Key::D))move_vec += vector3(+1, 0, 0);
			if (input->getKeyState(Key::A))move_vec += vector3(-1, 0, 0);

			//いらない回転を除く
			quaternion x_rotate = quaternion_by_euler(0, rotate.y, 0);
			position += vector3_be_rotated_by_quaternion(move_vec, x_rotate).unit_vect() * move_pow;

			//y軸方向の移動
			if (input->getKeyState(Key::Space))position += vector3(0, 1, 0) * move_pow;
			if (input->getKeyState(Key::LeftShift))position += vector3(0, -1, 0) * move_pow;
		}

		transform->position = position;

		if (rotate.x >= 90)rotate.x = 89.9;
		if (rotate.x <= -90)rotate.x = -89.9;
		transform->prientation = quaternion_by_euler(rotate.x,rotate.y,0);
	}

	// 毎フレーム、update()後に呼ばれる更新処理
	void camera_s::lateUpdate()
	{
	}

	// このスクリプトがアタッチされているGOのactiveSelfがtrueになった時呼ばれる
	void camera_s::onEnable()
	{

	}

	// このスクリプトがアタッチされているGOのactiveSelfがfalseになった時呼ばれる
	void camera_s::onDisable()
	{

	}

}
