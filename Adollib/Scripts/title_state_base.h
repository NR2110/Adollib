#pragma once

#include "title_states.h"
namespace Adollib {

	class Title_state_base {

	public:
		//virtual void awake_state() = 0;
		//virtual void update_0_state() = 0;
		//virtual void update_1_state() = 0;
		//virtual void update_2_state() = 0;

		virtual void start_state(Title_state_B state);
		virtual void select_state(Title_state_B state);


	};


}