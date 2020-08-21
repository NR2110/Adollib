
#include "loop.h"
#include "imgui_manager.h"
#include "work_meter.h"

using namespace Adollib;

bool loop::init(HWND hWnd) {

	scene_manager.initialize();
	go_manager.awake();

	Systems::SetViewPort(Al_Global::SCREEN_WIDTH, Al_Global::SCREEN_HEIGHT);
	Adollib::Imgui_manager::init(hWnd);

	return true;
}

bool loop::Update(MSG hMsg, HWND hWnd, int width, int height) {

	Work_meter::start(std::string("update_all"));
	Systems::inputManager->update();

	scene_manager.update();
	go_manager.update();

	Work_meter::stop(std::string("update_all"));
	return true;
}

bool loop::Render(){

	Systems::Clear();

	//loop::initで一度呼ばれているためコメント化
	//何かあれば適当に
	//Systems::SetViewPort(Systems::SCREEN_WIDTH, Systems::SCREEN_HEIGHT);

	Work_meter::start(std::string("render_all"));
	go_manager.render();
	Work_meter::stop(std::string("render_all"));

	Adollib::Imgui_manager::render();

	Systems::Flip();

	return true;
}

bool loop::destroy() {
	Adollib::Imgui_manager::destroy();
	for (int i = 0; i < static_cast<int>(Scenelist::scene_list_size); i++) {
		Gameobject_manager::destroy(static_cast<Scenelist>(i));
	}

	return true;
}