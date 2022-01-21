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
		Title_state_A now_state;
		Title_state_A next_state;

		Title_state_B state_B;

		std::vector<std::shared_ptr<Title_state_base>> state_bases;


	public:
		void set_nextstate(Title_state_A state) { next_state = state; };
		void add_nextstate(Title_state_A state) { next_state = state; };

		void update() override;





	};

}