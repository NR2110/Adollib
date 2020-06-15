#pragma once

#include "../Adollib/component.h"
#include "../Adollib/rigit_body.h"


namespace Adollib
{
	class object_fall : public Component
	{
	public:
		Rigitbody* collier;

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
