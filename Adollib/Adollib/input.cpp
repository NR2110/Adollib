#include "input.h"


namespace Adollib
{
	// =====================================================
	// ������
	// =====================================================
	void MonoInput::initialize(HWND hwnd)
	{
		// �����o�ϐ��̏�����
		MAX_REPEAT_TIME = 0.5f;
		deadZone = DirectX::GamePad::DeadZone::DEAD_ZONE_INDEPENDENT_AXES;

		_hwnd = hwnd;
		time = Time::getInstancePtr();

		keyboard = std::make_unique<DirectX::Keyboard>();
		gamePad = std::make_unique<DirectX::GamePad>();
		mouse = std::make_unique<DirectX::Mouse>();
		kbTracker = std::make_unique<DirectX::Keyboard::KeyboardStateTracker>();
		for (int i = 0; i < GAMEPAD_NUM; ++i)
		{
			padTracker[i] = std::make_unique<DirectX::GamePad::ButtonStateTracker>();
		}
		mouseTracker = std::make_unique<DirectX::Mouse::ButtonStateTracker>();

		mouse->SetWindow(_hwnd);

		// �g���b�J�[���̊e�{�^���ւ̃|�C���^��ݒ�
		// GamePad
		for (int i = 0; i < GAMEPAD_NUM; ++i)
		{
			padButtonPtrs[i][static_cast<int>(GamePad::UP)/*0*/] = &padTracker[i].get()->dpadUp;				// Up
			padButtonPtrs[i][static_cast<int>(GamePad::DOWN)/*1*/] = &padTracker[i].get()->dpadDown;			// Down
			padButtonPtrs[i][static_cast<int>(GamePad::RIGHT)/*2*/] = &padTracker[i].get()->dpadRight;			// Right
			padButtonPtrs[i][static_cast<int>(GamePad::LEFT)/*3*/] = &padTracker[i].get()->dpadLeft;			// Left
			padButtonPtrs[i][static_cast<int>(GamePad::A)/*4*/] = &padTracker[i].get()->a;						// A
			padButtonPtrs[i][static_cast<int>(GamePad::B)/*5*/] = &padTracker[i].get()->b;						// B
			padButtonPtrs[i][static_cast<int>(GamePad::X)/*6*/] = &padTracker[i].get()->x;						// X
			padButtonPtrs[i][static_cast<int>(GamePad::Y)/*7*/] = &padTracker[i].get()->y;						// Y
			padButtonPtrs[i][static_cast<int>(GamePad::LSTICK)/*8*/] = &padTracker[i].get()->leftStick;		// LStick
			padButtonPtrs[i][static_cast<int>(GamePad::RSTICK)/*9*/] = &padTracker[i].get()->rightStick;		// RStick
			padButtonPtrs[i][static_cast<int>(GamePad::LSHOULDER)/*10*/] = &padTracker[i].get()->leftShoulder;	// LShoulder
			padButtonPtrs[i][static_cast<int>(GamePad::RSHOULDER)/*11*/] = &padTracker[i].get()->rightShoulder;// RShoulder
			padButtonPtrs[i][static_cast<int>(GamePad::BACK)/*12*/] = &padTracker[i].get()->back;				// Back
			padButtonPtrs[i][static_cast<int>(GamePad::START)/*13*/] = &padTracker[i].get()->start;			// Start
			// Mouse
			mouseButtonPtrs[static_cast<int>(Mouse::LBUTTON)/*0*/] = &mouseTracker.get()->leftButton;	// LButton
			mouseButtonPtrs[static_cast<int>(Mouse::MBUTTON)/*1*/] = &mouseTracker.get()->middleButton;	// MButton
			mouseButtonPtrs[static_cast<int>(Mouse::RBUTTON)/*2*/] = &mouseTracker.get()->rightButton;	// RButton
			mouseButtonPtrs[static_cast<int>(Mouse::X1BUTTON)/*3*/] = &mouseTracker.get()->xButton1;		// XButton1
			mouseButtonPtrs[static_cast<int>(Mouse::X2BUTTON)/*4*/] = &mouseTracker.get()->xButton2;		// XButton2

		}
	}

