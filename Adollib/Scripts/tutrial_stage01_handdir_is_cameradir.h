//#pragma once
//
//#include "../Adollib/Scripts/Object/gameobject_manager.h"
//#include "../Adollib/Scripts/Object/component.h"
//
//#include  "tutrial_base.h"
//
//namespace Adollib {
//
//	class Material;
//	class Input_changer;
//
//	class Tutrial_stage01_handdir_is_cameradir : public Tutrial_base {
//
//	private:
//		std::shared_ptr<Material> mat_tutrial_check = nullptr;
//		std::shared_ptr<Material> mat_tutrial_handdir = nullptr;
//
//		Gameobject* go_tutrial_handdir = nullptr;
//
//		Vector3 check_base_scale;
//		int tutrial_flag = -1;
//		float tutrial_timer = 0;
//		float tutrial_move_state_timer = 0;
//		float tutrial_camera_state_timer = 0;
//		float tutrial_move_check_animation_timer = 0;
//		float tutrial_camera_check_animation_timer = 0;
//
//
//	public:
//
//		void set_tutrial_move_and_camera() {
//			tutrial_flag = 0;
//		};
//
//
//	private:
//		void tutrial_handdir(); //1 ~ 4
//
//
//
//	public:
//
//		void awake();
//
//		// 毎フレーム呼ばれる更新処理
//		void update();
//
//
//	};
//
//
//}