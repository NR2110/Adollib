#pragma once

#include "../Adollib/Scripts/Object/component.h"

#include <vector>
#include "stageparts_tags.h"

namespace Adollib {

	class Gameobject;
	class Collider;

	class Stage_base : public Component {
	public:
		Stage_parts::Stageparts_tagbit tags; //このstageが持つtag スイッチなどのflagの管理

		int respown_num = -1; // 前のrespown位置でrespownしないように respown毎にintの値を持たせる

		float y_respown_limit = -150; // Yがこれ以下だとrespownされる
		float y_respown_pos = 100; // respownしたときのYの座標

		float y_player_respown_limit = -130; // Yがこれ以下だとrespownされる playerはstageの小物より上でrespownされてほしいから変数を準備
		Vector3 player_respown_pos;
		int player_respown_num = 0; // 現在のrespownposの番号 前のrespownposで更新しないようにするため


	protected:
		std::vector<Gameobject*> stage_parts; // このstageが管理しているGOのポインタ

	private:


		void tag_reset() { tags = 0; };

	protected:
		Collider* set_sphere (Vector3 pos = Vector3(0), float r = 1, Vector3 color = Vector3(1, 1, 1), Gameobject* pearent = nullptr, bool is_static = true);
		Collider* set_capsule(Vector3 pos = Vector3(0), float r = 1, float length = 1, Vector3 rotate = Vector3(0), Vector3 color = Vector3(1, 1, 1), Gameobject* pearent = nullptr, bool is_static = true);
		Collider* set_box    (Vector3 pos = Vector3(0), Vector3 size = Vector3(1), Vector3 rotate = Vector3(0), Vector3 color = Vector3(1, 1, 1), Gameobject* pearent = nullptr,bool is_static = true);
		Collider* set_meshbox(Vector3 pos = Vector3(0), Vector3 size = Vector3(1), Vector3 rotate = Vector3(0), Vector3 color = Vector3(1, 1, 1), Gameobject* pearent = nullptr, bool is_static = true);
		Gameobject* set_plane  (Vector3 pos = Vector3(0), Vector3 size = Vector3(1), Vector3 rotate = Vector3(0), Vector3 color = Vector3(1, 1, 1), Gameobject* pearent = nullptr, bool is_static = true);

	public:
		// stage_managerが呼ぶawake このstageに切り替わったときに呼ばれる
		virtual void stage_awake() = 0;

		// stage_managerが呼ぶdestroy このstageから次に切り替わったときに呼ばれる
		virtual void stage_destroy() = 0;

	public:

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