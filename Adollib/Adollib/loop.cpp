
#include "loop.h"
#include "camera.h"

using namespace Adollib;

void loop::init() {

	scene_manager.initialize();
	go_manager.awake();

}

void loop::Update(HWND hWnd, int width, int height) {

	//Adollib::InputManager::getInstance()->update(hwnd);

	scene_manager.update();
	go_manager.update();

}

void loop::Render(){

	Systems::SetViewPort(Systems::SCREEN_WIDTH, Systems::SCREEN_HEIGHT);

	Systems::Clear();

	go_manager.render();
	

	Systems::Flip();

}