#include "input_changer.h"

#include "../Adollib/Scripts/Main/Adollib.h"
#include "../Adollib/Scripts/Imgui/debug.h"

using namespace Adollib;

void Input_changer::awake() {

}

void Input_changer::start() {

}

void Input_changer::update() {

	// Œü‚«
	dir = Vector3(0);

	// player”Ô†0”Ô‚ÍKeyboard‚Å‘€ì‚Å‚«‚é
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
	is_respown_trigger = (pad_num == 0 && input->getKeyTrigger(Key::R)) || input->getPadTrigger(pad_num, GamePad::START);

	// Žè‚ðŽ‚¿ã‚°‚é
	is_Rarm_state = (pad_num == 0 && input->getMouseState(Mouse::RBUTTON)) || input->getPadState(pad_num, GamePad::RSHOULDER);
	is_Larm_state = (pad_num == 0 && input->getMouseState(Mouse::LBUTTON)) || input->getPadState(pad_num, GamePad::LSHOULDER);

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
	if (pad_num == 0) {
		cursol_move.y = (input->getCursorPosX() - cursol_pos_save.x) * rotate_speed;
		cursol_move.x = (input->getCursorPosY() - cursol_pos_save.y) * rotate_speed;
	}

	Debug::set("cursol_move", cursol_move.x, cursol_move.y);

	cursol_move.y += input->getRStickX(pad_num);
	cursol_move.x -= input->getRStickY(pad_num);


	// cursol‚ªlock‚³‚ê‚Ä‚¢‚½‚ç’†‰›‚Ö
	// ˆá‚Á‚½‚ç‚¢‚¢Š´‚¶‚É
	if (pad_num == 0 && !is_lock_cursol_) {
		cursol_pos_save.x = (float)input->getCursorPosX();
		cursol_pos_save.y = (float)input->getCursorPosY();
	}
	else {
		input->setCursorPos(Al_Global::SCREEN_WIDTH * 0.5f, Al_Global::SCREEN_HEIGHT * 0.5f);
		cursol_pos_save = Vector2(Al_Global::SCREEN_WIDTH * 0.5f, Al_Global::SCREEN_HEIGHT * 0.5f);
	}
}

void Input_changer::set_lockcursol(bool is_show) {
	if (is_lock_cursol_ == is_show)return;

	is_lock_cursol_ = is_show;

	// lock‚³‚ê‚Ä‚½‚Æ‚«‚Ícurso‚ðŒ©‚¦‚È‚¢‚æ‚¤‚É
	if (is_lock_cursol_) ShowCursor(FALSE);
	else ShowCursor(TRUE);

	// lock‚³‚ê‚½‚Æ‚«‚Ícursol‚ð’†‰›‚ÉŽ‚Á‚Ä‚­‚é
	if (is_lock_cursol_ == true) {
		input->setCursorPos(Al_Global::SCREEN_WIDTH * 0.5f, Al_Global::SCREEN_HEIGHT * 0.5f);
		cursol_pos_save = Vector2(Al_Global::SCREEN_WIDTH * 0.5f, Al_Global::SCREEN_HEIGHT * 0.5f);
	}
}
