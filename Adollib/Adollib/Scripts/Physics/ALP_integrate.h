#pragma once
#include <list>

namespace Adollib {
	namespace Physics_function {
		class ALP_Joint;
		class ALP_Collider;
		class ALP_Physics;

		//::: ALP_Joint ::::::::
		void adapt_joint_limit(std::list<Physics_function::ALP_Joint*>& ALP_Joints);


		//::: ALP_colliders ::::::::::
		// gameobjectのtransformからcolliderのworld空間での情報を更新
		void update_world_trans(std::list<Physics_function::ALP_Collider*>& ALP_colliders);

		// k-DOPの更新 update_world_transでおこなっている
		//void update_dop(std::list<Physics_function::ALP_Collider*>& ALP_shapes);



		//::: ALP_phyicses ::::::::::

		// 速度、加速度を0にする
		void resetforce(std::list<Physics_function::ALP_Physics*>& ALP_physics);

		// 外力による速度などの更新
		void applyexternalforce(std::list<Physics_function::ALP_Physics*>& ALP_physics, const float timeratio_60);

		// 座標,姿勢の更新
		void integrate(std::list<Physics_function::ALP_Physics*>& ALP_physics);


		//::: どっちも ::::
		// oncoll_bitsやtrnsformのリセット
		void reset_data_per_frame(std::list<Physics_function::ALP_Collider*>& ALP_colliders, std::list<Physics_function::ALP_Physics*>& ALP_physics);

		// collider::componentのtagやphysics_dataを更新する
		void adapt_collider_component_data(std::list<Physics_function::ALP_Collider*>& ALP_colliders, std::list<Physics_function::ALP_Physics*>& ALP_physics);

		//void adapt_added_data(std::list<Physics_function::ALP_Collider*>& ALP_colliders, std::list<Physics_function::ALP_Physics*>& ALP_physics, std::list<Physics_function::ALP_Joint*>& ALP_joints);

	}
}