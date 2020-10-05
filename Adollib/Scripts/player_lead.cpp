
#include "../Adollib/Adollib.h"
#include "../Adollib/gameobject_manager.h"

#include "player_lead.h"

#include "stage_manager.h"
#include "../Adollib/lay_func.h"
#include "../Adollib/debug.h"
#include "../Adollib/physics_global.h"

using namespace Adollib;
using namespace Lay_func;

// 所属するシーンの初期化時に一度だけ呼ばれる
void Playerlead_s::awake() {
	collider = gameobject->add_collider<Sphere>();
	collider->tag = std::string("player");
	collider->No_hit_tag.push_back(std::string("player"));
	collider->inertial_mass = 1000;
	//collider->fall = false; 

	pos = &gameobject->transform->local_pos;
	r = &gameobject->transform->local_scale.x;

	float inv_lay_max = 1.0f / lays_size;
	for (int i = 0; i < lays_size; i++) {
		lay_dirs[i] = vector3_quatrotate(vector3(0, 1, 0), quaternion_from_euler(0, 0, inv_lay_max * i * 360));
	}

	for (int i = 0; i < lays_size; i++) {
		Debobb[i] = Gameobject_manager::createCube("");
		Debobb[i]->transform->local_scale = vector3(0.08, 5, 0.08);
		Debobb[i]->material->color = vector4(0, 1, 0, 1);
		Debobb[i]->active = false;
	}

	for (int i = 0; i < lays_size; i++) {
		Debsph[i] = Gameobject_manager::createSphere("");
		Debsph[i]->transform->local_scale = vector3(0.3, 0.3, 0.3);
		Debsph[i]->material->color = vector4(0, 0, 1, 1);
		Debsph[i]->active = false;
	}
}

void Playerlead_s::start()
{
	camera = Gameobject_manager::find_camera("camera");
	stages = &Gameobject_manager::find("stage_manager")->getComponent<Stage_manager_s>()->stages;
}

// 毎フレーム呼ばれる更新処理
void Playerlead_s::update()
{

	gameobject->transform->local_scale = vector3(*r, *r, *r);

	vector3 dis;
	dis.z = 100 - camera->transform->local_pos.z;
	dis.x = +(input->getCursorPosX() - Al_Global::SCREEN_WIDTH / 2) * dis.z * 0.0016;
	dis.y = -(input->getCursorPosY() - Al_Global::SCREEN_HEIGHT / 2) * dis.z * 0.0016;

	vector3 moveZ = vector3_quatrotate(vector3(0, 0, 1), camera->get_world_orientate());
	vector3 moveY(0, 1, 0);
	vector3 moveX = vector3_cross(moveY, moveZ);

	vector3 cursor_pos = camera->get_world_position() + moveZ * dis.z + moveX * dis.x + moveY * dis.y;

	vector3 to_cursor_vec = (cursor_pos - *pos).unit_vect();

	//playerからlayを飛ばしてstageと判定をとる
	Lay lay;
	lay.pos = *pos + to_cursor_vec * *r ;
	std::list<Stage_base*>::iterator sitr;
	std::list<Stage_base*>::iterator sitr_end = stages->end();

	vector3 sumpow = vector3(0, 0, 0);
	float laymin_dot = 0;

	// debug用
	for (int lay_num = 0; lay_num < lays_size; lay_num++) {
		Debobb[lay_num]->active = false;
		Debsph[lay_num]->active = false;
	}

	//layとstageの交差判定
	for (int lay_num = 0; lay_num < lays_size; lay_num++) {
		lay.dir = vector3_quatrotate(lay_dirs[lay_num], camera->get_world_orientate()).unit_vect();
		if (vector3_dot(lay.dir, to_cursor_vec) < 0) continue;
		if (lay.dir.x * to_cursor_vec.x < 0) continue;


		float laymin = FLT_MAX;
		bool Debugbool = false;
		for (sitr = stages->begin(); sitr != sitr_end; sitr++) {

			float min;
			if (intersectRayCollider(lay, (*sitr)->collider, min) == false) {
				if (intersectRayCollider(lay, (*sitr)->collider, min) == false)continue;
			};

			if (laymin > min)laymin = min;


			//各最低距離に届いていれば
			if (
				(((lay.dir.x * lay.dir.x) + (lay.dir.z * lay.dir.z)) * min * min < lay_max_tan* lay_max_tan) && //xz距離
				(fabsf(lay.dir.y * min) < lay_max_up) //y距離
				) {

				Debugbool = true;
			};
		}		

		if (Debugbool == false)continue;

		//一番遠くのlayを保存
		Debug::set("lay.dir", lay.dir.xyz);
		if (vector3_dot(lay.dir,to_cursor_vec) > laymin_dot) {
			laymin_dot = vector3_dot(lay.dir, to_cursor_vec);
			vector3 vec = lay.dir;
			if (vec.x * to_cursor_vec.x < 0)vec.x = 0;
			if (vec.y * to_cursor_vec.y < 0 || vec.y < 0)vec.y = 0;
			if (vec.z * to_cursor_vec.z < 0)vec.z = 0;
			sumpow = (vec * laymin);
		}

		//debug用
		Debobb[lay_num]->active = true;
		Debobb[lay_num]->transform->local_orient = quaternion_from_to_rotate(vector3(0, 1, 0), lay.dir);
		Debobb[lay_num]->transform->local_pos = lay.pos + lay.dir * 5;

		if (laymin > FLT_MAX - 1) {
			Debsph[lay_num]->active = false;
			continue;
		}
		else {
			Debsph[lay_num]->active = true;
			Debsph[lay_num]->transform->local_pos = lay.pos + lay.dir * laymin;
		}

	}

	if (input->getMouseState(Mouse::RBUTTON)) {
		*pos = cursor_pos;
		collider->linear_velocity *= 0;
	}

	//gに向かう力
	//if(input->getKeyState(Key::P))
	{
		vector3 n = (cursor_pos - *pos);
		if (input->getMouseState(Mouse::RBUTTON) == true) n = n.unit_vect();
		//else n = vector3(n.x, 0, n.z).unit_vect();

		float dis = fabsf((*pos - cursor_pos).norm_sqr()) - (*r - 2);

		float pow = fabsf(dis) * 1;
		if (pow > 1)
			pow = 1;

		collider->add_force((n * pow * collider->inertial_mass));
	}


	sumpow.y *= 1 ;
	collider->add_force(sumpow * 6 * collider->inertial_mass);
	collider->linear_velocity.x *= 0.98;
	collider->linear_velocity.z *= 0.98;
	if (sumpow.y > 0) collider->linear_acceleration += vector3(0, physics_g::gravity, 0) * 0.99;
	Debug::set("to_cursor_vec", to_cursor_vec.xyz);
	Debug::set("sumpow", sumpow.xyz);



}

// このスクリプトがアタッチされているGOのactiveSelfがtrueになった時呼ばれる
void Playerlead_s::onEnable()
{

}

// このスクリプトがアタッチされているGOのactiveSelfがfalseになった時呼ばれる
void Playerlead_s::onDisable()
{

}


