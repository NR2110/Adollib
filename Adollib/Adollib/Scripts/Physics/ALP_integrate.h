#pragma once
#include <list>
#include <unordered_map>

#include "../Scene/scene_list.h"

namespace Adollib {
	namespace Physics_function {
		class ALP_Joint;
		class ALP_Collider;
		class ALP_Physics;

		//::: ALP_Joint ::::::::
		void adapt_joint_limit(std::list<Physics_function::ALP_Joint*>& ALP_Joints);


		//::: ALP_colliders ::::::::::
		// gameobjectのtransformからcolliderのworld空間での情報を更新
		void update_world_trans(const std::list<Scenelist>& active_scenes,
			std::unordered_map<Scenelist, std::list<Physics_function::ALP_Collider*>>& ALP_colliders
		);

		// calculateが行われるたび更新する情報に
		void update_per_calculate(const std::list<Scenelist>& active_scenes,
			std::unordered_map<Scenelist, std::list<Physics_function::ALP_Collider*>>& ALP_colliders
		);



		//::: ALP_phyicses ::::::::::

		// 速度、加速度を0にする
		void resetforce(const std::list<Scenelist>& active_scenes,
			std::unordered_map<Scenelist, std::list<Physics_function::ALP_Physics*>>& ALP_physics
		);

		// 外力による速度などの更新
		void applyexternalforce(
			const std::list<Scenelist>& active_scenes,
			std::unordered_map<Scenelist, std::list<Physics_function::ALP_Physics*>>& ALP_physics,
			const float timeratio_60, const float time_scale
		);

		// 座標,姿勢の更新
		void integrate(
			const std::list<Scenelist>& active_scenes,
			std::unordered_map<Scenelist, std::list<Physics_function::ALP_Physics*>>& ALP_physics
		);


		//::: どっちも ::::
		// gameobjectが更新したworld_trnsformの更新
		void copy_transform(
			const std::list<Scenelist>& active_scenes,
			std::unordered_map<Scenelist, std::list<Physics_function::ALP_Collider*>>& ALP_colliders,
			std::unordered_map<Scenelist, std::list<Physics_function::ALP_Physics*>>& ALP_physics
		);

		// collider::componentのtagやphysics_dataを更新する
		void adapt_component_data(
			const std::list<Scenelist>& active_scenes,
			std::unordered_map<Scenelist, std::list<Physics_function::ALP_Collider*>>& ALP_colliders,
			std::unordered_map<Scenelist, std::list<Physics_function::ALP_Physics*>>& ALP_physics,
			std::list<Physics_function::ALP_Joint*>& ALP_joints
		);

		//void adapt_added_data(std::list<Physics_function::ALP_Collider*>& ALP_colliders, std::list<Physics_function::ALP_Physics*>& ALP_physics, std::list<Physics_function::ALP_Joint*>& ALP_joints);

	}
}