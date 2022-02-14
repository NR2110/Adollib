#pragma once
#include "stage_base.h"

namespace Adollib {

	// tutrial用
	//class Player_manager;

	class Stage_02 : public Stage_base {

	private:


	private:

		const Vector3 base_color = Vector3(233, 225, 217) / 255.0f * 0.9f;
		const Vector3 grass_color = Vector3(188, 214, 54) / 255.0f * 0.7f * 0.9f;
		const Vector3 wood_color = Vector3(90, 47, 27) / 255.0f * 0.9f;
		const Vector3 roof_color = Vector3(233, 225, 217) / 255.0f * 0.9f;
		const Vector3 stair_color = Vector3(215, 205, 197) / 255.0f * 0.9f;

		const Vector3 moveable_red = Vector3(1, 196 / 255.0f, 196 / 255.0f);
		const Vector3 moveable_green = Vector3(196 / 255.0f, 1, 196 / 255.0f);
		const Vector3 moveable_blue = Vector3(196 / 255.0f, 196 / 255.0f, 1);


		// 木を作る
		Gameobject* set_tree(const Vector3& pos, const Vector3& scale, const Vector3& rotate, const float y_scale = 1, const float tan_scale = 1, Gameobject* pearent = nullptr);
		// 柱を作る
		Gameobject* set_pillar(const Vector3& pos, const Vector3& scale, const Vector3& rotate, Gameobject* pearent = nullptr, const float base_length = 0.5f, const float pillar_length = 7, const bool moveable = true);
		// 柵を作る
		Gameobject* set_fence(const Vector3& pos, const Vector3& scale, const Vector3& rotate, Gameobject* pearent = nullptr, const int fence_count = 1);
		// ドアを作る
		Gameobject* set_door(const Vector3& pos, const Vector3& scale, const Vector3& rotate, const int is_left, Gameobject* pearent = nullptr);


		Gameobject* set_player_statue(const Vector3& pos, const float& scale, const Vector3& rotate, Gameobject* pearent = nullptr);

		Gameobject* set_desk(const Vector3& pos, const Vector3& scale, const Vector3& rotate, float mass);
		Gameobject* set_gear(const Vector3& pos, const Vector3& scale, const Vector3& rotate, float mass, int tooth_count);
		Gameobject* set_sphere_rope(const Vector3& pos, const float& scale, const Vector3& rotate, int quantity, float mass, const Vector3& color = Vector3(1, 1, 1));



		Gameobject* set_buttan(const Vector3& pos, const Vector3& scale, const Vector3& rotate, Stage_parts::Stageparts_tagbit tag, Gameobject* pearent = nullptr, bool is_use_trigger = false);
		Gameobject* set_gimmickdoor(
			const Vector3& start_pos, const Vector3& goal_pos,
			const Vector3& start_rot, const Vector3& goal_rot,
			float pos_speed, float rot_speed,
			const Vector3& scale,
			Stage_parts::Stageparts_tagbit tag,
			const Vector3& color = Vector3(1, 1, 1),
			Gameobject* pearent = nullptr
		);
		Gameobject* set_Andcircuit(Stage_parts::Stageparts_tagbit in_tag0, Stage_parts::Stageparts_tagbit in_tag1, Stage_parts::Stageparts_tagbit out_tag, Gameobject* pearent = nullptr);

	public:
		// stage_managerが呼ぶawake このstageに切り替わったときに呼ばれる
		void stage_awake() override;

		// stage_managerが呼ぶdestroy このstageから次に切り替わったときに呼ばれる
		void stage_destroy() override;

	public:

		void summon_block_01() override;

	public:
		// 毎フレーム呼ばれる更新処理
		void update()override;

		void Update_hierarchy() override;
	};
}