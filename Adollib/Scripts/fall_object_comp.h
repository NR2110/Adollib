#pragma once

#include "../Adollib/Scripts/Object/component_camera.h"
#include "../Adollib/Scripts/Physics/collider.h"

namespace Adollib
{

	class Fall_object_comp : public Component
	{
	private:


	public:
		std::shared_ptr<Transform> check_transform;
		float delete_y = -100;

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