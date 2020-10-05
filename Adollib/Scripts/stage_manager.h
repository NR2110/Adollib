#pragma once
#include "../Adollib/component.h"
#include "../Adollib/collider.h"
#include "stage_base.h"

namespace Adollib {

	class Stage_manager_s : public Component {
	public:
		std::list<Stage_base*> stages;
	private:
		vector3 *camera_pos;
		quaternion *camera_orient;

	private:
		void set_cube(vector3 pos, vector3 size, vector3 rotate, vector4 color = vector4(1, 1, 1, 1));



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