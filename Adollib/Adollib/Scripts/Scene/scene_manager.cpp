
#include "scene_manager.h"

#include "scene.h"
#include "scene_game.h"
#include "scene_title.h"

#include "../Object/gameobject_manager.h"
#include "../Imgui/work_meter.h"

using namespace Adollib;

std::map<Scenelist, std::unique_ptr<Scene>> Scene_manager::scenes;
std::list<Scenelist> Scene_manager::active_scenes;
Scenelist Scene_manager::now_scene;

//::: Sceneを変更する際はここを変更 ::::::::::::::::::::::::::::::::::::::::
void Scene_manager::initialize() {

	scenes[Scenelist::scene_game] = std::make_unique<scene_game>();
	scenes[Scenelist::scene_title] = std::make_unique<scene_title>();
}
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

void Scene_manager::update() {

	set_active(Scenelist::scene_game);

	for (auto& scene : active_scenes) {
		now_scene = scene;
		Gameobject_manager::update(now_scene);
	}

}

void Scene_manager::render() {
	for (const auto& scene : active_scenes) {
		Gameobject_manager::render(scene);
	}
}

void Scene_manager::set_active(Scenelist Sce) {

	for (auto& scene : active_scenes) {
		// すでにactiveならreturn
		if (scene == Sce)return;
	}

	// active_scenesに保存して
	active_scenes.emplace_back(Sce);

	auto scene_save = Sce;

	now_scene = Sce; //この値を初期値として渡すものもあるので 作成されるシーンを一時的に入れる
	// 初期化
	scenes[Sce]->initialize();
	Gameobject_manager::initialize(now_scene); //次sceneのinitializeを呼ぶ

	now_scene = scene_save;
}

void Scene_manager::set_inactive(Scenelist Sce) {

	auto scene_save = Sce;
	now_scene = Sce;

	auto itr = active_scenes.begin();

	for (auto& scene : active_scenes) {

		if (scene == Sce) {
			// 配列から削除
			active_scenes.erase(itr);

			//sceneのgameobjectを削除
			Gameobject_manager::destroy(now_scene);

			break;
		}
		++itr;
	}

	now_scene = scene_save;
}



