#include "collider_box.h"

#include "gameobject.h"
#include "Adollib.h"
#include "contact.h"


using namespace Adollib;
using namespace Contacts;
using namespace DOP;

void Plane::update_world_trans() {
	world_orientation = gameobject->get_world_orientate() * local_orientation;
	world_scale = gameobject->get_world_scale() * local_scale;
	world_position = gameobject->get_world_position() + vector3_quatrotate(local_position * world_scale, world_orientation);

	update_inertial(world_scale, density);
}
void Plane::update_dop14() {
	dop14.pos = gameobject->get_world_position();
	for (int i = 0; i < DOP_size; i++) {
		dop14.max[i] = FLT_MAX;
	}
}