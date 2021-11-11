#pragma once
#include "../Math/math.h"
#include "ALP_collider.h"

namespace Adollib {
	namespace Physics_function {

		//::::::
		// ray_pos : rayの始点
		// ray_dir : rayの向き
		// ray_min : rayの始点の距離
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

		//::::::
		// ray_pos : rayの始点
		// ray_dir : rayの向き
		// radius  : 球の半径
		// ray_min : rayの始点の距離
		// ----out----
		// contact_point : 衝突した点
		// coll : 衝突したcolliderへのポインタ
		// tmin から tmax
		// normal : 衝突した面のnormal
		bool sphere_cast(const Vector3& Ray_pos, const Vector3& Ray_dir, const float& radius, Vector3& contact_point,
			const float ray_min,
			const ALP_Collider* coll,
			float& tmin, float& tmax,
			Vector3& normal
		);

	}
}