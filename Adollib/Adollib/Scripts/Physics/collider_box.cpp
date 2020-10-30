#include "collider_box.h"

#include "../Object/gameobject.h"
#include "../Main/Adollib.h"
#include "contact.h"


using namespace Adollib;
using namespace Contacts;
using namespace DOP;

void Box::update_world_trans() {
	world_orientation = gameobject->get_world_orientate() * local_orientation;
	world_scale = gameobject->get_world_scale() * local_scale;
	world_position = gameobject->get_world_position() + vector3_quatrotate(local_position * gameobject->get_world_scale(), world_orientation);
	
	update_inertial(half_size * world_scale, density);
}
void Box::update_dop14() {
	dop14.pos = gameobject->get_world_position() + local_position;

	//各頂点のローカル座標
	Vector3 half[4]{
	{+half_size.x, -half_size.y, -half_size.z},
	{+half_size.x, -half_size.y, +half_size.z},
	{+half_size.x, +half_size.y, -half_size.z},
	{+half_size.x, +half_size.y, +half_size.z}
	};

	for (int i = 0; i < 4; i++) {
		half[i] = half[i] * world_scale;
	}

	Quaternion WO = gameobject->get_world_orientate();
	for (int i = 0; i < 4; i++) {
		half[i] = vector3_quatrotate(half[i], WO);
	}

	//DOPの更新
	float max_len = 0;
	for (int i = 0; i < DOP_size; i++) {
		max_len = 0;
		for (int o = 0; o < 4; o++) {
			float dis = fabsf(vector3_dot(DOP_14_axis[i], half[o]));
			if (max_len < dis) {
				dop14.max[i] = +dis * 1.00000001f;//確実にするためちょっと大きめにとる
				dop14.min[i] = -dis * 1.00000001f;//確実にするためちょっと大きめにとる
				max_len = dis;
			}

		}
	}

}