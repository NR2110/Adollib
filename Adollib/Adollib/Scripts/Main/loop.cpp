
#include "loop.h"
#include "../Imgui/imgui_manager.h"
#include "../Imgui/work_meter.h"

#include "../Physics/ALP__physics_manager.h"
using namespace Adollib;

bool loop::init(HWND hWnd) {

	Scene_manager::initialize();
	Gameobject_manager::awake();

	Systems::SetViewPort(Al_Global::SCREEN_WIDTH, Al_Global::SCREEN_HEIGHT);
	Adollib::Imgui_manager::init(hWnd);

	return true;
}

bool loop::Update(MSG hMsg, HWND hWnd, int width, int height) {

	Work_meter::start(std::string("loop_update"));
	Systems::inputManager->update();

	Scene_manager::update();
	//Gameobject_manager::update();

	Work_meter::stop(std::string("loop_update"));
	return true;
}

bool loop::Render(){

	Work_meter::start(std::string("loop_render"));
	Scene_manager::render();
	Work_meter::stop(std::string("loop_render"));

	Systems::Flip();

	return true;
}

bool loop::destroy() {

	// ‚·‚×‚Ä‚Ìscene‚Ìgameobject‚ğíœ
	for (int i = 0; i < static_cast<int>(Scenelist::scene_list_size); i++) {
		Gameobject_manager::destroy(static_cast<Scenelist>(i));
	}

	// phtsics_manager‚Ìthread‚ğ~‚ß‚é
	Physics_function::Physics_manager::thread_stop_and_join();
	Physics_function::Physics_manager::destroy();

	// imgu.i‚ğ~‚ß‚é
	Adollib::Imgui_manager::destroy();

	return true;
}