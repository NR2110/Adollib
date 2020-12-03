#pragma once

#include <vector>
#include "object.h"
#include "transform.h"
#include "../Imgui/imgui_all.h"

namespace Adollib {
	namespace Hierarchy {
		void update_hierarchy(std::vector<object*>& masters /*ˆê”Ôe‚Ìobject”z—ñ*/);
	}
}