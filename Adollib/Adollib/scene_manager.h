#pragma once

#include <memory>
#include <map>

#include "scene.h"
#include "scene_list.h"

namespace Adollib {

	class Scene_manager {

		std::map<Scenelist, std::unique_ptr<Scene>> scenes;

	public:
		void initialize();
		void update();
	//	void render();


	};
}