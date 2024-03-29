#pragma once

#include "../Adollib/Scripts/Object/component.h"
#include "../Adollib/Scripts/Renderer/renderer_base.h"

#include <vector>
#include "stageparts_tags.h"
#include "stage_types.h"

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

		Stage_types next_stage = Stage_types::none; //次のstage この値がnone以外でrespownが行われるとき 次のstageに行く

	protected:
		std::vector<Gameobject*> stage_parts; // このstageが管理しているGOのポインタ

	private:


		void tag_reset() { tags = 0; };

	protected:
		Collider* set_sphere (const Vector3& pos = Vector3(0), const float& r = 1, const Vector3& color = Vector3(1, 1, 1), Gameobject* pearent = nullptr, bool is_static = true);
		Collider* set_capsule(const Vector3& pos = Vector3(0), const float& r = 1, const float& length = 1, const Vector3& rotate = Vector3(0), const Vector3& color = Vector3(1, 1, 1), Gameobject* pearent = nullptr, bool is_static = true);
		Collider* set_box    (const Vector3& pos = Vector3(0), const Vector3& size = Vector3(1), const Vector3& rotate = Vector3(0), const Vector3& color = Vector3(1, 1, 1), Gameobject* pearent = nullptr,bool is_static = true);
		Collider* set_meshbox(const Vector3& pos = Vector3(0), const Vector3& size = Vector3(1), const Vector3& rotate = Vector3(0), const Vector3& color = Vector3(1, 1, 1), Gameobject* pearent = nullptr, bool is_static = true);
		Gameobject* set_plane  (const Vector3& pos = Vector3(0), const Vector3& size = Vector3(1), const Vector3& rotate = Vector3(0), const Vector3& color = Vector3(1, 1, 1), Gameobject* pearent = nullptr, bool is_static = true);

		Gameobject* set_respown_area(const Vector3& pos, const Vector3& size, const Vector3& rotate, int respown_num, Vector3 respown_pos, Gameobject* pearent = nullptr);
		Gameobject* set_goal_area(const Vector3& pos, const Vector3& size, const Vector3& rotate, Stage_types next_stage, Gameobject* pearent = nullptr);

	public:
		// stage_managerが呼ぶawake このstageに切り替わったときに呼ばれる
		virtual void stage_awake() = 0;

		// stage_managerが呼ぶdestroy このstageから次に切り替わったときに呼ばれる
		virtual void stage_destroy() = 0;

		virtual void  summon_block_01() {};
		virtual void  summon_block_02() {};
		virtual void  summon_block_03() {};
		virtual void  summon_block_04() {};

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