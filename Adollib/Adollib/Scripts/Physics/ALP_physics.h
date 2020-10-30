#pragma once
#include <unordered_map>
#include <string>
#include "../Math/math.h"

namespace Adollib {
	namespace physics_function {

		class Physics {
		public:
			float mass; //����
			float drag; //��C��R
			float anglar_drag; //��C��R
			float dynamic_friction;//�����C
			float static_friction; //�Ö��C
			float restitution;	 //�����W��

			bool is_fallable; //�����Ȃ�
			bool is_kinematic; //�e�������Ȃ�(fall�͂���)
			bool is_moveable;//�����Ȃ�

		private:
			float speed;//debug���₷���悤��
			Vector3 linear_velocity;//���i���x
			Vector3 anglar_velocity; //��]���x
			Matrix inertia_tensor;//�����e���\��

			bool sleep_state; //sleep��Ԃ���flag

		};
	}
}
