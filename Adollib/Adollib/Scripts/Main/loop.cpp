
#include "loop.h"
#include "../Imgui/imgui_manager.h"
#include "../Imgui/work_meter.h"

#include "../Physics/ALP__physics_manager.h"
#include "../Audio/audio_manager.h"
#include "../Renderer/renderer_manager.h"

#include "defines.h"


using namespace Adollib;

bool loop::init(HWND hWnd) {

	Renderer_manager::awake();
	Scene_manager::initialize();
	//Audio_manager::initialize();
	Gameobject_manager::awake();

	Systems::SetViewPort(Al_Global::SCREEN_WIDTH, Al_Global::SCREEN_HEIGHT);

#ifdef UseImgui
	Adollib::Imgui_manager::init(hWnd);
#endif

	return true;
}

bool loop::Update(MSG hMsg, HWND hWnd, int width, int height) {

	Systems::inputManager->update();

	Scene_manager::update();
	//Audio_manager::update();

	return true;
}

bool loop::Render(){

	Work_meter::start(std::string("loop_render"));
	Scene_manager::render();

	Work_meter::start("Imgui_render");
#ifdef UseImgui
	Adollib::Imgui_manager::render();
#endif
	Work_meter::stop("Imgui_render");
	Work_meter::stop(std::string("loop_render"));


	Systems::Flip();

	return true;
}

bool loop::destroy() {

	//Audio_manager::destroy();

	// ‚·‚×‚Ä‚Ìscene‚Ìgameobject‚ğíœ
	for (int i = 0; i < static_cast<int>(Scenelist::scene_list_size); i++) {
		Gameobject_manager::destroy(static_cast<Scenelist>(i));
	}

	// phtsics_manager‚Ìthread‚ğ~‚ß‚é
	Physics_function::Physics_manager::thread_stop_and_join();
	Physics_function::Physics_manager::destroy();


	// imgu.i‚ğ~‚ß‚é
#ifdef UseImgui
	Adollib::Imgui_manager::destroy();
#endif

	return true;
}