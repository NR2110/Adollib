#pragma once

#include "../../Scripts/Object/component.h"

namespace Adollib {

	class Physics_demo_stage : public Component {


	private:
		Gameobject* stage = nullptr;


	public:
		void awake() override;
		void update() override;

		void Update_hierarchy() override;

	};

}