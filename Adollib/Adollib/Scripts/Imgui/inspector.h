#pragma once

#include <vector>
#include "../Object/Object.h"
#include "../Object/transform.h"
#include "../Imgui/imgui_all.h"

namespace Adollib {
	namespace Inspector {
		void update_hierarchy(std::vector<Object*>& masters /*ˆê”Ôe‚Ìobject”z—ñ*/);
	}
}