	// =====================================================
	// �X�V����
	// =====================================================
	void MonoInput::update()
	{
		// ***********************************************
		// �L�[�{�[�h�A�}�E�X�̍X�V
		// ***********************************************
		// ���̓f�o�C�X�̌���Ԃ��擾(keyboard, mouse)
		kbState = keyboard.get()->GetState();
		mouseState = mouse.get()->GetState();

		// �g���b�J�[�̍X�V(keyboard, mouse)
		kbTracker.get()->Update(kbState);
		mouseTracker.get()->Update(mouseState);

		// �L�[�{�[�h�̓��͏�Ԃ��擾
		{
			for (int i = 0; i < 256; ++i)
			{
				// �Q�Ɛ�ւ̃|�C���^�Ƃ��Ēu���Ă���
				auto r = &kbRepeat[i];
				auto s = &keyInputState[i];
				// ���݂��Ȃ��L�[�R�[�h�͓K���Ȓl�������Ă��܂����O���ŌĂяo���Ȃ��̂ō\��Ȃ�
				DirectX::Keyboard::Keys key = static_cast<DirectX::Keyboard::Keys>(i);
				s->STATE = kbState.IsKeyDown(key);
				s->TRIGGER = kbTracker->IsKeyPressed(key);
				s->RELEASED = kbTracker->IsKeyReleased(key);
				// �J��Ԃ����͏�Ԃ�ݒ肷��
				{
					// �t���O�̏�����
					r->isRepeated = false;
					// 2�x�ڈȍ~Trigger�ł��邱�Ƃ��m���߂�
					if (s->RELEASED == true)
					{
						r->isReleased = true;
					}
					if (s->TRIGGER == true)
					{
						if (r->isReleased == true)
						{
							// �J��Ԃ����͂�true�ɂȂ�B��
							r->isRepeated = true;
						}
						r->repeatTimer = 0.0f;
					}
					// �J��Ԃ����͎�t���Ԃ𒴂���ꍇ
					if ((r->isReleased == true) && (r->repeatTimer >= MAX_REPEAT_TIME))
					{
						r->isReleased = false;
						r->repeatCounter = 0;
					}
				}
				// �J��Ԃ��񐔂��C���N�������g
				if (r->isRepeated == true) ++(r->repeatCounter);
				// �^�C�}�[�����Z
				if (r->repeatTimer <= MAX_REPEAT_TIME)
				{
					r->repeatTimer += time->deltaTime();
				}
				s->REPEAT = r->isRepeated;

			}
		}
		// �}�E�X�̓��͏�Ԃ��擾
		{
			for (int i = 0; i < static_cast<int>(Mouse::NUM); ++i)
			{
				// �Q�Ɛ�ւ̃|�C���^�Ƃ��Ēu���Ă���
				auto r = &mouseRepeat[i];
				auto s = &mouseInputState[i];
				// �g���b�J�[�����Ԃ��擾
				s->STATE = (*mouseButtonPtrs[i] == DirectX::Mouse::ButtonStateTracker::ButtonState::HELD);
				s->TRIGGER = (*mouseButtonPtrs[i] == DirectX::Mouse::ButtonStateTracker::ButtonState::PRESSED);
				s->RELEASED = (*mouseButtonPtrs[i] == DirectX::Mouse::ButtonStateTracker::ButtonState::RELEASED);
				// �J��Ԃ����͏�Ԃ�ݒ肷��
				{
					// �t���O�̏�����
					r->isRepeated = false;
					// 2�x�ڈȍ~Trigger�ł��邱�Ƃ��m���߂�
					if (s->RELEASED == true)
					{
						r->isReleased = true;
					}
					if (s->TRIGGER == true)
					{
						if (r->isReleased == true)
						{
							// �J��Ԃ����͂�true�ɂȂ�B��
							r->isRepeated = true;
						}
						r->repeatTimer = 0.0f;
					}
					// �J��Ԃ����͎�t���Ԃ𒴂���ꍇ
					if ((r->isReleased == true) && (r->repeatTimer >= MAX_REPEAT_TIME))
					{
						r->isReleased = false;
						r->repeatCounter = 0;
					}
				}
				// �J��Ԃ��񐔂��C���N�������g
				if (r->isRepeated == true) ++(r->repeatCounter);
				// �^�C�}�[�����Z
				if (r->repeatTimer <= MAX_REPEAT_TIME)
				{
					r->repeatTimer += time->deltaTime();
				}
				s->REPEAT = r->isRepeated;
			}
			// �}�E�X�z�C�[�������Z�b�g����
			mouse.get()->ResetScrollWheelValue();
		}

		// ***********************************************
		// �Q�[���p�b�h�̍X�V
		// ***********************************************
		for (int i = 0; i < GAMEPAD_NUM; ++i)
		{
			// ���̓f�o�C�X�̌���Ԃ��擾(gamepad)
			padState[i] = gamePad->GetState(i, deadZone);

			// �ڑ�����Ă��Ȃ���Ώ������΂�
			if (!padState[i].IsConnected())continue;

			// �g���b�J�[�̍X�V(gamepad)
			padTracker[i].get()->Update(padState[i]);

			// �U���̎��Ԃɂ��X�V
			if (pad_vibrations[i].vibration == true)
			{
				pad_vibrations[i].timer += time->deltaTime();
				if (pad_vibrations[i].stop_time <= pad_vibrations[i].timer)
				{
					stopVibration(i);
				}
			}

			// �Q�[���p�b�h�̓��͏�Ԃ��擾
			for (int j = 0; j < static_cast<int>(GamePad::NUM); ++j)
			{
				// �Q�Ɛ�ւ̃|�C���^�Ƃ��Ēu���Ă���
				auto r = &padRepeat[i][j];
				auto s = &padInputState[i][j];

				// �g���b�J�[�����Ԃ��擾
				s->STATE = (*padButtonPtrs[i][j] == DirectX::GamePad::ButtonStateTracker::ButtonState::HELD);
				s->TRIGGER = (*padButtonPtrs[i][j] == DirectX::GamePad::ButtonStateTracker::ButtonState::PRESSED);
				s->RELEASED = (*padButtonPtrs[i][j] == DirectX::GamePad::ButtonStateTracker::ButtonState::RELEASED);
				// �J��Ԃ����͏�Ԃ�ݒ肷��
				{
					// �t���O�̏�����
					r->isRepeated = false;
					// 2�x�ڈȍ~Trigger�ł��邱�Ƃ��m���߂�
					if (s->RELEASED == true)
					{
						r->isReleased = true;
					}
					if (s->TRIGGER == true)
					{
						if (r->isReleased == true)
						{
							// �J��Ԃ����͂�true�ɂȂ�B��
							r->isRepeated = true;
						}
						r->repeatTimer = 0.0f;
					}
					// �J��Ԃ����͎�t���Ԃ𒴂���ꍇ
					if ((r->isReleased == true) && (r->repeatTimer >= MAX_REPEAT_TIME))
					{
						r->isReleased = false;
						r->repeatCounter = 0;
					}
				}
				// �J��Ԃ��񐔂��C���N�������g
				if (r->isRepeated == true) ++(r->repeatCounter);
				// �^�C�}�[�����Z
				if (r->repeatTimer <= MAX_REPEAT_TIME)
				{
					r->repeatTimer += time->deltaTime();
				}
				s->REPEAT = r->isRepeated;
			}
		}

	}

