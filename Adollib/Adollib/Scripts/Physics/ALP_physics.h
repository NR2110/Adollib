#pragma once
#include <unordered_map>
#include <string>
#include "../Math/math.h"

namespace Adollib {
	namespace physics_function {

		class ALP_Physics {
		public:
			float mass; //質量
			float drag; //空気抵抗
			float anglar_drag; //空気抵抗
			float dynamic_friction;//動摩擦
			float static_friction; //静摩擦
			float restitution;	 //反発係数

			bool is_fallable; //落ちない
			bool is_kinematic; //影響うけない(fallはする)
			bool is_moveable;//動かない

			float speed;//debugしやすいように
			Vector3 linear_velocity;//並進速度
			Vector3 anglar_velocity; //回転速度
			Matrix inertia_tensor;//慣性テンソル

			bool sleep_state; //sleep状態かのflag

		public:
			//外力の更新
			void apply_external_force(float duration = 1);

			//速度、加速度を0にする
			void reset_force();

			//並進移動に力を加える
			void add_force(const Vector3& force);

			//角回転に力を加える
			void add_torque(const Vector3& force);

			//可動オブジェクトかどうか
			bool is_movable() const;

			//落下オブジェクトかどうか
			bool is_fallable() const;

			//質量の逆数を返す(不稼働オブジェクトは0を返す)
			float inverse_mass() const;

			//慣性モーメントの逆行列を返す
			Matrix inverse_inertial_tensor() const;

		};
	}
}
