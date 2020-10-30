#pragma once

#include "scene_list.h"

namespace Adollib {
	class Scene {

	private:

	public:
		static Scenelist now_scene;
		static Scenelist next_scene;

		virtual void initialize() = 0;
		//virtual void update() = 0;
		//virtual void render() = 0;
		//virtual void destroy() = 0;

	private:
		//Gameobject_manager go_manager;

	};
}