	// =====================================================
	// ���\�b�h(private)
	// =====================================================
	// �J��Ԃ��������Z�b�g����
	void MonoInput::resetRepeatState(RepeatState& state)
	{
		state.isReleased = false;
		state.isRepeated = false;
		state.repeatTimer = 0.0f;
		state.repeatCounter = 0;
	}

	// =====================================================
	// ���\�b�h(public)
	// =====================================================
	// Repeat���͂��󂯕t���鎞�Ԃ�ݒ肷��(�f�t�H���g��0.5f)
	//void MonoInput::setRepeatTime(float time);

	// =====================================================
	// Keyborad
	// =====================================================
	// �L�[�{�[�h��STATE���擾����
	//bool MonoInput::getKeyState(DirectX::Keyboard::Keys key) const;

	// �L�[�{�[�h��TRIGGER���擾����
	//bool MonoInput::getKeyTrigger(DirectX::Keyboard::Keys key) const;

	// �L�[�{�[�h��RELEASED���擾����
	//bool MonoInput::getKeyReleased(DirectX::Keyboard::Keys key) const;

	// �L�[�{�[�h��REPEAT���擾����
	//bool MonoInput::getKeyRepeat(DirectX::Keyboard::Keys key) const;

	// �L�[�̃��s�[�g�񐔂��擾����(�_�u���N���b�N����1��A3�N���b�N�ڂ�2���Ԃ�)
	//int MonoInput::getKeyRepeatCount(DirectX::Keyboard::Keys key) const;

	// =====================================================
	// GamePad
	// =====================================================
	// (No.player)�̉E�X�e�B�b�N�̒l���擾����(-1 ~ +1)
	//float MonoInput::getRStickX(int player) const;
	//float MonoInput::getRStickY(int player) const;

	// (No.player)�̍��X�e�B�b�N�̒l���擾����(-1 ~ +1)
	//float MonoInput::getLStickX(int player) const;
	//float MonoInput::getLStickY(int player) const;

