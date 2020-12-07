#pragma once

#include <vector>
#include "../Object/object.h"
#include "../Object/transform.h"
#include "../Imgui/imgui_all.h"

namespace Adollib {
	namespace Inspector {
		void update_hierarchy(std::vector<object*>& masters /*ˆê”Ôe‚Ìobject”z—ñ*/);
	}
}