#pragma once

#include "../Adollib/Scripts/Object/gameobject_manager.h"
#include "../Adollib/Scripts/Object/component.h"

#include  "tutrial_base.h"

namespace Adollib {

	class Material;
	class Input_changer;

	class Tutrial_stage01_move_camera_catch : public Tutrial_base {

	private:
		std::shared_ptr<Material> mat_tutrial_check = nullptr;
		std::shared_ptr<Material> mat_tutrial_move = nullptr;
		std::shared_ptr<Material> mat_tutrial_camera = nullptr;
		std::shared_ptr<Material> mat_tutrial_hand = nullptr;
		std::shared_ptr<Material> mat_tutrial_catch_object = nullptr;

		Gameobject* go_tutrial_move = nullptr;
		Gameobject* go_tutrial_camera = nullptr;
		Gameobject* go_tutrial_hand = nullptr;
		Gameobject* go_tutrial_catch_object = nullptr;

		Vector3 check_base_scale;
		int tutrial_flag = -1;
		float tutrial_timer = 0;
		float tutrial_move_state_timer = 0;
		float tutrial_camera_state_timer = 0;
		float tutrial_move_check_animation_timer = 0;
		float tutrial_camera_check_animation_timer = 0;


	public:

		void set_tutrial_move_and_camera() {
			tutrial_flag = 0;
		};


	private:
		void tutrial_move_and_camera(); //1 ~ 4

		void tutrial_hand_stretch(); //5 ~ 9

		void tutrial_hand_catch(); //10 ~



	public:

		void awake();

		// 毎フレーム呼ばれる更新処理
		void update();


	};


}