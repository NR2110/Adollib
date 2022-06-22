#pragma once
#include <list>
#include <unordered_map>

namespace Adollib {
	namespace Physics_function {
		class ALP_Joint;
		class ALP_Collider;
		class ALP_Physics;

		//::: ALP_Joint ::::::::
		void apply_joint_velocityeffect(std::list<Physics_function::ALP_Joint*>& ALP_Joints);


		//::: ALP_colliders ::::::::::
		// gameobjectのtransformからcolliderのworld空間での情報を更新
		void update_world_trans(std::list<Physics_function::ALP_Collider*>& ALP_colliders);

		// calculateが行われるたび更新する情報に
		void update_per_calculate(std::list<Physics_function::ALP_Collider*>& ALP_colliders);



		//::: ALP_phyicses ::::::::::

		// 速度、加速度を0にする
		void resetforce(std::list<Physics_function::ALP_Physics*>& ALP_physics);

		// 外力による速度などの更新
		void applyexternalforce(
			std::list<Physics_function::ALP_Physics*>& ALP_physics,
			const float timeratio_60, const float time_scale
		);

		// 座標,姿勢の更新
		void integrate(std::list<Physics_function::ALP_Physics*>& ALP_physics);


		//::: どっちも ::::
		// gameobjectが更新したworld_trnsformの更新
		void copy_transform(
			std::list<Physics_function::ALP_Collider*>& ALP_colliders,
			std::list<Physics_function::ALP_Physics*>& ALP_physics
		);

		// collider::componentのtagやphysics_dataを更新する
		void adapt_component_data(
			std::list<Physics_function::ALP_Collider*>& ALP_colliders,
			std::list<Physics_function::ALP_Physics*>& ALP_physics,
			std::list<Physics_function::ALP_Joint*>& ALP_joints
		);

		//void adapt_added_data(std::list<Physics_function::ALP_Collider*>& ALP_colliders, std::list<Physics_function::ALP_Physics*>& ALP_physics, std::list<Physics_function::ALP_Joint*>& ALP_joints);

	}
}