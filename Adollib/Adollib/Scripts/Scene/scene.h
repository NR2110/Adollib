#pragma once

#include "scene_list.h"

namespace Adollib {
	class Scene {

	public:
		virtual void awake() = 0;

		virtual void initialize() = 0;

		virtual void destroy() {};

	private:

	};
}