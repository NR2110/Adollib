
#include "title_state_manager.h"
#include "title_state_base.h"

using namespace Adollib;

void Title_state_manager::update() {


	if (now_state != next_state) {
		state_B = Title_state_B::Awake;
		now_state = next_state;
	}


	switch (now_state)
	{
	case Adollib::Title_state_A::Start:
		for (auto& base : state_bases) base->start_state(state_B);
		break;
	case Adollib::Title_state_A::Select_stage:
		for (auto& base : state_bases) base->select_state(state_B);
		break;
	default:
		break;
	}


}