#pragma once

#include <vector>
#include "gameobject.h"
#include "transform.h"
#include "../Imgui/imgui_all.h"

namespace Adollib {
	namespace Hierarchy {
		void update_hierarchy(std::vector<Gameobject*>& masters /*��Ԑe��object�z��*/);
	}
}