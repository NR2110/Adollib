#pragma once

#include "singleton.h"
#include "time.h"

#include "./DirectXTK-master/Inc/Keyboard.h"
#include "./DirectXTK-master/Inc/GamePad.h"
#include "./DirectXTK-master/Inc/Mouse.h"

#define NOMINMAX
#include <Windows.h>
#include <memory>

namespace Adollib
{
	// �p�b�h�̍ő吔
	const int GAMEPAD_NUM = 4;
	// �Q�[���p�b�h
	enum class GamePad
	{
		UP = 0,	// dpad
		DOWN,
		RIGHT,
		LEFT,
		A = 4,	// buttons
		B,
		X,
		Y,
		LSTICK,
		RSTICK,
		LSHOULDER,
		RSHOULDER,
		BACK,
		START,

		NUM,	// �I�[
	};

	// �}�E�X
	enum class Mouse
	{
		LBUTTON = 0,
		MBUTTON,
		RBUTTON,
		X1BUTTON,
		X2BUTTON,

		NUM,	// �I�[
	};

	// �L�[�{�[�h
	enum class Key
	{
		None = 0,

		Back = 0x8,
		Tab = 0x9,

		Enter = 0xd,

		Pause = 0x13,
		CapsLock = 0x14,
		Kana = 0x15,

		Kanji = 0x19,

		Escape = 0x1b,
		ImeConvert = 0x1c,
		ImeNoConvert = 0x1d,

		Space = 0x20,
		PageUp = 0x21,
		PageDown = 0x22,
		End = 0x23,
		Home = 0x24,
		Left = 0x25,
		Up = 0x26,
		Right = 0x27,
		Down = 0x28,
		Select = 0x29,
		Print = 0x2a,
		Execute = 0x2b,
		PrintScreen = 0x2c,
		Insert = 0x2d,
		Delete = 0x2e,
		Help = 0x2f,
		D0 = 0x30,
		D1 = 0x31,
		D2 = 0x32,
		D3 = 0x33,
		D4 = 0x34,
		D5 = 0x35,
		D6 = 0x36,
		D7 = 0x37,
		D8 = 0x38,
		D9 = 0x39,

		A = 0x41,
		B = 0x42,
		C = 0x43,
		D = 0x44,
		E = 0x45,
		F = 0x46,
		G = 0x47,
		H = 0x48,
		I = 0x49,
		J = 0x4a,
		K = 0x4b,
		L = 0x4c,
		M = 0x4d,
		N = 0x4e,
		O = 0x4f,
		P = 0x50,
		Q = 0x51,
		R = 0x52,
		S = 0x53,
		T = 0x54,
		U = 0x55,
		V = 0x56,
		W = 0x57,
		X = 0x58,
		Y = 0x59,
		Z = 0x5a,
		LeftWindows = 0x5b,
		RightWindows = 0x5c,
		Apps = 0x5d,

		Sleep = 0x5f,
		NumPad0 = 0x60,
		NumPad1 = 0x61,
		NumPad2 = 0x62,
		NumPad3 = 0x63,
		NumPad4 = 0x64,
		NumPad5 = 0x65,
		NumPad6 = 0x66,
		NumPad7 = 0x67,
		NumPad8 = 0x68,
		NumPad9 = 0x69,
		Multiply = 0x6a,
		Add = 0x6b,
		Separator = 0x6c,
		Subtract = 0x6d,

		Decimal = 0x6e,
		Divide = 0x6f,
		F1 = 0x70,
		F2 = 0x71,
		F3 = 0x72,
		F4 = 0x73,
		F5 = 0x74,
		F6 = 0x75,
		F7 = 0x76,
		F8 = 0x77,
		F9 = 0x78,
		F10 = 0x79,
		F11 = 0x7a,
		F12 = 0x7b,
		F13 = 0x7c,
		F14 = 0x7d,
		F15 = 0x7e,
		F16 = 0x7f,
		F17 = 0x80,
		F18 = 0x81,
		F19 = 0x82,
		F20 = 0x83,
		F21 = 0x84,
		F22 = 0x85,
		F23 = 0x86,
		F24 = 0x87,

