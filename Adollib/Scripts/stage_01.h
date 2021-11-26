#pragma once
#include "stage_base.h"

namespace Adollib {

	class Stage_01 : public Stage_base {


	private:

		// 木を作る
		Gameobject* set_tree(const Vector3& pos, const Vector3& scale, const Vector3& rotate, const float y_scale = 1, const float tan_scale = 1, Gameobject* pearent = nullptr);
		// 柱を作る
		Gameobject* set_pillar(const Vector3& pos, const Vector3& scale, const Vector3& rotate, Gameobject* pearent = nullptr, const float base_length = 0.5f, const float pillar_length = 7);
		// 柵を作る
		Gameobject* set_fence(const Vector3& pos, const Vector3& scale, const Vector3& rotate, Gameobject* pearent = nullptr, const int fence_count = 1);
		// ドアを作る
		Gameobject* set_door(const Vector3& pos, const Vector3& scale, const Vector3& rotate, const int is_left, Gameobject* pearent = nullptr);



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
		// 毎フレーム呼ばれる更新処理
		void update()override;

		void Update_hierarchy() override;
	};
}