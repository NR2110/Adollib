#include "input.h"
#include "systems.h"

namespace Adollib
{
	// =====================================================
	// 初期化
	// =====================================================
	void MonoInput::initialize(HWND hwnd)
	{
		// メンバ変数の初期化
		MAX_REPEAT_TIME = 0.5f;
		deadZone = DirectX::GamePad::DeadZone::DEAD_ZONE_INDEPENDENT_AXES;

		_hwnd = hwnd;

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

		// トラッカー内の各ボタンへのポインタを設定
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
	// 更新処理
	// =====================================================
	void MonoInput::update()
	{
		// ***********************************************
		// キーボード、マウスの更新
		// ***********************************************
		// 入力デバイスの現状態を取得(keyboard, mouse)
		kbState = keyboard.get()->GetState();
		mouseState = mouse.get()->GetState();

		// トラッカーの更新(keyboard, mouse)
		kbTracker.get()->Update(kbState);
		mouseTracker.get()->Update(mouseState);

		// キーボードの入力状態を取得
		{
			for (int i = 0; i < 256; ++i)
			{
				// 参照先へのポインタとして置いておく
				auto r = &kbRepeat[i];
				auto s = &keyInputState[i];
				// 存在しないキーコードは適当な値が入ってしまうが外部で呼び出せないので構わない
				DirectX::Keyboard::Keys key = static_cast<DirectX::Keyboard::Keys>(i);
				s->STATE = kbState.IsKeyDown(key);
				s->TRIGGER = kbTracker->IsKeyPressed(key);
				s->RELEASED = kbTracker->IsKeyReleased(key);
				// 繰り返し入力状態を設定する
				{
					// フラグの初期化
					r->isRepeated = false;
					// 2度目以降Triggerであることを確かめる
					if (s->RELEASED == true)
					{
						r->isReleased = true;
					}
					if (s->TRIGGER == true)
					{
						if (r->isReleased == true)
						{
							// 繰り返し入力がtrueになる唯一
							r->isRepeated = true;
						}
						r->repeatTimer = 0.0f;
					}
					// 繰り返し入力受付時間を超える場合
					if ((r->isReleased == true) && (r->repeatTimer >= MAX_REPEAT_TIME))
					{
						r->isReleased = false;
						r->repeatCounter = 0;
					}
				}
				// 繰り返し回数をインクリメント
				if (r->isRepeated == true) ++(r->repeatCounter);
				// タイマーを加算
				if (r->repeatTimer <= MAX_REPEAT_TIME)
				{
					r->repeatTimer += Systems::time->deltaTime();
				}
				s->REPEAT = r->isRepeated;

			}
		}
		// マウスの入力状態を取得
		{
			for (int i = 0; i < static_cast<int>(Mouse::NUM); ++i)
			{
				// 参照先へのポインタとして置いておく
				auto r = &mouseRepeat[i];
				auto s = &mouseInputState[i];
				// トラッカーから状態を取得
				s->STATE = (*mouseButtonPtrs[i] == DirectX::Mouse::ButtonStateTracker::ButtonState::HELD);
				s->TRIGGER = (*mouseButtonPtrs[i] == DirectX::Mouse::ButtonStateTracker::ButtonState::PRESSED);
				s->RELEASED = (*mouseButtonPtrs[i] == DirectX::Mouse::ButtonStateTracker::ButtonState::RELEASED);
				// 繰り返し入力状態を設定する
				{
					// フラグの初期化
					r->isRepeated = false;
					// 2度目以降Triggerであることを確かめる
					if (s->RELEASED == true)
					{
						r->isReleased = true;
					}
					if (s->TRIGGER == true)
					{
						if (r->isReleased == true)
						{
							// 繰り返し入力がtrueになる唯一
							r->isRepeated = true;
						}
						r->repeatTimer = 0.0f;
					}
					// 繰り返し入力受付時間を超える場合
					if ((r->isReleased == true) && (r->repeatTimer >= MAX_REPEAT_TIME))
					{
						r->isReleased = false;
						r->repeatCounter = 0;
					}
				}
				// 繰り返し回数をインクリメント
				if (r->isRepeated == true) ++(r->repeatCounter);
				// タイマーを加算
				if (r->repeatTimer <= MAX_REPEAT_TIME)
				{
					r->repeatTimer += Systems::time->deltaTime();
				}
				s->REPEAT = r->isRepeated;
			}
			// マウスホイールをリセットする
			mouse.get()->ResetScrollWheelValue();
		}

		// ***********************************************
		// ゲームパッドの更新
		// ***********************************************
		for (int i = 0; i < GAMEPAD_NUM; ++i)
		{
			// 入力デバイスの現状態を取得(gamepad)
			padState[i] = gamePad->GetState(i, deadZone);

			// 接続されていなければ処理を飛ばす
			if (!padState[i].IsConnected())continue;

			// トラッカーの更新(gamepad)
			padTracker[i].get()->Update(padState[i]);

			// 振動の時間による更新
			if (pad_vibrations[i].vibration == true)
			{
				pad_vibrations[i].timer += Systems::time->deltaTime();
				if (pad_vibrations[i].stop_time <= pad_vibrations[i].timer)
				{
					stopVibration(i);
				}
			}

			// ゲームパッドの入力状態を取得
			for (int j = 0; j < static_cast<int>(GamePad::NUM); ++j)
			{
				// 参照先へのポインタとして置いておく
				auto r = &padRepeat[i][j];
				auto s = &padInputState[i][j];

				// トラッカーから状態を取得
				s->STATE = (*padButtonPtrs[i][j] == DirectX::GamePad::ButtonStateTracker::ButtonState::HELD);
				s->TRIGGER = (*padButtonPtrs[i][j] == DirectX::GamePad::ButtonStateTracker::ButtonState::PRESSED);
				s->RELEASED = (*padButtonPtrs[i][j] == DirectX::GamePad::ButtonStateTracker::ButtonState::RELEASED);
				// 繰り返し入力状態を設定する
				{
					// フラグの初期化
					r->isRepeated = false;
					// 2度目以降Triggerであることを確かめる
					if (s->RELEASED == true)
					{
						r->isReleased = true;
					}
					if (s->TRIGGER == true)
					{
						if (r->isReleased == true)
						{
							// 繰り返し入力がtrueになる唯一
							r->isRepeated = true;
						}
						r->repeatTimer = 0.0f;
					}
					// 繰り返し入力受付時間を超える場合
					if ((r->isReleased == true) && (r->repeatTimer >= MAX_REPEAT_TIME))
					{
						r->isReleased = false;
						r->repeatCounter = 0;
					}
				}
				// 繰り返し回数をインクリメント
				if (r->isRepeated == true) ++(r->repeatCounter);
				// タイマーを加算
				if (r->repeatTimer <= MAX_REPEAT_TIME)
				{
					r->repeatTimer += Systems::time->deltaTime();
				}
				s->REPEAT = r->isRepeated;
			}
		}

	}

	// =====================================================
	// メソッド(private)
	// =====================================================
	// 繰り返し情報をリセットする
	void MonoInput::resetRepeatState(RepeatState& state)
	{
		state.isReleased = false;
		state.isRepeated = false;
		state.repeatTimer = 0.0f;
		state.repeatCounter = 0;
	}

	// =====================================================
	// メソッド(public)
	// =====================================================
	// Repeat入力を受け付ける時間を設定する(デフォルトは0.5f)
	//void MonoInput::setRepeatTime(float time);

	// =====================================================
	// Keyborad
	// =====================================================
	// キーボードのSTATEを取得する
	//bool MonoInput::getKeyState(DirectX::Keyboard::Keys key) const;

	// キーボードのTRIGGERを取得する
	//bool MonoInput::getKeyTrigger(DirectX::Keyboard::Keys key) const;

	// キーボードのRELEASEDを取得する
	//bool MonoInput::getKeyReleased(DirectX::Keyboard::Keys key) const;

	// キーボードのREPEATを取得する
	//bool MonoInput::getKeyRepeat(DirectX::Keyboard::Keys key) const;

	// キーのリピート回数を取得する(ダブルクリック時に1回、3クリック目に2回を返す)
	//int MonoInput::getKeyRepeatCount(DirectX::Keyboard::Keys key) const;

	// =====================================================
	// GamePad
	// =====================================================
	// (No.player)の右スティックの値を取得する(-1 ~ +1)
	//float MonoInput::getRStickX(int player) const;
	//float MonoInput::getRStickY(int player) const;

	// (No.player)の左スティックの値を取得する(-1 ~ +1)
	//float MonoInput::getLStickX(int player) const;
	//float MonoInput::getLStickY(int player) const;

	// (No.player)のトリガーの値を取得する(0 ~ 1)
	//float MonoInput::getRTrigger(int player) const;
	//float MonoInput::getLTrigger(int player) const;

	// (No.player)のSTATEを取得する(T or F)
	//bool MonoInput::getPadState(int player, Input::GamePad button) const;

	// (No.player)のTRIGGERを取得する(T or F)
	//bool MonoInput::getPadTrigger(int player, Input::GamePad button) const;

	// (No.player)のRELEASEDを取得する(T or F)
	//bool MonoInput::getPadReleased(int player, Input::GamePad button) const;

	// (No.player)のREPEATを取得する(T or F)
	//bool MonoInput::getPadRepeat(int player, Input::GamePad button) const;

	// ボタンのリピート回数を取得する(ダブルクリック時に1回、3クリック目に2回を返す)
	//int MonoInput::getPadRepeatCount(int player, Input::GamePad button) const;

	// ゲームパッドの振動を設定する
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

	// ゲームパッドのDeadZoneを設定する
	//void MonoInput::setPadDeadZone(DirectX::GamePad::DeadZone dz);


	// =====================================================
	// Mouse
	// =====================================================
	// マウスの座標取得
	//POINT MonoInput::getCursorPos() const;
	//int MonoInput::getCursorPosX() const;
	//int MonoInput::getCursorPosY() const;

	// マウスホイール量の取得
	//int MonoInput::getMouseWheel() const;

	// マウス座標の設定
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

	// マウスのボタン入力を取得する
	//bool MonoInput::getMouseState(TKGlib::Input::Mouse button) const;
	//bool MonoInput::getMouseTrigger(TKGlib::Input::Mouse button) const;
	//bool MonoInput::getMouseReleased(TKGlib::Input::Mouse button) const;
	//bool MonoInput::getMouseRepeat(TKGlib::Input::Mouse button) const;

	// ボタンのリピート回数を取得する(ダブルクリック時に1回、3クリック目に2回を返す)
	//int MonoInput::getMouseRepeatCount(TKGlib::Input::Mouse button) const;

	// マウスカーソルの可視性を設定する(true->表示   false->非表示)相対位置はデフォルト表示、絶対位置はデフォルト非表示
	void MonoInput::setMouseVisible(bool isVisible)
	{
		mouse.get()->SetVisible(isVisible);
	}

	// マウス座標を相対位置、絶対位置のどちらでとるか
	void MonoInput::setMousePosMode(DirectX::Mouse::Mode mode)
	{
		mouse.get()->SetMode(mode);
	}
}