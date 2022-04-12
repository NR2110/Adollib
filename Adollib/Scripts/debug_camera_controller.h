#pragma once

#include "../Adollib/Scripts/Object/component_camera.h"
#include "../Adollib/Scripts/Physics/collider.h"

namespace Adollib
{
	class Input_changer;


	class Debug_camera_controller : public Component
	{
	public:

		Vector2 max_rotate = Vector2(-50, 60); //カメラの上下の最大角度

		Vector3 rotate_vec_save;
		Vector2 cursol_pos_save;

		float rotate_speed = 0.5f;
		float linear_speed = 30;

	public:
		Quaternion camera_rot; //cameraの向き

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