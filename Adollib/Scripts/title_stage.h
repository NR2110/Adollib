#pragma once

#include "../Adollib/Scripts/Object/component_camera.h"
#include "../Adollib/Scripts/Physics/collider.h"

namespace Adollib
{

	class Title_stage : public Component
	{
	private:
		Vector4 wall_color ;
		Vector4 floar_color;
		Vector4 stair_color;

		Collider* set_box(const char* name, Vector3 position, Vector3 rotate, Vector3 scale, Vector4 color, Gameobject* pearent = nullptr, bool is_moveable = false);
		Collider* set_gear(const char* name, Vector3 position, Vector3 rotate, Vector3 scale, Vector4 color, int gear_tooth_num, Gameobject* pearent = nullptr, bool is_moveable = false);
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