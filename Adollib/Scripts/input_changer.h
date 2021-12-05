#pragma once

#include "../Adollib/Scripts/Object/component.h"

namespace Adollib
{
	class Input_changer : public Component
	{
	public:
		int pad_num = 0;

		Vector3 dir; //動く向き
		Vector2 cursol_move; //cursolの移動量

		bool is_Rarm_state = false;
		bool is_Larm_state = false;

		bool is_respown_trigger = false;

		Quaternion camera_rot; //cameraの向き


	private:
		//const Key move_front = Key::W;
		//const Key move_left =  Key::A;
		//const Key move_back =  Key::S;
		//const Key move_right = Key::D;

		//const Mouse Rarm_up = Mouse::RBUTTON;
		//const Mouse Larm_up = Mouse::LBUTTON;

		//const GamePad Rarm_up_pad = GamePad::RSHOULDER;
		//const GamePad Larm_up_pad = GamePad::RSHOULDER;



	public:
		void awake() override;

		// 所属するシーンの初期化時に一度だけ呼ばれる
		void start() override;

		// 毎フレーム呼ばれる更新処理
		void update() override;

		// このスクリプトがアタッチされているGOのactiveSelfがtrueになった時呼ばれる
		void onEnable() override;

		// このスクリプトがアタッチされているGOのactiveSelfがfalseになった時呼ばれる
		void onDisable() override;

	};

}