	// (No.player)�̃g���K�[�̒l���擾����(0 ~ 1)
	//float MonoInput::getRTrigger(int player) const;
	//float MonoInput::getLTrigger(int player) const;

	// (No.player)��STATE���擾����(T or F)
	//bool MonoInput::getPadState(int player, Input::GamePad button) const;

	// (No.player)��TRIGGER���擾����(T or F)
	//bool MonoInput::getPadTrigger(int player, Input::GamePad button) const;

	// (No.player)��RELEASED���擾����(T or F)
	//bool MonoInput::getPadReleased(int player, Input::GamePad button) const;

	// (No.player)��REPEAT���擾����(T or F)
	//bool MonoInput::getPadRepeat(int player, Input::GamePad button) const;

	// �{�^���̃��s�[�g�񐔂��擾����(�_�u���N���b�N����1��A3�N���b�N�ڂ�2���Ԃ�)
	//int MonoInput::getPadRepeatCount(int player, Input::GamePad button) const;

	// �Q�[���p�b�h�̐U����ݒ肷��
	bool MonoInput::startVibration(int player, float leftMotor, float rightMotor, float leftTrigger, float rightTrigger)
	{
		return gamePad.get()->SetVibration(player, leftMotor, rightMotor, leftTrigger, rightTrigger);
	}
	bool MonoInput::setVibration(int player, float seconds, float leftMotor, float rightMotor, float leftTrigger, float rightTrigger)
	{
		pad_vibrations[player].timer = 0.0f;
		pad_vibrations[player].stop_time = seconds;
		pad_vibrations[player].vibration = true;
		return startVibration(player, leftMotor, rightMotor, leftTrigger, rightTrigger);
	}
	bool MonoInput::stopVibration(int player)
	{
		pad_vibrations[player].vibration = false;
		return gamePad.get()->SetVibration(player, 0.0f, 0.0f);
	}

	// �Q�[���p�b�h��DeadZone��ݒ肷��
	//void MonoInput::setPadDeadZone(DirectX::GamePad::DeadZone dz);


	// =====================================================
	// Mouse
	// =====================================================
	// �}�E�X�̍��W�擾
	//POINT MonoInput::getCursorPos() const;
	//int MonoInput::getCursorPosX() const;
	//int MonoInput::getCursorPosY() const;

	// �}�E�X�z�C�[���ʂ̎擾
	//int MonoInput::getMouseWheel() const;

	// �}�E�X���W�̐ݒ�
	void MonoInput::setCursorPos(POINT point)
	{
		POINT p = point;
		ClientToScreen(_hwnd, &p);
		SetCursorPos(p.x, p.y);
	}
	void MonoInput::setCursorPos(int x, int y)
	{
		POINT p = { x,y };
		ClientToScreen(_hwnd, &p);
		SetCursorPos(p.x, p.y);
	}
	void MonoInput::setCursorPosX(int x)
	{
		POINT p = { x,mouseState.y };
		ClientToScreen(_hwnd, &p);
		SetCursorPos(p.x, p.y);
	}
	void MonoInput::setCursorPosY(int y)
	{
		POINT p = { mouseState.x, y };
		ClientToScreen(_hwnd, &p);
		SetCursorPos(p.x, p.y);
	}

	// �}�E�X�̃{�^�����͂��擾����
	//bool MonoInput::getMouseState(TKGlib::Input::Mouse button) const;
	//bool MonoInput::getMouseTrigger(TKGlib::Input::Mouse button) const;
	//bool MonoInput::getMouseReleased(TKGlib::Input::Mouse button) const;
	//bool MonoInput::getMouseRepeat(TKGlib::Input::Mouse button) const;

	// �{�^���̃��s�[�g�񐔂��擾����(�_�u���N���b�N����1��A3�N���b�N�ڂ�2���Ԃ�)
	//int MonoInput::getMouseRepeatCount(TKGlib::Input::Mouse button) const;

	// �}�E�X�J�[�\���̉�����ݒ肷��(true->�\��   false->��\��)���Έʒu�̓f�t�H���g�\���A��Έʒu�̓f�t�H���g��\��
	void MonoInput::setMouseVisible(bool isVisible)
	{
		mouse.get()->SetVisible(isVisible);
	}

	// �}�E�X���W�𑊑Έʒu�A��Έʒu�̂ǂ���łƂ邩
	void MonoInput::setMousePosMode(DirectX::Mouse::Mode mode)
	{
		mouse.get()->SetMode(mode);
	}
}