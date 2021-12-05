#include "input_changer.h"

#include "../Adollib/Scripts/Main/Adollib.h"

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
	is_jump_trigger = (pad_num == 0 && input->getKeyState(Key::Space)) || input->getPadState(pad_num, GamePad::A);

	// gunyatto
	is_gunyatto_state = (pad_num == 0 && input->getKeyState(Key::LeftControl)) || input->getPadState(pad_num, GamePad::X);

	// respown
	is_respown_trigger = (pad_num == 0 && input->getKeyTrigger(Key::R)) || input->getPadTrigger(pad_num, GamePad::START);

	// Žè‚ðŽ‚¿ã‚°‚é
	is_Rarm_state = (pad_num == 0 && input->getMouseState(Mouse::RBUTTON)) || input->getPadState(pad_num, GamePad::RSHOULDER);
	is_Larm_state = (pad_num == 0 && input->getMouseState(Mouse::LBUTTON)) || input->getPadState(pad_num, GamePad::LSHOULDER);


	//// cursol_move
	//if (pad_num == 0) {
	//	cursol_move.y = (input->getCursorPosX() - cursol_pos_save.x) * rotate_speed;
	//	cursol_move.x = (input->getCursorPosY() - cursol_pos_save.y) * rotate_speed;
	//}

	//cursol_move.y += input->getLStickX(pad_num);
	//cursol_move.x += input->getLStickY(pad_num);


	//// cursol‚ªlock‚³‚ê‚Ä‚¢‚½‚ç’†‰›‚Ö
	//// ˆá‚Á‚½‚ç‚¢‚¢Š´‚¶‚É
	//if (pad_num == 0 && !is_lock_cursol) {
	//	input->setCursorPos(Al_Global::SCREEN_WIDTH * 0.5f, Al_Global::SCREEN_HEIGHT * 0.5f);

	//	cursol_pos_save.x = (float)cursol_pos_save.x + cursol_move.y;
	//	cursol_pos_save.y = (float)cursol_pos_save.y + cursol_move.x;
	//}
	//else {
	//	cursol_pos_save = Vector2(Al_Global::SCREEN_WIDTH * 0.5f, Al_Global::SCREEN_HEIGHT * 0.5f);
	//}
}

void Input_changer::set_showcursol(bool is_show) {
	if (is_show_cursol == is_show)return;

	is_lock_cursol = is_show;

	// lock‚³‚ê‚Ä‚½‚Æ‚«‚Ícurso‚ðŒ©‚¦‚È‚¢‚æ‚¤‚É
	if (is_lock_cursol) ShowCursor(FALSE);
	else ShowCursor(TRUE);

	// lock‚³‚ê‚½‚Æ‚«‚Ícursol‚ð’†‰›‚ÉŽ‚Á‚Ä‚­‚é
	if (is_lock_cursol == true) {
		input->setCursorPos(Al_Global::SCREEN_WIDTH * 0.5f, Al_Global::SCREEN_HEIGHT * 0.5f);
		cursol_pos_save = Vector2(Al_Global::SCREEN_WIDTH * 0.5f, Al_Global::SCREEN_HEIGHT * 0.5f);
	}
}
