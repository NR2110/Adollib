
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

//::: Scene��ύX����ۂ͂�����ύX ::::::::::::::::::::::::::::::::::::::::
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
		// ���ł�active�Ȃ�return
		if (scene == Sce)return;
	}

	// active_scenes�ɕۑ�����
	active_scenes.emplace_back(Sce);

	auto scene_save = Sce;

	now_scene = Sce; //���̒l�������l�Ƃ��ēn�����̂�����̂� �쐬�����V�[�����ꎞ�I�ɓ����
	// ������
	scenes[Sce]->initialize();
	Gameobject_manager::initialize(now_scene); //��scene��initialize���Ă�

	now_scene = scene_save;
}

void Scene_manager::set_inactive(Scenelist Sce) {

	auto scene_save = Sce;
	now_scene = Sce;

	auto itr = active_scenes.begin();

	for (auto& scene : active_scenes) {

		if (scene == Sce) {
			// �z�񂩂�폜
			active_scenes.erase(itr);

			//scene��gameobject���폜
			Gameobject_manager::destroy(now_scene);

			break;
		}
		++itr;
	}

	now_scene = scene_save;
}



