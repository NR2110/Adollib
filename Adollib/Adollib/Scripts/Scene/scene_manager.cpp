
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

//::: Scene��ύX����ۂ͂�����ύX ::::::::::::::::::::::::::::::::::::::::
void Scene_manager::initialize() {

	scenes[Scenelist::scene_game] = std::make_unique<Scene_game>();
	scenes[Scenelist::scene_title] = std::make_unique<Scene_title>();
	scenes[Scenelist::scene_player] = std::make_unique<Scene_player>();


	//set_active(Scenelist::scene_player);
	set_active(Scenelist::scene_title);
}
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

void Scene_manager::update() {

	// �폜�\���scene�̍폜
	for (auto& Sce : delete_scene_buffer) {

		auto itr = active_scenes.begin();
		for (auto& scene : active_scenes) {

			if (scene == Sce) {
				// �z�񂩂�폜
				active_scenes.erase(itr);

				//scene��gameobject���폜
				scenes[Sce]->destroy();
				Gameobject_manager::destroy(Sce);

				break;
			}
			++itr;
		}

		if (latest_activated_scene == Sce)latest_activated_scene = Scenelist::scene_null; //latest_activated_scene���폜���ꂽ��null�ɂ���
	}
	delete_scene_buffer.clear();

	// �ǉ��\���scene�̒ǉ�
	for (int i = 0; i < active_scene_buffer.size(); ++i) {
		auto Sce = active_scene_buffer[i];

		bool is_actived = false;
		for (auto& scene : active_scenes) {
			// ���ł�active�Ȃ�return
			if (scene == Sce) {
				is_actived = true;
				break;
			}
		}
		if (is_actived)continue;

		// active_scenes�ɕۑ�����
		active_scenes.emplace_back(Sce);

		auto scene_save = Sce;

		processing_scene = Sce; //���̒l�������l�Ƃ��ēn�����̂�����̂� �쐬�����V�[�����ꎞ�I�ɓ����
		// ������
		scenes[Sce]->initialize();

		processing_scene = scene_save;

	}
	for (auto& Sce : active_scene_buffer) {
		processing_scene = Sce; //���̒l�������l�Ƃ��ēn�����̂�����̂� �쐬�����V�[�����ꎞ�I�ɓ����
		Gameobject_manager::initialize(processing_scene); //scene��initialize���Ă�

	}
	processing_scene = Scenelist::scene_null;
	active_scene_buffer.clear();

	// physics_manager��update
	{
		Work_meter::start("adapt_transform_to_gameobject");
		Physics_function::Physics_manager::adapt_transform_to_gameobject();
		Work_meter::stop("adapt_transform_to_gameobject");

#ifdef UseImgui
		Physics_function::Physics_manager::update_Gui();
#endif

	}

	// activescene��update
	for (auto& scene : active_scenes) {
		processing_scene = scene;
		Gameobject_manager::update(scene);
	}
	processing_scene = Scenelist::scene_null;

	// physics_manager��late_update
	{
		Work_meter::start("copy_gameobject_transform");
		Physics_function::Physics_manager::copy_gameobject_transform();
		Work_meter::stop("copy_gameobject_transform");
	}


	// latest_activated_scene��null�Ȃ� ��ԍŌ��scenelist������
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

	// ���łɕۑ�����Ă����return
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
		// ���łɕۑ�����Ă����return
		if (scene == Sce)return;
	}
	delete_scene_buffer.emplace_back(Sce);

}



