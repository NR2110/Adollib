#pragma once
#include "stage_base.h"

namespace Adollib {

	// tutrial用
	//class Player_manager;

	class Box;
	class Gameobject;

	class Stage_99 : public Stage_base {

	private:


	private:

		const Vector3 base_color = Vector3(233, 225, 217) / 255.0f * 0.9f;
		const Vector3 grass_color = Vector3(188, 214, 54) / 255.0f * 0.7f * 0.9f;
		const Vector3 wood_color = Vector3(90, 47, 27) / 255.0f * 0.9f;
		const Vector3 fence_color = Vector3(164, 107, 81) / 255.0f;
		const Vector3 stair_color = Vector3(215, 205, 197) / 255.0f * 0.9f;
		const Vector3 Yellow_color = Vector3(253, 255, 0) / 255.0f * 0.9f;

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
		// hit event
		Gameobject* set_event(const Vector3& pos, Gameobject* pearent = nullptr);


		Gameobject* set_player_statue(const Vector3& pos, const float& scale, const Vector3& rotate, Gameobject* pearent = nullptr);





		Gameobject* set_buttan(const Vector3& pos, const Vector3& scale, const Vector3& rotate, Stage_parts::Stageparts_tagbit tag, Gameobject* pearent = nullptr, bool is_use_trigger = false);
		Gameobject* set_lever(const Vector3& pos, const Vector3& scale, const Vector3& rotate,
			Stage_parts::Stageparts_tagbit left_tag, Stage_parts::Stageparts_tagbit right_tag,
			Gameobject* pearent = nullptr);
		Gameobject* set_gimmickdoor(
			const Vector3& start_pos, const Vector3& goal_pos,
			const Vector3& start_rot, const Vector3& goal_rot,
			float pos_speed, float rot_speed,
			const Vector3& scale,
			Stage_parts::Stageparts_tagbit tag,
			const Vector3& color = Vector3(1, 1, 1),
			Gameobject* pearent = nullptr
		);
		Collider* set_move_block_2flags(
			const Vector3& left_pos, const Vector3& right_pos,
			const Vector3& scale, const Vector3& rotate,
			float time_pow,
			float now_time,
			Stage_parts::Stageparts_tagbit left_tag,
			Stage_parts::Stageparts_tagbit right_tag,
			const Vector3& color = Vector3(1, 1, 1),
			Gameobject* pearent = nullptr
		);
		Gameobject* set_Andcircuit(Stage_parts::Stageparts_tagbit in_tag0, Stage_parts::Stageparts_tagbit in_tag1, Stage_parts::Stageparts_tagbit out_tag, Gameobject* pearent = nullptr);


	private:
		Gameobject* create_alphabet_GO(Collider*& coll, const std::string& name, const Vector3& pos, const Vector3& scale, const Vector3& rotate, Gameobject* pearent, float color_timr_start = 0);
		Box* create_alphabet_box(const Vector3& pos, const Vector3& scale, const Vector3& rotate, Collider* coll);

		Gameobject* set_T(const Vector3& pos, const Vector3& scale, const Vector3& rotate, const Vector3& color = Vector3(1, 1, 1), Gameobject* pearent = nullptr, float color_timr_start = 0);
		Gameobject* set_H(const Vector3& pos, const Vector3& scale, const Vector3& rotate, const Vector3& color = Vector3(1, 1, 1), Gameobject* pearent = nullptr, float color_timr_start = 0);
		Gameobject* set_A(const Vector3& pos, const Vector3& scale, const Vector3& rotate, const Vector3& color = Vector3(1, 1, 1), Gameobject* pearent = nullptr, float color_timr_start = 0);
		Gameobject* set_N(const Vector3& pos, const Vector3& scale, const Vector3& rotate, const Vector3& color = Vector3(1, 1, 1), Gameobject* pearent = nullptr, float color_timr_start = 0);
		Gameobject* set_K(const Vector3& pos, const Vector3& scale, const Vector3& rotate, const Vector3& color = Vector3(1, 1, 1), Gameobject* pearent = nullptr, float color_timr_start = 0);

		Gameobject* set_Y(const Vector3& pos, const Vector3& scale, const Vector3& rotate, const Vector3& color = Vector3(1, 1, 1), Gameobject* pearent = nullptr, float color_timr_start = 0);
		Gameobject* set_O(const Vector3& pos, const Vector3& scale, const Vector3& rotate, const Vector3& color = Vector3(1, 1, 1), Gameobject* pearent = nullptr, float color_timr_start = 0);
		Gameobject* set_U(const Vector3& pos, const Vector3& scale, const Vector3& rotate, const Vector3& color = Vector3(1, 1, 1), Gameobject* pearent = nullptr, float color_timr_start = 0);

		Gameobject* set_F(const Vector3& pos, const Vector3& scale, const Vector3& rotate, const Vector3& color = Vector3(1, 1, 1), Gameobject* pearent = nullptr, float color_timr_start = 0);
		//Gameobject* set_O(const Vector3& pos, const Vector3& scale, const Vector3& rotate, const Vector3& color = Vector3(1, 1, 1), Gameobject* pearent = nullptr, float color_timr_start = 0);
		Gameobject* set_R(const Vector3& pos, const Vector3& scale, const Vector3& rotate, const Vector3& color = Vector3(1, 1, 1), Gameobject* pearent = nullptr, float color_timr_start = 0);

		Gameobject* set_P(const Vector3& pos, const Vector3& scale, const Vector3& rotate, const Vector3& color = Vector3(1, 1, 1), Gameobject* pearent = nullptr, float color_timr_start = 0);
		Gameobject* set_L(const Vector3& pos, const Vector3& scale, const Vector3& rotate, const Vector3& color = Vector3(1, 1, 1), Gameobject* pearent = nullptr, float color_timr_start = 0);
		//Gameobject* set_A(const Vector3& pos, const Vector3& scale, const Vector3& rotate, const Vector3& color = Vector3(1, 1, 1), Gameobject* pearent = nullptr, float color_timr_start = 0);
		//Gameobject* set_Y(const Vector3& pos, const Vector3& scale, const Vector3& rotate, const Vector3& color = Vector3(1, 1, 1), Gameobject* pearent = nullptr, float color_timr_start = 0);
		Gameobject* set_I(const Vector3& pos, const Vector3& scale, const Vector3& rotate, const Vector3& color = Vector3(1, 1, 1), Gameobject* pearent = nullptr, float color_timr_start = 0);
		//Gameobject* set_N(const Vector3& pos, const Vector3& scale, const Vector3& rotate, const Vector3& color = Vector3(1, 1, 1), Gameobject* pearent = nullptr, float color_timr_start = 0);
		Gameobject* set_G(const Vector3& pos, const Vector3& scale, const Vector3& rotate, const Vector3& color = Vector3(1, 1, 1), Gameobject* pearent = nullptr, float color_timr_start = 0);



	public:
		// stage_managerが呼ぶawake このstageに切り替わったときに呼ばれる
		void stage_awake() override;

		// stage_managerが呼ぶdestroy このstageから次に切り替わったときに呼ばれる
		void stage_destroy() override;

	public:

	public:
		// 毎フレーム呼ばれる更新処理
		void update()override;

		void Update_hierarchy() override;
	};
}