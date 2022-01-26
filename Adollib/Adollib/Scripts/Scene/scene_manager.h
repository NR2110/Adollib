#pragma once

#include <memory>
#include <map>
#include <list>
#include <vector>

#include "scene_list.h"

namespace Adollib {
	class Scene;

	class Scene_manager {
	private:
		// sceneの配列
		static std::map<Scenelist, std::unique_ptr<Scene>> scenes;

		// 有効になっているscene render、updateが行われる
		static std::list<Scenelist> active_scenes;

		// 削除される予定のscene
		static std::vector<Scenelist> delete_scene_buffer;
		static std::vector<Scenelist> active_scene_buffer;

		static Scenelist latest_activated_scene;
		static Scenelist processing_scene;

	public:
		static Scenelist get_latest_activated_scene() { return latest_activated_scene; };
		static Scenelist get_processing_scene() { return processing_scene; };
		static std::list<Scenelist> get_activescenes() { return active_scenes; };

	public:
		static void initialize();
		static void update();
		static void render();

		static void set_active(Scenelist Sce);

		static void set_inactive(Scenelist Sce);

	};
}