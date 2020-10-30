#pragma once
#include <unordered_map>
#include <string>
#include "../Math/math.h"
#include "../Object/gameobject_tags.h"

namespace Adollib {
	namespace physics_function {

		class Collider {

			Vector3 local_position;
			Vector3 local_orientation;
			Vector3 local_scale;

			Vector3	world_position;
			Vector3	world_prientation;
			Vector3	world_scale;

			Quaternion offset_CollGO_quat;
			Vector3 offset_CollGO_pos;

			//itr
			//	itr
			std::unordered_map<GO_Tag, bool>concollflags

		};
	}
}
