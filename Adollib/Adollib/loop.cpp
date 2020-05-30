
#include "loop.h"

using namespace Adollib;

void loop::init() {

	scene_manager.initialize();
	go_manager.awake();

}

void loop::Update(HWND hWnd, int width, int height) {

	//Adollib::InputManager::getInstance()->update(hwnd);

	Systems::inputManager->update();

	scene_manager.update();
	go_manager.update();

}

void loop::Render(){

	Systems::SetViewPort(Al_Global::SCREEN_WIDTH, Al_Global::SCREEN_HEIGHT);

	Systems::Clear();

	//loop::init�ň�x�Ă΂�Ă��邽�߃R�����g��
	//��������ΓK����
	//Systems::SetViewPort(Systems::SCREEN_WIDTH, Systems::SCREEN_HEIGHT);

	go_manager.render();
	

	Systems::Flip();

}