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
		int select_stage_num = 0; //userが選択しているstageのID
		float change_timer_pow = 0; //changeの時間 1/pow だけの時間をかけて変わる
		float change_select_timer = 0; //
		int  change_select_sign = 0; //入力により +1,-1になる
		int  change_select_state_sign = 0; //入力が続いていればパラパラ動くようにするためのフラグ 入力により +1,-1になる

		int select_player_num = 0; //playerの数 0->1人 1->2人 ...
		float change_playernum_timer = 0; //timer
		int  change_playernum_sign = 0; //入力により +1,-1になる

		static constexpr int select_stage_size = 2; //stageの数
		std::shared_ptr<Material>  select_stage_material[select_stage_size]; //stageの数だけ張り付けるためのmaterialを作る

		Vector3 base_select_back_pos[5]; //selectの基本座標 使いまわすため5個数でOK
		Gameobject* select_stage_back[5]; //spriteの後ろの板
		Gameobject* select_stage_front[5]; //spriteを張る板

		Gameobject* select_playernum_sprite = nullptr; //player人数表示GO
		Gameobject* yajirushi_go[4];  //
		float yajirushi_timer;

		Title_state_manager* title_state_manager = nullptr;
		Player_manager* player_manager = nullptr;
		bool is_next_state = false; // 次に行く入力が入ればtrueになる

	public:
		void start_state(Title_state_B state) override;

		void select_state(Title_state_B state) override;

	public:

		void awake() override;

		void start() override;

		void update() override;





	};

}