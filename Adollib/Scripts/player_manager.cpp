
#include "../Adollib/Adollib.h"
#include "../Adollib/gameobject_manager.h"
#include "../Adollib/imgui_all.h"

#include "player_manager.h"

#include "player.h"
#include "player_lead.h"

using namespace Adollib;

	// 所属するシーンの初期化時に一度だけ呼ばれる
void Player_manager_s::awake() {

}

void Player_manager_s::start(){
	add_player_leader(
	vector3(0,10,0),
		1.5,
		10000,
		vector4(0,1,1,1)
	);
}

// 毎フレーム呼ばれる更新処理
void Player_manager_s::update() {


	ImGuiWindowFlags flag = 0;
	//flag |= ImGuiWindowFlags_AlwaysAutoResize;
	std::list<Player_base*>::iterator pitr = players.begin();
	std::list<Player_base*>::iterator pitr_end = players.end();


	if (ImGui::Begin("player_manage", 0, flag)) {
		pitr = players.begin();
		pitr_end = players.end();
		bool del = false;
		ImGui::Checkbox("delete", &del); //objectの削除

		if (del) {

			for (pitr = players.begin(); pitr != pitr_end; pitr++) {
				(*pitr)->gameobject->clear_collider();
				(*pitr)->gameobject->active = false;
			}
			players.clear();
		}

		bool add = false;
		ImGui::Checkbox("add_player", &add); //objectの削除

		if (add == true) {
			for (int i = 0; i < 10; i++) {
				float sps = (rand() % 141);
				sps *= sps * 0.0001;
				sps = 2 - sps;

				float pps = (rand() % 173);
				pps *= pps * 0.0001;
				pps = 3 - pps;
				pps *= pps;

				float aps = (rand() % 632) / 10.0f;
				aps *= aps * 0.0001f;
				aps = 40 - aps;

				add_player(
					vector3((rand() % 10) / 10.0f, (rand() % 10) / 10.0f + 20, 0), //pos
					//sps + 1.0f, //scale
					(rand() % 3) * 0.8f + 1,
					(rand() % 12) * 100 + 100, //maxpow(1200)
					//900,
					aps + 10 //accel(50)
					//50
				);
			}
		}

		ImGui::End();
	}

	//対象の設定
#if 1
	for (pitr = players.begin(); pitr != pitr_end; pitr++) {
		(*pitr)->springs.clear();
		(*pitr)->g_pos = player_leader->pos;
	}
	//他すべて
	pitr_end = players.end();
	std::list<Player_base*>::iterator pitr_save = players.begin();
	for (pitr = players.begin(); pitr != pitr_end; pitr++) {

		for (pitr_save = pitr; pitr_save != pitr_end; pitr_save++) {
			
			(*pitr_save)->springs.emplace_back(*pitr);
			(*pitr)->springs.emplace_back(*pitr_save);

		}
	}
#else
	for (pitr = players.begin(); pitr != pitr_end; pitr++) {
		(*pitr)->springs.clear();
		(*pitr)->collider->linear_velocity *= 0.96;
		(*pitr)->g_pos = player_leader->pos;
	}

	pitr_end = players.end();
	std::list<Player_base*>::iterator pitr_save;
	int count = 0;

	for (pitr = players.begin(); pitr != pitr_end; pitr++) {
		if (count == 0) {
			(*pitr)->g_pos = player_leader->pos;
			pitr_save = pitr;
		}
		else {
			(*pitr)->g_pos = (*pitr_save)->pos;
		}
		count++;
		if (count > 5)count = 0;
	}

	//他すべて
	for (pitr = players.begin(); pitr != pitr_end; pitr++) {

		for (pitr_save = pitr; pitr_save != pitr_end; pitr_save++) {

			(*pitr_save)->springs.emplace_back(*pitr);
			(*pitr)->springs.emplace_back(*pitr_save);

		}
	}
#endif
}

// このスクリプトがアタッチされているGOのactiveSelfがtrueになった時呼ばれる
void Player_manager_s::onEnable(){

}

// このスクリプトがアタッチされているGOのactiveSelfがfalseになった時呼ばれる
void Player_manager_s::onDisable() {

}

void Player_manager_s::add_player(vector3 pos, float r, float max_speed, float acc, vector4 color) {
	Gameobject* GO = Gameobject_manager::createSphere("");
	GO->material->color = color;
	GO->transform->local_pos = pos;
	GO->transform->local_scale = vector3(r, r, r);

	Player_s* p = GO->addComponent<Player_s>();
	p->max_pow = max_speed;
	p->accel = acc;

	players.emplace_back(p);
}

void Player_manager_s::add_player_leader(vector3 pos, float r, float max_speed, vector4 color) {
	Gameobject* GO = Gameobject_manager::createSphere("");
	GO->material->color = color;
	GO->transform->local_pos = pos;
	GO->transform->local_scale = vector3(r, r, r);

	Playerlead_s* p = GO->addComponent<Playerlead_s>();
	p->max_pow = max_speed;

	player_leader = p;
}

