#pragma once

#include <vector>
#include <memory>

#include "../Adollib/Scripts/Object/gameobject.h"
#include "../Adollib/Scripts/Object/component.h"
#include "../Adollib/Scripts/Main/time.h"
#include "../Adollib/Scripts/Main/input.h"

#include "title_states.h"

namespace Adollib {

	class Title_state_base;

	class Title_state_manager : public Component {

	private:
		Title_state_A now_stateA;
		Title_state_A next_stateA;

		Title_state_B now_stateB;
		Title_state_B next_stateB;

		std::vector<Title_state_base*> state_bases;

		float state_A_timer = 0;
		float state_B_timer = 0;

	public:
		void add_statebase(Title_state_base* statebase) { state_bases.emplace_back(statebase); };

		void set_nextstate_A(Title_state_A state) { next_stateA = state; };
		void set_nextstate_B(Title_state_B state) { next_stateB = state; };

		float get_timer_stateA() { return state_A_timer; };
		float get_timer_stateB() { return state_B_timer; };

	public:

		void update() override;





	};

}