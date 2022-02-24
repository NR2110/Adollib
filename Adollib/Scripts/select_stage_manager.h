#pragma once

#include <vector>
#include <memory>

#include "../Adollib/Scripts/Object/gameobject.h"
#include "../Adollib/Scripts/Object/component.h"
#include "../Adollib/Scripts/Main/time.h"
#include "../Adollib/Scripts/Main/input.h"

#include "title_states.h"
#include "title_state_base.h"

namespace Adollib {

	class Collider;
	class Select_stage_base;
	class Material;
	class Title_state_manager;
	class Player_manager;

	class Select_state_manager : public Component, public Title_state_base {

	private:
		int select_stage_num = 0; //user���I�����Ă���stage��ID
		float change_timer_pow = 0; //change�̎��� 1/pow �����̎��Ԃ������ĕς��
		float change_select_timer = 0; //
		int  change_select_sign = 0; //���͂ɂ�� +1,-1�ɂȂ�
		int  change_select_state_sign = 0; //���͂������Ă���΃p���p�������悤�ɂ��邽�߂̃t���O ���͂ɂ�� +1,-1�ɂȂ�

		int select_player_num = 0; //player�̐� 0->1�l 1->2�l ...
		float change_playernum_timer = 0; //timer
		int  change_playernum_sign = 0; //���͂ɂ�� +1,-1�ɂȂ�

		static constexpr int select_stage_size = 2; //stage�̐�
		std::shared_ptr<Material>  select_stage_material[select_stage_size]; //stage�̐���������t���邽�߂�material�����

		Vector3 base_select_back_pos[5]; //select�̊�{���W �g���܂킷����5����OK
		Gameobject* select_stage_back[5]; //sprite�̌��̔�
		Gameobject* select_stage_front[5]; //sprite�𒣂��

		Gameobject* select_playernum_sprite = nullptr; //player�l���\��GO
		Gameobject* yajirushi_go[4];  //
		float yajirushi_timer;

		Title_state_manager* title_state_manager = nullptr;
		Player_manager* player_manager = nullptr;
		bool is_next_state = false; // ���ɍs�����͂������true�ɂȂ�

	public:
		void start_state(Title_state_B state) override;

		void select_state(Title_state_B state) override;

	public:

		void awake() override;

		void start() override;

		void update() override;





	};

}