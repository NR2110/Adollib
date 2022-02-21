#pragma once

#include "../Adollib/Scripts/Object/gameobject_manager.h"
#include "../Adollib/Scripts/Object/component.h"

#include  "tutrial_base.h"

namespace Adollib {

	class Material;
	class Input_changer;

	class Tutrial_stage02_rope : public Tutrial_base {

	private:
		std::shared_ptr<Material> mat_tutrial_check = nullptr;
		std::shared_ptr<Material> mat_tutrial_aim     = nullptr;
		std::shared_ptr<Material> mat_tutrial_shot   = nullptr;
		std::shared_ptr<Material> mat_tutrial_shrink = nullptr;
		std::shared_ptr<Material> mat_tutrial_cut    = nullptr;

		Gameobject* go_tutrial_aim = nullptr;
		Gameobject* go_tutrial_shot = nullptr;
		Gameobject* go_tutrial_shrink = nullptr;
		Gameobject* go_tutrial_cut = nullptr;

		Vector3 check_base_scale;
		int tutrial_flag = -1;
		float tutrial_timer = 0;
		float tutrial_aim_state_timer = 0;
		float tutrial_shot_state_timer = 0;
		float tutrial_move_check_animation_timer = 0;
		float tutrial_camera_check_animation_timer = 0;


	public:


	private:
		void tutrial_aim_and_shot(); //1 ~ 4
		void tutrial_shrink(); //1 ~ 4
		void tutrial_cut(); //1 ~ 4


	public:

		void awake();

		// 毎フレーム呼ばれる更新処理
		void update();


	};


}