#pragma once
#include "player_base.h"

#include "camera_s.h"

namespace Adollib
{

	class Playerlead_s : public Player_base
	{
	private:

	public:
		Camera* camera;
		Sphere* sphere;

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
