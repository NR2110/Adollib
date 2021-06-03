#pragma once
#include <memory>
#include "../Scene/scene.h"
#include "ALP_collider.h"
#include "collider_shape.h"


namespace Adollib {
	namespace Physics_function {


		//::: ALP_colliders ::::::::::

		// gameobjectのtransformからcolliderのworld空間での情報を更新
		void update_world_trans(std::list<Physics_function::ALP_Collider*>& ALP_colliders);

		// gameobjectへの変化量を求める
		void solv_resolve(std::list<Physics_function::ALP_Collider*>& ALP_colliders);

		// gameobjectへ変化量を渡す
		void resolve_gameobject(std::list<Physics_function::ALP_Collider*>& ALP_colliders);

		// k-DOPの更新
		void update_dop14(std::list<Physics_function::ALP_Collider*>& ALP_shapes);

		// oncoll_bitsのリセット
		void reset_oncoll_bits(std::list<Physics_function::ALP_Collider*>& ALP_colliders);

		//::: ALP_phyicses ::::::::::

		// 速度、加速度を0にする
		void resetforce(std::list<Physics_function::ALP_Physics*>& ALP_physics);

		// 外力による速度などの更新
		void applyexternalforce(std::list<Physics_function::ALP_Physics*>& ALP_physics);

		// 座標,姿勢の更新
		void integrate(std::list<Physics_function::ALP_Physics*>& ALP_physics);


	}
}