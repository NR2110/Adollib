#include "ray.h"

#include "ALP__physics_manager.h"

using namespace Adollib;
using namespace Physics_function;

bool Ray::ray_cast(u_int tag, const float ray_min, float& min, float& max, Vector3& normal, Collider* coll) {
	return Physics_manager::ray_cast(position, direction, tag, ray_min, min, max, normal, coll);
}