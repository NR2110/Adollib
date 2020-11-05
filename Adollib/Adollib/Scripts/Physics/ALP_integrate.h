#pragma once
#include <memory>
#include "../Scene/scene.h"
#include "ALP_collider.h"
namespace Adollib {
	namespace physics_function {

		//::: ALP_collidersから呼ぶ ::::::::::

		// gameobjectのtransformからcolliderのworld空間での情報を更新
		void update_world_trans(std::list<physics_function::ALP_Collider>& ALP_colliders);

		//gameobjectへの変化量を求める
		void solv_resolve(std::list<physics_function::ALP_Collider>& ALP_colliders);

		//gameobjectへ変化量を渡す
		void resolve_gameobject(std::list<physics_function::ALP_Collider>& ALP_colliders);

		// k-DOPの更新
		void update_dop14(std::list<physics_function::ALP_Collider>& ALP_shapes);

		//::: ALP_phyicsesから呼ぶ ::::::::::

		// 速度、加速度を0にする
		void resetforce(std::list<physics_function::ALP_Physics>& ALP_physics);

		// 外力による速度などの更新
		void applyexternalforce(std::list<physics_function::ALP_Physics>& ALP_physics);

		// 座標,姿勢の更新
		void integrate(std::list<physics_function::ALP_Physics>& ALP_physics);


	}
}