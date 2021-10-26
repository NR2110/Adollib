#pragma once

#include <memory>
#include <map>
#include <list>

#include "scene_list.h"

namespace Adollib {
	class Scene;

	class Scene_manager {
	private:
		// sceneの配列
		static std::map<Scenelist, std::unique_ptr<Scene>> scenes;

		// 有効になっているscene render、updateが行われる
		static std::list<Scenelist> active_scenes;

		static Scenelist now_scene;

	public:
		static Scenelist get_nowscene() { return now_scene; };

	public:
		static void initialize();
		static void update();
		static void render();

		static void set_active(Scenelist Sce);

		static void set_inactive(Scenelist Sce);

	};
}