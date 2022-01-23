#pragma once

#include "title_states.h"
namespace Adollib {

	class Title_state_base {

	public:

	public:
		virtual void start_state(Title_state_B state) = 0;
		virtual void select_state(Title_state_B state) = 0;


	};


}