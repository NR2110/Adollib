#pragma once

#include "scene_list.h"

namespace Adollib {
	class Scene {

	public:

		virtual void initialize() = 0;

		virtual void destroy() {};

	private:

	};
}