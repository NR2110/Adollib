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

	class Select_playernum_manager : public Component, public Title_state_base {

	private:
		int select_player_num = 0; //userが選択している
		float change_timer_pow = 0; //changeの時間 1/pow だけの時間をかけて変わる
		float change_select_timer = 0; //
		int  change_select_sign = 0; //入力により +1,-1になる
		float rotate_angle; //板の回転の角度

		static constexpr int select_playernum_material_size = 3; //playerの数
		std::shared_ptr<Material>  select_stage_material[select_playernum_material_size]; //material

		Vector3 base_select_back_pos; //基本座標
		Gameobject* select_sprite;  //
		Gameobject* yajirushi;  //

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