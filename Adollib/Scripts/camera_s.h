#pragma once

#include "../Adollib/component_camera.h"
#include "../Adollib/rigit_body.h"

namespace Adollib
{
	class camera_s : public Component_camera
	{
	public:
		Transfome* player;

	public:
		void awake();

		// 所属するシーンの初期化時に一度だけ呼ばれる
		void start();

		// 毎フレーム呼ばれる更新処理
		void update();

		// 毎フレーム、update()後に呼ばれる更新処理
		void lateUpdate();

		// このスクリプトがアタッチされているGOのactiveSelfがtrueになった時呼ばれる
		void onEnable();

		// このスクリプトがアタッチされているGOのactiveSelfがfalseになった時呼ばれる
		void onDisable();

	};

}