		NumLock = 0x90,
		Scroll = 0x91,

		LeftShift = 0xa0,
		RightShift = 0xa1,
		LeftControl = 0xa2,
		RightControl = 0xa3,
		LeftAlt = 0xa4,
		RightAlt = 0xa5,
		BrowserBack = 0xa6,
		BrowserForward = 0xa7,
		BrowserRefresh = 0xa8,
		BrowserStop = 0xa9,
		BrowserSearch = 0xaa,
		BrowserFavorites = 0xab,
		BrowserHome = 0xac,
		VolumeMute = 0xad,
		VolumeDown = 0xae,
		VolumeUp = 0xaf,
		MediaNextTrack = 0xb0,
		MediaPreviousTrack = 0xb1,
		MediaStop = 0xb2,
		MediaPlayPause = 0xb3,
		LaunchMail = 0xb4,
		SelectMedia = 0xb5,
		LaunchApplication1 = 0xb6,
		LaunchApplication2 = 0xb7,

		OemSemicolon = 0xba,
		OemPlus = 0xbb,
		OemComma = 0xbc,
		OemMinus = 0xbd,
		OemPeriod = 0xbe,
		OemQuestion = 0xbf,
		OemTilde = 0xc0,

		OemOpenBrackets = 0xdb,
		OemPipe = 0xdc,
		OemCloseBrackets = 0xdd,
		OemQuotes = 0xde,
		Oem8 = 0xdf,

		OemBackslash = 0xe2,

		ProcessKey = 0xe5,

		OemCopy = 0xf2,
		OemAuto = 0xf3,
		OemEnlW = 0xf4,

		Attn = 0xf6,
		Crsel = 0xf7,
		Exsel = 0xf8,
		EraseEof = 0xf9,
		Play = 0xfa,
		Zoom = 0xfb,

		Pa1 = 0xfd,
		OemClear = 0xfe,

		NUM		// �I�[
	};

	class MonoInput : public Singleton<MonoInput>
	{
	private:
		// �J��Ԃ����̓f�[�^�p�\����
		struct RepeatState
		{
			bool	isReleased;	// �����ꂽ��ł��邱�Ƃ��m�F����t���O
			bool	isRepeated;	// �J��Ԃ����͂��ꂽ���ɗ��t���O
			float	repeatTimer;// ������Ă���̃^�C�}�[(isReleased��true���������Z�����)
			int		repeatCounter;	// �J��Ԃ����͉�(Trigger�łƂ�B�_�u���N���b�N����1�ɂȂ銴��)
		};

		// ���͏�ԗp�\����
		struct InputState
		{
			bool STATE;
			bool TRIGGER;
			bool RELEASED;
			bool REPEAT;
		};

		// GamePad�̐U���p�\����
		struct PadVibrate
		{
			bool vibration = false;	// ���ݐU�����Ă��邩�ǂ���
			float timer = 0.0f;		// ���݂܂ł̐U������[s]
			float stop_time = 0.0f;	// �ݒ肵����~����[s]
		};

		// �J��Ԃ����͂������E����
		float MAX_REPEAT_TIME;

		// �J��Ԃ����͏�Ԃ�ۑ�����z��
		RepeatState	kbRepeat[256/*DirectX::Keyboard::Keys*/] = {};
		RepeatState	padRepeat[GAMEPAD_NUM][static_cast<int>(GamePad::NUM)/*buttons*/] = {};
		RepeatState	mouseRepeat[static_cast<int>(Mouse::NUM)/*buttons*/] = {};

		// �L�[��ԏ��
		InputState	keyInputState[256/*DirectX::Keyboard::Keys*/] = {};
		InputState	padInputState[GAMEPAD_NUM][static_cast<int>(GamePad::NUM)] = {};
		InputState	mouseInputState[static_cast<int>(Mouse::NUM)] = {};

		// �f�o�C�X���̓g���b�J�[���̊e�{�^���ւ̃|�C���^(initialize�Ńg���b�J�[�쐬��ɐݒ�)
		const DirectX::GamePad::ButtonStateTracker::ButtonState* padButtonPtrs[GAMEPAD_NUM][static_cast<int>(GamePad::NUM)/*buttons*/] = {};
		const DirectX::Mouse::ButtonStateTracker::ButtonState* mouseButtonPtrs[static_cast<int>(Mouse::NUM)/*buttons*/] = {};

