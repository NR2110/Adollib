#pragma once
#include "../Math/math.h"
#include "ALP_collider.h"

namespace Adollib {
	namespace Physics_function {

		bool ray_cast(const Vector3& Ray_pos, const Vector3& Ray_dir,
			const ALP_Collider& coll,
			const float ray_min,
			float& tmin, float& tmax,
			Vector3& normal
			);

	}
}