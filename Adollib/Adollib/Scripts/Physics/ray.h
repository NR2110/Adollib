#pragma once
#include "../Math/math.h"

#include "collider__base.h"

namespace Adollib {

	class Ray {
	public:
		Vector3 position;
		Vector3 direction;

	public:
		bool ray_cast(u_int tag, float& tmin, float& tmax, Vector3& normal, Collider* coll = nullptr);

		bool ray_cast(float& tmin, float& tmax, Collider* coll = nullptr) {
			Vector3 normal;
			return ray_cast(UINT_MAX, tmin, tmax, normal, coll);
		}

		bool ray_cast() {
			float tmin = 0, tmax = 0;
			Vector3 normal;
			Collider* coll;
			return ray_cast(UINT_MAX, tmin, tmax, normal, coll);
		}


	};

}