#pragma once
#include "stage_base.h"

namespace Adollib {

	class Stage_01 : public Stage_base {


	private:

		Gameobject* set_tree(const Vector3& pos, const Vector3& scale, const Vector3& rotate);
		Gameobject* set_desk(const Vector3& pos, const Vector3& scale, const Vector3& rotate, float mass);
		Gameobject* set_gear(const Vector3& pos, const Vector3& scale, const Vector3& rotate, float mass, int tooth_count);



		Gameobject* set_buttan(const Vector3& pos, const Vector3& scale, const Vector3& rotate, Stage_parts::Stageparts_tagbit tag, const Vector3& color = Vector3(1, 1, 1));
		Gameobject* set_door(
			const Vector3& start_pos, const Vector3& goal_pos,
			const Vector3& start_rot, const Vector3& goal_rot,
			float pos_speed, float rot_speed,
			const Vector3& scale,
			Stage_parts::Stageparts_tagbit tag,
			const Vector3& color = Vector3(1, 1, 1)
		);
		Gameobject* set_sphere_rope(const Vector3& pos, const float& scale, const Vector3& rotate, int quantity, float mass, const Vector3& color = Vector3(1, 1, 1));

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