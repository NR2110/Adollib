#include "collider_box.h"

#include "gameobject.h"
#include "Adollib.h"
#include "contact.h"


using namespace Adollib;
using namespace Contacts;
using namespace DOP;

void Sphere::update_world_trans() {
	world_orientation = gameobject->get_world_orientate() * local_orientation;
	world_size = gameobject->get_world_scale() * local_scale * r;
	world_position = gameobject->get_world_position() + vector3_be_rotated_by_quaternion(local_position * world_size, world_orientation);
	update_inertial(world_size, density);
}
void Sphere::update_dop14() {
	dop14.pos = gameobject->get_world_position();
	for (int i = 0; i < DOP_size; i++) {
		dop14.max[i] = +r * 1.0000001 * world_size.x;
		dop14.min[i] = -r * 1.0000001 * world_size.x;
	}
}