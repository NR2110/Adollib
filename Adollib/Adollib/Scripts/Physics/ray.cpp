#include "ray.h"

#include "ALP__physics_manager.h"

using namespace Adollib;
using namespace Physics_function;

bool Ray::ray_cast(u_int tag, float& min, float& max, Vector3& normal, Collider* coll) {
	return Phyisics_manager::ray_cast(position, direction, tag, min, max, normal, coll);
}