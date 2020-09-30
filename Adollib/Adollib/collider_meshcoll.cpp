#include "collider_meshcoll.h"

#include "gameobject.h"
#include "Adollib.h"
#include "contact.h"


using namespace Adollib;
using namespace Contacts;
using namespace DOP;

void Meshcoll::update_world_trans() {
	world_orientation = gameobject->get_world_orientate() * local_orientation;
	world_scale = gameobject->get_world_scale() * local_scale;
	world_position = gameobject->get_world_position() + vector3_quatrotate((local_position + offset) * gameobject->get_world_scale(), world_orientation);
	update_inertial(world_scale, density);
}
void Meshcoll::update_dop14() {
	dop14.pos = gameobject->get_world_position() + local_position;

	vector3 rotated_axis[DOP_size];
	for (int i = 0; i < DOP_size; i++) {
		rotated_axis[i] = vector3_quatrotate(DOP_14_axis[i], gameobject->get_world_orientate().conjugate()).unit_vect();
		dop14.max[i] = -FLT_MAX;
		dop14.min[i] = +FLT_MAX;
	}

	vector3 half[DOP_size * 2];
	int sum = 0;
	for (int i = 0; i < DOP_size; i++) {
		half[sum] = dopbase.max[i] * DOP_14_axis[i] * world_scale;
		sum++;
		half[sum] = dopbase.min[i] * DOP_14_axis[i] * world_scale;
		sum++;
	}

	//DOPの更新
	for (int i = 0; i < DOP_size; i++) {
		for (int o = 0; o < DOP_size * 2; o++) {
			float dis = vector3_dot(rotated_axis[i], half[o]);
			if (dop14.max[i] < dis) dop14.max[i] = dis * 1.00000001f;//確実にするためちょっと大きめにとる
			if (dop14.min[i] > dis) dop14.min[i] = dis * 1.00000001f;//確実にするためちょっと大きめにとる

		}
	}

	half_size = vector3(dop14.max[0] - dop14.min[0], dop14.max[1] - dop14.min[1], dop14.max[2] - dop14.min[2]) / 2.0f;

}