		// �Q�[���p�b�h�̐U�����Ǘ�����\���̔z��
		PadVibrate pad_vibrations[GAMEPAD_NUM];

		// �Q�[���p�b�h��DeadZone�̐ݒ�
		DirectX::GamePad::DeadZone deadZone;
		// ���������ɃE�B���h�E�n���h�����󂯎��
		HWND _hwnd;
		// Time�I�u�W�F�N�g�̃|�C���^���擾����
		const Time* time = nullptr;


		// ���̓f�o�C�X�Ɋւ���|�C���^
		std::unique_ptr<DirectX::Keyboard>	keyboard;
		std::unique_ptr<DirectX::GamePad>	gamePad;
		std::unique_ptr<DirectX::Mouse>		mouse;
		std::unique_ptr<DirectX::Keyboard::KeyboardStateTracker>	kbTracker;
		std::unique_ptr<DirectX::GamePad::ButtonStateTracker>		padTracker[GAMEPAD_NUM];
		std::unique_ptr<DirectX::Mouse::ButtonStateTracker>			mouseTracker;
		// ���͏�Ԃ��i�[�����
		DirectX::Keyboard::State	kbState;
		DirectX::GamePad::State		padState[GAMEPAD_NUM];
		DirectX::Mouse::State		mouseState;

	private:
		// ==================================================================
		// ���\�b�h(private)
		// ==================================================================
		// �J��Ԃ��������Z�b�g����
		void resetRepeatState(RepeatState&);

	public:
		// ������(call only first time)
		void initialize(HWND);
		// ���͏��̍X�V(call every frame)
		void update();

		// ==================================================================
		// ���\�b�h(public)
		// ==================================================================
		// Repeat���͂��󂯕t���鎞�Ԃ�ݒ肷��(�f�t�H���g��0.5f)
		void setRepeatTime(float time)
		{
			MAX_REPEAT_TIME = time;
		}


		// ==================================================================
		// KeyBoard
		// ==================================================================
		// �L�[�{�[�h��STATE���擾����
		bool getKeyState(Key key) const
		{
			return keyInputState[static_cast<int>(key)].STATE;
		}
		// �L�[�{�[�h��TRIGGER���擾����
		bool getKeyTrigger(Key key) const
		{
			return keyInputState[static_cast<int>(key)].TRIGGER;
		}
		// �L�[�{�[�h��RELEASED���擾����
		bool getKeyReleased(Key key) const
		{
			return keyInputState[static_cast<int>(key)].RELEASED;
		}
		// �L�[�{�[�h��REPEAT���擾����
		bool getKeyRepeat(Key key) const
		{
			return keyInputState[static_cast<int>(key)].REPEAT;
		}

		// �L�[�̃��s�[�g�񐔂��擾����(�_�u���N���b�N����1��A3�N���b�N�ڂ�2���Ԃ�)
		int getKeyRepeatCount(Key key) const
		{
			return kbRepeat[static_cast<int>(key)].repeatCounter;
		}

		// ==================================================================
		// GamePad
		// ==================================================================
		// (No.player)�̉E�X�e�B�b�N�̒l���擾����(-1 ~ +1)
		float getRStickX(int player) const
		{
			return padState[player].thumbSticks.rightX;
		}
		float getRStickY(int player) const
		{
			return padState[player].thumbSticks.rightY;
		}
		// (No.player)�̍��X�e�B�b�N�̒l���擾����(-1 ~ +1)
		float getLStickX(int player) const
		{
			return padState[player].thumbSticks.leftX;
		}
		float getLStickY(int player) const
		{
			return padState[player].thumbSticks.leftY;
		}
		// (No.player)�̃g���K�[�̒l���擾����(0 ~ 1)
		float getRTrigger(int player) const
		{
			return padState[player].triggers.right;
		}
		float getLTrigger(int player) const
		{
			return padState[player].triggers.left;
		}
		// (No.player)��STATE���擾����(T or F)
		bool getPadState(int player, GamePad button) const
		{
			return padInputState[player][static_cast<int>(button)].STATE;
		}
		// (No.player)��TRIGGER���擾����(T or F)
		bool getPadTrigger(int player, GamePad button) const
		{
			return padInputState[player][static_cast<int>(button)].TRIGGER;
		}
		// (No.player)��RELEASED���擾����(T or F)
		bool getPadReleased(int player, GamePad button) const
		{
			return padInputState[player][static_cast<int>(button)].RELEASED;
		}
		// (No.player)��REPEAT���擾����(T or F)
		bool getPadRepeat(int player, GamePad button) const
		{
			return padInputState[player][static_cast<int>(button)].REPEAT;
		}

