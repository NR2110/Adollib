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
		int select_player_num = 0; //user���I�����Ă���
		float change_timer_pow = 0; //change�̎��� 1/pow �����̎��Ԃ������ĕς��
		float change_select_timer = 0; //
		int  change_select_sign = 0; //���͂ɂ�� +1,-1�ɂȂ�
		float rotate_angle; //�̉�]�̊p�x

		static constexpr int select_playernum_material_size = 3; //player�̐�
		std::shared_ptr<Material>  select_stage_material[select_playernum_material_size]; //material

		Vector3 base_select_back_pos; //��{���W
		Gameobject* select_sprite;  //
		Gameobject* yajirushi;  //

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