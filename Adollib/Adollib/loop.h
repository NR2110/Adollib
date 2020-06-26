#pragma once

#include "systems.h"
#include "scene_manager.h"
#include "gameobject_manager.h"

namespace Adollib {

	class loop {
	public:
		void init();
		void Update(HWND hWnd, int width, int height);
		void Render();
		void destroy();

	private:
		Scene_manager scene_manager;
		Gameobject_manager go_manager;

	};

}