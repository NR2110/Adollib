#include "ALP_Collider.h"

#include "collider__base.h"
#include "../Object/gameobject.h"

using namespace Adollib;
using namespace physics_function;

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
#pragma region Collider
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
const bool ALP_Collider::concoll_enter (const unsigned int tag_name) {
	oncoll_check_bits |= tag_name;
	return (oncoll_bits & tag_name);
}

void ALP_Collider::solv_resolve() {
	offset_CollGO_quat = local_orientation.conjugate() * coll->gameobject->get_world_orientate().conjugate() * world_orientation;
	offset_CollGO_pos = world_position - vector3_quatrotate(local_position * coll->gameobject->get_world_scale(), world_orientation) - coll->gameobject->get_world_position();
}

void ALP_Collider::resolve_gameobject() {
	coll->gameobject->transform->local_orient *= offset_CollGO_quat;
	coll->gameobject->transform->local_pos += offset_CollGO_pos;
}

void ALP_Collider::update_world_trans() {
	world_orientation = coll->gameobject->get_world_orientate() * local_orientation;
	world_scale = coll->gameobject->get_world_scale() * local_scale;
	world_position = coll->gameobject->get_world_position() + vector3_quatrotate(local_position * coll->gameobject->get_world_scale(), world_orientation);
}

#pragma endregion
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
#pragma region Update_dop14
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
void ALP_Collider::update_dop14() {

	switch (shape)
	{
	case ALP_Collider_shape::Sphere:
		ALP_Collider::update_dop14_as_sphere();

		break;
	case ALP_Collider_shape::BOX:
		ALP_Collider::update_dop14_as_box();

		break;
	case ALP_Collider_shape::Plane:
		ALP_Collider::update_dop14_as_plane();

		break;
	case ALP_Collider_shape::Mesh:
		ALP_Collider::update_dop14_as_mesh();

		break;
	default:
		break;
	}
}

void ALP_Collider::update_dop14_as_sphere() {
	dop14.pos = world_position;
	for (int i = 0; i < DOP::DOP_size; i++) {
		dop14.max[i] = +world_scale.x * 1.0000001;
		dop14.min[i] = -world_scale.x * 1.0000001;
	}
}

void ALP_Collider::update_dop14_as_box() {
	dop14.pos = world_position;

	//各頂点のローカル座標
	Vector3 half[4]{
	{+world_scale.x, -world_scale.y, -world_scale.z},
	{+world_scale.x, -world_scale.y, +world_scale.z},
	{+world_scale.x, +world_scale.y, -world_scale.z},
	{+world_scale.x, +world_scale.y, +world_scale.z}
	};

	//for (int i = 0; i < 4; i++) {
	//	half[i] = half[i];
	//}

	Quaternion WO = world_orientation;
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

void ALP_Collider::update_dop14_as_plane() {
	dop14.pos = world_position;
	for (int i = 0; i < DOP::DOP_size; i++) {
		dop14.max[i] = FLT_MAX;
	}
}

void ALP_Collider::update_dop14_as_mesh() {
	dop14.pos = world_position;

	Vector3 rotated_axis[DOP::DOP_size];
	for (int i = 0; i < DOP::DOP_size; i++) {
		rotated_axis[i] = vector3_quatrotate(DOP::DOP_14_axis[i], world_orientation.conjugate()).unit_vect();
		dop14.max[i] = -FLT_MAX;
		dop14.min[i] = +FLT_MAX;
	}

	Vector3 half[DOP::DOP_size * 2];
	int sum = 0;
	for (int i = 0; i < DOP::DOP_size; i++) {
		half[sum] = dopbase.max[i] * DOP::DOP_14_axis[i] * world_scale;
		sum++;
		half[sum] = dopbase.min[i] * DOP::DOP_14_axis[i] * world_scale;
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

#pragma endregion
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::