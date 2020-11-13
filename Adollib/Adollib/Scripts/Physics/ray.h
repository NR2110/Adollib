#pragma once
#include "../Math/math.h"

#include "collider__base.h"

namespace Adollib {

	class Ray {
	public:
		Vector3 position;
		Vector3 direction;

	public:
		bool ray_cast(u_int tag, Collider* coll);
		bool ray_cast(Collider* coll);

	};

}