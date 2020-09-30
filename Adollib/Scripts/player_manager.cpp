
#include "../Adollib/Adollib.h"
#include "../Adollib/gameobject_manager.h"
#include "../Adollib/imgui_all.h"

#include "player_manager.h"

#include "player.h"
#include "player_lead.h"

using namespace Adollib;

	// ��������V�[���̏��������Ɉ�x�����Ă΂��
void Player_manager_s::awake() {

}

void Player_manager_s::start(){
	Gameobject* GO = Gameobject_manager::create();
	Playerlead_s* p = GO->addComponent<Playerlead_s>();
	player_leader = p;
}

// ���t���[���Ă΂��X�V����
void Player_manager_s::update() {
	ImGuiWindowFlags flag = 0;
	//flag |= ImGuiWindowFlags_AlwaysAutoResize;
	std::list<Player_base*>::iterator pitr = players.begin();
	std::list<Player_base*>::iterator pitr_end = players.end();


	if (ImGui::Begin("player_manage", 0, flag)) {
		pitr = players.begin();
		pitr_end = players.end();
		bool del = false;
		ImGui::Checkbox("delete", &del); //object�̍폜

		if (del) {

			for (pitr = players.begin(); pitr != pitr_end; pitr++) {
				(*pitr)->gameobject->clear_collider();
				(*pitr)->gameobject->active = false;
			}
			players.clear();
		}

		bool add = false;
		ImGui::Checkbox("add_player", &add); //object�̍폜

		if (add == true) {
			Gameobject* GO = Gameobject_manager::create();
			Player_s* p = GO->addComponent<Player_s>();
			p->max_speed = rand() % 400 + 100;
			players.emplace_back(p);
		}

		ImGui::End();
	}

	//�Ώۂ̐ݒ�
#if 1
	for (pitr = players.begin(); pitr != pitr_end; pitr++) {
		(*pitr)->springs.clear();
		(*pitr)->collider->linear_velocity *= 0.96;
		(*pitr)->g_pos = player_leader->pos;
	}
	//�����ׂ�
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

	//�����ׂ�
	for (pitr = players.begin(); pitr != pitr_end; pitr++) {

		for (pitr_save = pitr; pitr_save != pitr_end; pitr_save++) {

			(*pitr_save)->springs.emplace_back(*pitr);
			(*pitr)->springs.emplace_back(*pitr_save);

		}
	}
#endif
}

// ���̃X�N���v�g���A�^�b�`����Ă���GO��activeSelf��true�ɂȂ������Ă΂��
void Player_manager_s::onEnable(){

}

// ���̃X�N���v�g���A�^�b�`����Ă���GO��activeSelf��false�ɂȂ������Ă΂��
void Player_manager_s::onDisable() {

}


