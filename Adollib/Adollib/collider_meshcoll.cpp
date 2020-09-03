#include "collider_meshcoll.h"

#include "gameobject.h"
#include "Adollib.h"
#include "contact.h"


using namespace Adollib;
using namespace Contacts;

void Meshcoll::update_world_trans() {
	world_orientation = gameobject->get_world_orientate() * local_orientation;
	world_size = gameobject->get_world_scale() * local_scale * half_size;
	world_position = gameobject->get_world_position() + vector3_be_rotated_by_quaternion((local_position + offset) * gameobject->get_world_scale(), world_orientation);
	update_inertial(world_size, density);
}
void Meshcoll::update_dop14() {
	dop14.pos = gameobject->get_world_position() + local_position;

	quaternion WO = gameobject->get_world_orientate();

	vector3 rotated_axis[DOP_size];
	for (int i = 0; i < DOP_size; i++) {
		rotated_axis[i] = vector3_be_rotated_by_quaternion(DOP_14_axis[i], WO);
	}

	vector3 half[DOP_size * 2];
	int sum = 0;
	for (int i = 0; i < DOP_size; i++) {
		half[sum] = dopbase.max[i] * DOP_14_axis[i];
		sum++;
		half[sum] = dopbase.min[i] * DOP_14_axis[i];
		sum++;
	}

	//DOPの更新
	float max_len = 0;
	for (int i = 0; i < DOP_size; i++) {
		max_len = 0;
		for (int o = 0; o < DOP_size; o++) {
			float dis = fabsf(vector3_dot(DOP_14_axis[i], half[o]));
			if (max_len < dis) {
				dop14.max[i] = +dis * 1.00000001f;//確実にするためちょっと大きめにとる
				dop14.min[i] = -dis * 1.00000001f;//確実にするためちょっと大きめにとる
				max_len = dis;
			}

		}
	}

}