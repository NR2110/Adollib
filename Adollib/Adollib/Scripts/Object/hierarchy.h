#pragma once

#include <vector>
#include "Object.h"
#include "transform.h"
#include "../Imgui/imgui_all.h"

namespace Adollib {
	namespace Hierarchy {
		void update_hierarchy(std::vector<Object*>& masters /*ˆê”Ôe‚Ìobject”z—ñ*/);
	}
}