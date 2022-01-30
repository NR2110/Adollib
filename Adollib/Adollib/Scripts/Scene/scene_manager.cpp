
#include "scene_manager.h"

#include "scene.h"
#include "scene_game.h"
#include "scene_title.h"
#include "scene_player.h"

#include "../Object/gameobject_manager.h"
#include "../Physics/ALP__physics_manager.h"
#include "../Imgui/work_meter.h"

using namespace Adollib;

std::map<Scenelist, std::unique_ptr<Scene>> Scene_manager::scenes;
std::list<Scenelist> Scene_manager::active_scenes;
std::vector<Scenelist> Scene_manager::delete_scene_buffer;
std::vector<Scenelist> Scene_manager::active_scene_buffer;
Scenelist Scene_manager::latest_activated_scene;
Scenelist Scene_manager::processing_scene;

//::: Sceneを変更する際はここを変更 ::::::::::::::::::::::::::::::::::::::::
void Scene_manager::initialize() {

	scenes[Scenelist::scene_game] = std::make_unique<Scene_game>();
	scenes[Scenelist::scene_title] = std::make_unique<Scene_title>();
	scenes[Scenelist::scene_player] = std::make_unique<Scene_player>();


	//set_active(Scenelist::scene_player);
	set_active(Scenelist::scene_title);
}
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

void Scene_manager::update() {

	// 削除予定のsceneの削除
	for (auto& Sce : delete_scene_buffer) {

		auto itr = active_scenes.begin();
		for (auto& scene : active_scenes) {

			if (scene == Sce) {
				// 配列から削除
				active_scenes.erase(itr);

				//sceneのgameobjectを削除
				scenes[Sce]->destroy();
				Gameobject_manager::destroy(Sce);

				break;
			}
			++itr;
		}

		if (latest_activated_scene == Sce)latest_activated_scene = Scenelist::scene_null; //latest_activated_sceneが削除されたらnullにする
	}
	delete_scene_buffer.clear();

	// 追加予定のsceneの追加
	for (int i = 0; i < active_scene_buffer.size(); ++i) {
		auto Sce = active_scene_buffer[i];

		bool is_actived = false;
		for (auto& scene : active_scenes) {
			// すでにactiveならreturn
			if (scene == Sce) {
				is_actived = true;
				break;
			}
		}
		if (is_actived)continue;

		// active_scenesに保存して
		active_scenes.emplace_back(Sce);

		auto scene_save = Sce;

		processing_scene = Sce; //この値を初期値として渡すものもあるので 作成されるシーンを一時的に入れる
		// 初期化
		scenes[Sce]->initialize();

		processing_scene = scene_save;

	}
	for (auto& Sce : active_scene_buffer) {
		processing_scene = Sce; //この値を初期値として渡すものもあるので 作成されるシーンを一時的に入れる
		Gameobject_manager::initialize(processing_scene); //sceneのinitializeを呼ぶ

	}
	processing_scene = Scenelist::scene_null;
	active_scene_buffer.clear();

	// physics_managerのupdate
	{
		Work_meter::start("adapt_transform_to_gameobject");
		Physics_function::Physics_manager::adapt_transform_to_gameobject();
		Work_meter::stop("adapt_transform_to_gameobject");

#ifdef UseImgui
		Physics_function::Physics_manager::update_Gui();
#endif

	}

	// activesceneのupdate
	for (auto& scene : active_scenes) {
		processing_scene = scene;
		Gameobject_manager::update(scene);
	}
	processing_scene = Scenelist::scene_null;

	// physics_managerのlate_update
	{
		Work_meter::start("copy_gameobject_transform");
		Physics_function::Physics_manager::copy_gameobject_transform();
		Work_meter::stop("copy_gameobject_transform");
	}


	// latest_activated_sceneがnullなら 一番最後のscenelistを入れる
	if (latest_activated_scene == Scenelist::scene_null) {
		if (active_scenes.size() != 0) {
			auto itr = active_scenes.end();
			--itr;
			latest_activated_scene = *itr;
		}
	}
}

void Scene_manager::render() {
	//for (const auto& scene : active_scenes) {
	//	processing_scene = scene;
	Gameobject_manager::render(active_scenes);
	//}
	//processing_scene = Scenelist::scene_null;
}

void Scene_manager::set_active(Scenelist Sce) {

	// すでに保存されていればreturn
	{
		for (auto& scene : active_scene_buffer) {
			if (scene == Sce)return;
		}
		for (auto& scene : active_scenes) {
			if (scene == Sce)return;
		}
	}

	auto save = processing_scene;
	processing_scene = Sce;
	scenes[Sce]->awake();
	processing_scene = save;

	active_scene_buffer.emplace_back(Sce);
}

void Scene_manager::set_inactive(Scenelist Sce) {

	for (auto& scene : delete_scene_buffer) {
		// すでに保存されていればreturn
		if (scene == Sce)return;
	}
	delete_scene_buffer.emplace_back(Sce);

}



