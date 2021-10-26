
#include "loop.h"
#include "../Imgui/imgui_manager.h"
#include "../Imgui/work_meter.h"

#include "../Physics/ALP__physics_manager.h"
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

	//loop::initで一度呼ばれているためコメント化
	//何かあれば適当に
	//Systems::SetViewPort(Systems::SCREEN_WIDTH, Systems::SCREEN_HEIGHT);

	Work_meter::start(std::string("render_all"));
	go_manager.render();
	Work_meter::stop(std::string("render_all"));

	Systems::Flip();

	return true;
}

bool loop::destroy() {

	for (int i = 0; i < static_cast<int>(Scenelist::scene_list_size); i++) {
		Gameobject_manager::destroy(static_cast<Scenelist>(i));
	}

	Physics_function::Physics_manager::thread_stop_and_join();
	Physics_function::Physics_manager::destroy();

	Adollib::Imgui_manager::destroy();

	return true;
}