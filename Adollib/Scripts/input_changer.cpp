#include "input_changer.h"

#include "../Adollib/Scripts/Imgui/imgui_all.h"
#include "../Adollib/Scripts/Main/Adollib.h"
#include "../Adollib/Scripts/Imgui/debug.h"

using namespace Adollib;

void Input_changer::awake() {

}

void Input_changer::start() {

}

void Input_changer::update() {

	static int y_sign = 1; //y反転させるか

	if (input->getKeyTrigger(Key::M))y_sign *= -1;

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

	// jump
	is_jump_trigger = (pad_num == 0 && input->getKeyTrigger(Key::Space)) || input->getPadTrigger(pad_num, GamePad::A);

	// gunyatto
	is_gunyatto_state = (pad_num == 0 && input->getKeyState(Key::LeftControl)) || input->getPadState(pad_num, GamePad::X);

	// respown
	is_respown_trigger = (pad_num == 0 && input->getKeyTrigger(Key::T)) || input->getPadTrigger(pad_num, GamePad::START);

	// 手を持ち上げる
	is_Larm_state = (pad_num == 0 && input->getMouseState(Mouse::LBUTTON)) || input->getPadState(pad_num, GamePad::LTRIGGER);
	is_Rarm_state = (pad_num == 0 && input->getMouseState(Mouse::RBUTTON)) || input->getPadState(pad_num, GamePad::RTRIGGER);

	// 縄を放つ
	is_rope_state = (pad_num == 0 && input->getKeyState(Key::R)) || input->getPadState(pad_num, GamePad::LSHOULDER);
	is_rope_releaced = (pad_num == 0 && input->getKeyReleased(Key::R)) || input->getPadReleased(pad_num, GamePad::LSHOULDER);
	is_rope_delete = (pad_num == 0 && input->getKeyTrigger(Key::F)) || input->getPadTrigger(pad_num, GamePad::B);
	is_rope_shrink = (pad_num == 0 && input->getKeyState(Key::Q)) || input->getPadState(pad_num, GamePad::RSHOULDER);

	// wheel
	wheel_move = 0;
	if (pad_num == 0) {
		wheel_move += input->getMouseWheel();
	}

	if (input->getPadState(pad_num, GamePad::UP))
		wheel_move += 1;
	if (input->getPadState(pad_num, GamePad::DOWN))
		wheel_move -= 1;

	// cursol_move
	cursol_move = Vector2(0);
#ifdef ON_DEBUG
	if (pad_num == 0) {
		cursol_move.y = (input->getCursorPosX() - cursol_pos_save.x) * rotate_speed;
		cursol_move.x = (input->getCursorPosY() - cursol_pos_save.y) * rotate_speed * y_sign;
	}
#endif

	Debug::set("cursol_move", cursol_move.x, cursol_move.y);

	cursol_move.y += input->getRStickX(pad_num) * time->deltaTime() * 130;
	cursol_move.x -= input->getRStickY(pad_num) * time->deltaTime() * 130 * y_sign;


	// cursolがlockされていたら中央へ
	// 違ったらいい感じに
	if (pad_num == 0 && !is_lock_cursol_) {
		cursol_pos_save.x = (float)input->getCursorPosX();
		cursol_pos_save.y = (float)input->getCursorPosY();
	}
	else if(pad_num == 0){
		input->setCursorPos(Al_Global::SCREEN_WIDTH * 0.5f, Al_Global::SCREEN_HEIGHT * 0.5f);
		cursol_pos_save = Vector2(Al_Global::SCREEN_WIDTH * 0.5f, Al_Global::SCREEN_HEIGHT * 0.5f);
	}
}

void Input_changer::set_lockcursol(bool is_show) {
	if (is_lock_cursol_ == is_show)return;

	is_lock_cursol_ = is_show;

	// lockされてたときはcursoを見えないように
	if (is_lock_cursol_) ShowCursor(FALSE);
	else ShowCursor(TRUE);

	// lockされたときはcursolを中央に持ってくる
	if (is_lock_cursol_ == true) {
		//input->setCursorPos(Al_Global::SCREEN_WIDTH * 0.5f, Al_Global::SCREEN_HEIGHT * 0.5f);
		///cursol_pos_save = Vector2(Al_Global::SCREEN_WIDTH * 0.5f, Al_Global::SCREEN_HEIGHT * 0.5f);
	}
}
