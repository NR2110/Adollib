#pragma once
#include "stage_base.h"

namespace Adollib {

	class Stage_demo : public Stage_base {


	private:

		Gameobject* set_tree(Vector3 pos, Vector3 scale, Vector3 rotate);
		Gameobject* set_buttan(Vector3 pos, Vector3 scale, Vector3 rotate, Vector3 color = Vector3(1, 1, 1));
		Gameobject* set_door(Vector3 pos, Vector3 scale, Vector3 rotate, Vector3 color = Vector3(1, 1, 1));
		Gameobject* set_rope(Vector3 pos, Vector3 scale, Vector3 rotate, Vector3 color = Vector3(1, 1, 1));

	public:
		void awake() override;

		// 所属するシーンの初期化時に一度だけ呼ばれる
		void start()override;

		// 毎フレーム呼ばれる更新処理
		void update()override;

		void Update_hierarchy() override {};

		// このスクリプトがアタッチされているGOのactiveSelfがtrueになった時呼ばれる
		void onEnable()override;

		// このスクリプトがアタッチされているGOのactiveSelfがfalseになった時呼ばれる
		void onDisable()override;
	};
}