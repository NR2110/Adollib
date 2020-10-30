
#include "../Adollib/Scripts/Main/systems.h"
#include "../Adollib/Scripts/Main/Adollib.h"
#include "../Adollib/Scripts/Object/transform.h"
#include "../Adollib/Scripts/Object/gameobject_manager.h"
#include "camera_s.h"

namespace Adollib
{
	Vector3 eular_xy(Vector3 V) {

		//::: X-Yで表す(Zは0) ::::::::::::
		Vector3 ret;

		Vector3 Q = vector3_quatrotate(Vector3(0,0,1), quaternion_from_euler(V.x, V.y, V.z));

		ret.x = vector3_angle(Vector3(Q.x, 0, Q.z).unit_vect(), Vector3(0, 0, 1));
		if (vector3_cross(Vector3(Q.x, 0, Q.z), Vector3(0, 0, 1)).y < 0) ret.y *= -1;
		Q = vector3_quatrotate(Q, quaternion_angle_axis(-ret.y, Vector3(0, 1, 0)));

		ret.y = vector3_angle(Vector3(0, Q.y, Q.z).unit_vect(), Vector3(0, 0, 1));
		if (vector3_cross(Vector3(0, Q.y, Q.z), Vector3(0, 0, 1)).x < 0) ret.x *= -1;
		Q = vector3_quatrotate(Q, quaternion_angle_axis(-ret.x, Vector3(1, 0, 0)));


		return ret;
	}

	// 所属するシーンの初期化時に一度だけ呼ばれる
	void camera_s::awake()
	{

	}

	void camera_s::start()
	{
//		player = Gameobject_manager::find("player")->transform;
	}

	// 毎フレーム呼ばれる更新処理
	void camera_s::update()
	{
		//static int time = 0;
		//transform->position.x = sinf(ToRadian(time));
		//transform->position.y = cosf(ToRadian(time));
		//time++;

		Vector3 position = Vector3(0, 0, 0);
		Quaternion rotate = quaternion_identity();

		static bool set_carsol_stop = true;
		static bool set_c_player_pos = false;

		if (set_c_player_pos == false) {
			//カメラの回転
			if (set_carsol_stop == false) {
				float rotate_pow = 3 * Al_Global::second_per_frame;
				Vector3 rotate_vec = Vector3(0, 0, 0);
				rotate_vec.y = input->getCursorPosX() - Al_Global::SCREEN_WIDTH / 2;
				rotate_vec.x = input->getCursorPosY() - Al_Global::SCREEN_HEIGHT / 2;

				rotate *= quaternion_angle_axis(rotate_vec.y, Vector3(0, 1, 0));
				rotate *= quaternion_angle_axis(rotate_vec.x, vector3_cross(Vector3(0, 1, 0), vector3_quatrotate(Vector3(0, 0, 1), transform->local_orient)).unit_vect()) * rotate_pow;

				input->setCursorPos(Al_Global::SCREEN_WIDTH / 2, Al_Global::SCREEN_HEIGHT / 2);
			}
			else {
				float rotate_pow = 20 * Al_Global::second_per_frame;
				Vector3 rotate_vec = Vector3(0, 0, 0);
				if (input->getKeyState(Key::I))rotate_vec += Vector3(-1, 0, 0);
				if (input->getKeyState(Key::K))rotate_vec += Vector3(+1, 0, 0);
				if (input->getKeyState(Key::J))rotate_vec += Vector3(0, -1, 0);
				if (input->getKeyState(Key::L))rotate_vec += Vector3(0, +1, 0);

				rotate_vec *= rotate_pow;
				rotate *= quaternion_angle_axis(rotate_vec.y, Vector3(0, 1, 0));
				rotate *= quaternion_angle_axis(rotate_vec.x, vector3_cross(Vector3(0, 1, 0), vector3_quatrotate(Vector3(0, 0, 1), transform->local_orient)).unit_vect());

			}

			//カメラの移動
			{
				Vector3 move_vec = Vector3(0, 0, 0);
				float   move_pow = 10 * Al_Global::second_per_frame;
				if (input->getKeyState(Key::W))move_vec += Vector3(0, 0, +1);
				if (input->getKeyState(Key::S))move_vec += Vector3(0, 0, -1);
				if (input->getKeyState(Key::D))move_vec += Vector3(+1, 0, 0);
				if (input->getKeyState(Key::A))move_vec += Vector3(-1, 0, 0);

				//いらない回転を除く
				Vector3 eu = transform->local_orient.euler();
				Quaternion y_axis_rotate = quaternion_from_euler(0, eu.y, 0);
				position += vector3_quatrotate(move_vec, y_axis_rotate).unit_vect() * move_pow;

				//y軸方向の移動
				if (input->getKeyState(Key::Space))position += Vector3(0, 1, 0) * move_pow;
				if (input->getKeyState(Key::LeftShift))position += Vector3(0, -1, 0) * move_pow;
			}

			transform->local_pos += position;
			transform->local_orient *= rotate;
		}
		else {

			transform->local_orient = player->local_orient;
			transform->local_pos = player->local_pos + Vector3(0, 4, 0);
		}

		//if (input->getKeyTrigger(Key::P))
		//	set_carsol_stop == true ?
		//	(set_carsol_stop = false, input->setCursorPos(Al_Global::SCREEN_WIDTH / 2, Al_Global::SCREEN_HEIGHT / 2) ):
		//	(set_carsol_stop = true, input->setCursorPos(Al_Global::SCREEN_WIDTH / 2, Al_Global::SCREEN_HEIGHT / 2));
		//if (input->getKeyTrigger(Key::Q))
		//	set_c_player_pos == true ?
		//	set_c_player_pos = false :
		//	set_c_player_pos = true;

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
