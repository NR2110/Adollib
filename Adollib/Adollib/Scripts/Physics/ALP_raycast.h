#pragma once
#include "../Math/math.h"
#include "ALP_collider.h"

namespace Adollib {
	namespace Physics_function {

		//::::::
		// ray_pos : rayの始点
		// ray_dir : rayの向き
		// ray_min :
		// ----out----
		// coll : 衝突したcolliderへのポインタ
		// tmin から tmax
		// normal : 衝突した面のnormal
		bool ray_cast(const Vector3& Ray_pos, const Vector3& Ray_dir,
			const float ray_min,
			const ALP_Collider* coll,
			float& tmin, float& tmax,
			Vector3& normal
			);

	}
}