#pragma once
#include <unordered_map>
#include <string>
#include "../Math/math.h"

namespace Adollib {
	namespace physics_function {

		class Physics {
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

		private:
			float speed;//debugしやすいように
			Vector3 linear_velocity;//並進速度
			Vector3 anglar_velocity; //回転速度
			Matrix inertia_tensor;//慣性テンソル

			bool sleep_state; //sleep状態かのflag

		};
	}
}
