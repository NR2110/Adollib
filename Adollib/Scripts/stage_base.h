#pragma once

#include "../Adollib/Scripts/Object/component.h"


#include "stageparts_tags.h"

namespace Adollib {

	class Collider;

	class Stage_base : public Component {
	public:
		Stage_parts::Stageparts_tagbit tags; //このstageが持つtag

		float y_respown_value = 0; // Yがこれ以下だとrespownされる

	private:


	protected:
		Collider* set_sphere (Vector3 pos = Vector3(0), float r = 1, Vector3 color = Vector3(1, 1, 1), bool is_static = true);
		Collider* set_capsule(Vector3 pos = Vector3(0), float r = 1, float length = 1, Vector3 rotate = Vector3(0), Vector3 color = Vector3(1, 1, 1), bool is_static = true);
		Collider* set_box    (Vector3 pos = Vector3(0), Vector3 size = Vector3(1), Vector3 rotate = Vector3(0), Vector3 color = Vector3(1, 1, 1), bool is_static = true);
		Collider* set_meshbox(Vector3 pos = Vector3(0), Vector3 size = Vector3(1), Vector3 rotate = Vector3(0), Vector3 color = Vector3(1, 1, 1), bool is_static = true);
		Collider* set_plane  (Vector3 pos = Vector3(0), Vector3 normal = Vector3(0,1,0), Vector3 color = Vector3(1, 1, 1), bool is_static = true);

	public:
		void tag_reset() { tags = 0; };


		// addComponentされたときに呼ばれる
		virtual void awake() {};

		// 所属するシーンの初期化時に一度だけ呼ばれる
		virtual void start() {};

		// Hierarchyの表示(Imguiの関数 Imgui::begin,Imgui::endはいらない)
		virtual void Update_hierarchy() {};

		// 毎フレーム呼ばれる更新処理
		virtual void update() {};

		// このスクリプトがアタッチされているGOのactiveSelfがtrueになった時呼ばれる(GO生成時にスクリプトが有効な時も呼ばれる)
		virtual void onEnable() {};

		// このスクリプトがアタッチされているGOのactiveSelfがfalseになった時呼ばれる
		virtual void onDisable() {};

		// removeComponent()、clear()時に呼ぶ
		virtual void finalize() {};
	};

}