
#include "loop.h"

using namespace Adollib;

void loop::init() {

	scene_manager.initialize();
	go_manager.awake();

	Systems::SetViewPort(Al_Global::SCREEN_WIDTH, Al_Global::SCREEN_HEIGHT);
}

void loop::Update(HWND hWnd, int width, int height) {

	Systems::inputManager->update();

	scene_manager.update();
	go_manager.update();
}

void loop::Render(){

	Systems::Clear();

	//loop::initで一度呼ばれているためコメント化
	//何かあれば適当に
	//Systems::SetViewPort(Systems::SCREEN_WIDTH, Systems::SCREEN_HEIGHT);

	go_manager.render();
	

	//Systems::Flip();

}

void loop::destroy() {
	for (int i = 0; i < static_cast<int>(Scenelist::scene_list_size); i++) {
		Gameobject_manager::destroy(static_cast<Scenelist>(i));
	}
}