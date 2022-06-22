#include "ray.h"

#include "ALP__physics_manager.h"

using namespace Adollib;
using namespace Physics_function;


bool Ray::ray_cast(Raycast_struct& str) {
	return Physics_manager::ray_cast(position, direction, str.collider_tag, str.ray_offset, str.raymin, str.raymax, str.normal, str.coll);
}

bool Ray::sphere_cast(const float& radius, Vector3& contact_point, Raycast_struct& str) {
	return Physics_manager::sphere_cast(position, direction, radius, contact_point, str.collider_tag, str.ray_offset, str.raymin, str.raymax, str.normal, str.coll);
}