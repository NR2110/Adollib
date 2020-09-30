#pragma once

#include "../Adollib/component.h"
#include "../Adollib/collider.h"

namespace Adollib
{
	class Camera_manager_s : public Component
	{
	public:
		void set_camera(vector3 pos, vector3 look_pos);

	public:
		void awake();

		// 所属するシーンの初期化時に一度だけ呼ばれる
		void start();

		// 毎フレーム呼ばれる更新処理
		void update();

		// このスクリプトがアタッチされているGOのactiveSelfがtrueになった時呼ばれる
		void onEnable();

		// このスクリプトがアタッチされているGOのactiveSelfがfalseになった時呼ばれる
		void onDisable();

	};

}