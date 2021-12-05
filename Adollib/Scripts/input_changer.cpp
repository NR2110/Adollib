#include "input_changer.h"

using namespace Adollib;

void Input_changer::awake() {

}

void Input_changer::start() {

}

void Input_changer::update() {

	// 向き
	dir = Vector3(0);

	// player番号0番はKeyboardで操作できる
	if (pad_num == 0) {
		if (input->getKeyState(Key::W)) {
			dir += Vector3(0, 0, +1);
		}
		if (input->getKeyState(Key::S)) {
			dir += Vector3(0, 0, -1);
		}
		if (input->getKeyState(Key::A)) {
			dir += Vector3(-1, 0, 0);
		}
		if (input->getKeyState(Key::D)) {
			dir += Vector3(+1, 0, 0);
		}
	}

	dir.x += input->getLStickX(pad_num);
	dir.z += input->getLStickY(pad_num);

	dir = dir.unit_vect();


	// 手を持ち上げる
	is_Rarm_state = (pad_num == 0 && input->getMouseState(Mouse::RBUTTON)) || input->getPadState(pad_num, GamePad::RSHOULDER);
	is_Larm_state = (pad_num == 0 && input->getMouseState(Mouse::LBUTTON)) || input->getPadState(pad_num, GamePad::LSHOULDER);

	is_respown_trigger = (pad_num == 0 && input->getKeyTrigger(Key::R)) || input->getPadTrigger(pad_num, GamePad::START);

}

// このスクリプトがアタッチされているGOのactiveSelfがtrueになった時呼ばれる
void Input_changer::onEnable()
{

}

// このスクリプトがアタッチされているGOのactiveSelfがfalseになった時呼ばれる
void Input_changer::onDisable()
{

}
