#pragma once

#include <vector>
#include "../Object/gameobject.h"
#include "../Object/transform.h"
#include "../Imgui/imgui_all.h"

namespace Adollib {
	namespace Inspector {
		void update_hierarchy(std::vector<Gameobject*>& masters /*��Ԑe��object�z��*/);
	}
}