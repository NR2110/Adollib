#include "ALP_shape.h"

//#include "collider__base.h"
//#include "../Object/gameobject.h"

using namespace Adollib;
using namespace physics_function;

void ALP_Shape::update_dop14(){

	switch (ALP_coll->shape)
	{
	case ALP_Collider_shape::Sphere:
		ALP_Shape::update_dop14_as_sphere();

		break;
	case ALP_Collider_shape::BOX:
		ALP_Shape::update_dop14_as_box();

		break;
	case ALP_Collider_shape::Plane:
		ALP_Shape::update_dop14_as_plane();

		break;
	case ALP_Collider_shape::Mesh:
		ALP_Shape::update_dop14_as_mesh();

		break;
	default:
		break;
	}
}

void ALP_Shape::update_dop14_as_sphere() {
	dop14.pos = ALP_coll->world_position;
	for (int i = 0; i < DOP::DOP_size; i++) {
		dop14.max[i] = +ALP_coll->world_scale.x * 1.0000001;
		dop14.min[i] = -ALP_coll->world_scale.x * 1.0000001;
	}
}

void ALP_Shape::update_dop14_as_box() {
	dop14.pos = ALP_coll->world_position;

	//各頂点のローカル座標
	Vector3 half[4]{
	{+ALP_coll->world_scale.x, -ALP_coll->world_scale.y, -ALP_coll->world_scale.z},
	{+ALP_coll->world_scale.x, -ALP_coll->world_scale.y, +ALP_coll->world_scale.z},
	{+ALP_coll->world_scale.x, +ALP_coll->world_scale.y, -ALP_coll->world_scale.z},
	{+ALP_coll->world_scale.x, +ALP_coll->world_scale.y, +ALP_coll->world_scale.z}
	};

	//for (int i = 0; i < 4; i++) {
	//	half[i] = half[i];
	//}

	Quaternion WO = ALP_coll->world_orientation;
	for (int i = 0; i < 4; i++) {
		half[i] = vector3_quatrotate(half[i], WO);
	}

	//DOPの更新
	float max_len = 0;
	for (int i = 0; i < DOP::DOP_size; i++) {
		max_len = 0;
		for (int o = 0; o < 4; o++) {
			float dis = fabsf(vector3_dot(DOP::DOP_14_axis[i], half[o]));
			if (max_len < dis) {
				dop14.max[i] = +dis * 1.00000001f;//確実にするためちょっと大きめにとる
				dop14.min[i] = -dis * 1.00000001f;//確実にするためちょっと大きめにとる
				max_len = dis;
			}

		}
	}
}

void ALP_Shape::update_dop14_as_plane() {
	dop14.pos = ALP_coll->world_position;
	for (int i = 0; i < DOP::DOP_size; i++) {
		dop14.max[i] = FLT_MAX;
	}
}

void ALP_Shape::update_dop14_as_mesh() {
	dop14.pos = ALP_coll->world_position;

	Vector3 rotated_axis[DOP::DOP_size];
	for (int i = 0; i < DOP::DOP_size; i++) {
		rotated_axis[i] = vector3_quatrotate(DOP::DOP_14_axis[i], ALP_coll->world_orientation.conjugate()).unit_vect();
		dop14.max[i] = -FLT_MAX;
		dop14.min[i] = +FLT_MAX;
	}

	Vector3 half[DOP::DOP_size * 2];
	int sum = 0;
	for (int i = 0; i < DOP::DOP_size; i++) {
		half[sum] = dopbase.max[i] * DOP::DOP_14_axis[i] * ALP_coll->world_scale;
		sum++;
		half[sum] = dopbase.min[i] * DOP::DOP_14_axis[i] * ALP_coll->world_scale;
		sum++;
	}

	//DOPの更新
	for (int i = 0; i < DOP::DOP_size; i++) {
		for (int o = 0; o < DOP::DOP_size * 2; o++) {
			float dis = vector3_dot(rotated_axis[i], half[o]);
			if (dop14.max[i] < dis) dop14.max[i] = dis * 1.00000001f;//確実にするためちょっと大きめにとる
			if (dop14.min[i] > dis) dop14.min[i] = dis * 1.00000001f;//確実にするためちょっと大きめにとる

		}
	}
}

