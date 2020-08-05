#pragma once

#include "systems.h"
#include "scene_manager.h"
#include "gameobject_manager.h"

namespace Adollib {

	class loop {
	public:
		bool init(HWND hWnd);
		bool Update(MSG hMsg, HWND hWnd, int width, int height);
		bool Render();
		bool destroy();

	private:
		Scene_manager scene_manager;
		Gameobject_manager go_manager;

	};

}