#pragma once
#include "../Math/math.h"

#include "collider__base.h"

namespace Adollib {

	class Collider;

	class Ray {
	public:
		Vector3 position;
		Vector3 direction;

	public:
		bool ray_cast(u_int tag,
			const float ray_min,
			float& tmin, float& tmax, Vector3& normal, Collider* coll = nullptr);

		bool ray_cast(float ray_min, float& tmin, float& tmax, Collider* coll = nullptr) {
			Vector3 normal;
			return ray_cast(UINT_MAX, ray_min, tmin, tmax, normal, coll);
		}

		bool ray_cast() {
			float tmin = 0, tmax = 0;
			float ray_min = -FLT_MAX;
			Vector3 normal;
			Collider* coll = nullptr;
			return ray_cast(UINT_MAX, ray_min, tmin, tmax, normal, coll);
		}


	};

}