		// �{�^���̃��s�[�g�񐔂��擾����(�_�u���N���b�N����1��A3�N���b�N�ڂ�2���Ԃ�)
		int getPadRepeatCount(int player, GamePad button) const
		{
			return padRepeat[player][static_cast<int>(button)].repeatCounter;
		}

		// �Q�[���p�b�h�̐U����ݒ肷��
	private:
		bool startVibration(int player, float leftMotor, float rightMotor, float leftTrigger = 0.f, float rightTrigger = 0.f);
	public:
		// =================================
		// �f���^�^�C���ŐU�����Ԃ��Z�b�g����
		// �^�C���X�P�[����������Ǝ����ԂŐU������
		// ---------------------------------
		// float	:	seconds[s]
		// =================================
		bool setVibration(int player, float seconds/*vibrate timer[s]*/, float leftMotor, float rightMotor, float leftTrigger = 0.f, float rightTrigger = 0.f);
		bool stopVibration(int player);

		// �Q�[���p�b�h��DeadZone��ݒ肷��
		void setPadDeadZone(DirectX::GamePad::DeadZone dz = DirectX::GamePad::DeadZone::DEAD_ZONE_INDEPENDENT_AXES)
		{
			deadZone = dz;
		}


		// ==================================================================
		// Mouse
		// ==================================================================
		// �}�E�X�̍��W�擾
		POINT	getCursorPos() const
		{
			return POINT{ mouseState.x,mouseState.y };
		}
		int		getCursorPosX() const
		{
			return mouseState.x;
		}
		int		getCursorPosY() const
		{
			return mouseState.y;
		}
		// �}�E�X�z�C�[���ʂ̎擾
		int		getMouseWheel() const
		{
			return mouseState.scrollWheelValue;
		}
		// �}�E�X���W�̐ݒ�(Window��)
		void setCursorPos(POINT);
		void setCursorPos(int, int);
		void setCursorPosX(int);
		void setCursorPosY(int);
		// �}�E�X�̃{�^�����͂��擾����
		bool getMouseState(Adollib::Mouse button) const
		{
			return mouseInputState[static_cast<int>(button)].STATE;
		}
		bool getMouseTrigger(Adollib::Mouse button) const
		{
			return mouseInputState[static_cast<int>(button)].TRIGGER;
		}
		bool getMouseReleased(Adollib::Mouse button) const
		{
			return mouseInputState[static_cast<int>(button)].RELEASED;
		}
		bool getMouseRepeat(Adollib::Mouse button) const
		{
			return mouseInputState[static_cast<int>(button)].REPEAT;
		}

		// �{�^���̃��s�[�g�񐔂��擾����(�_�u���N���b�N����1��A3�N���b�N�ڂ�2���Ԃ�)
		int getMouseRepeatCount(Adollib::Mouse button) const
		{
			return mouseRepeat[static_cast<int>(button)].repeatCounter;
		}

		// �}�E�X�J�[�\���̉�����ݒ肷��(true->�\��   false->��\��)���Έʒu�̓f�t�H���g�\���A��Έʒu�̓f�t�H���g��\��
		void setMouseVisible(bool);
		// �}�E�X���W�𑊑Έʒu�A��Έʒu�̂ǂ���łƂ邩
		void setMousePosMode(DirectX::Mouse::Mode);

		// =====================================================================

	public:
		MonoInput() {}
		~MonoInput() {}
	};
	//}
}
