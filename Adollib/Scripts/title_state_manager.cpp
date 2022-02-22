
#include "title_state_manager.h"
#include "title_state_base.h"

using namespace Adollib;


void Title_state_manager::awake() {
	now_stateA = Title_state_A::Select_playernum;
	now_stateB = Title_state_B::Awake;
}

void Title_state_manager::update() {

	state_A_timer += time->deltaTime();
	state_B_timer += time->deltaTime();

	if (now_stateB != next_stateB) {
		now_stateB = next_stateB;
		state_B_timer = 0;
	}

	if (now_stateA != next_stateA) {
		now_stateB = Title_state_B::Awake;
		now_stateA = next_stateA;

		state_A_timer = 0;
		state_B_timer = 0;
	}


	switch (now_stateA)
	{
	case Adollib::Title_state_A::Start:
		for (auto& base : state_bases) base->start_state(now_stateB);
		break;
	case Adollib::Title_state_A::Select_stage:
		for (auto& base : state_bases) base->select_state(now_stateB);
		break;
	default:
		break;
	}


	if (now_stateB == Title_state_B::Awake)next_stateB = Title_state_B::Update_0;


}