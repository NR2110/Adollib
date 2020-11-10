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
	offset_CollGO_quat = local_orientation.conjugate() * (*coll_itr)->gameobject->get_world_orientate().conjugate() * world_orientation;
	offset_CollGO_pos = world_position - vector3_quatrotate(local_position * (*coll_itr)->gameobject->get_world_scale(), world_orientation) - (*coll_itr)->gameobject->get_world_position();
}

void ALP_Collider::resolve_gameobject() {
	(*coll_itr)->gameobject->transform->local_orient *= offset_CollGO_quat;
	(*coll_itr)->gameobject->transform->local_pos += offset_CollGO_pos;
}

void ALP_Collider::update_world_trans() {
	world_orientation = (*coll_itr)->gameobject->get_world_orientate() * local_orientation;
	world_scale = (*coll_itr)->gameobject->get_world_scale() * local_scale;
	world_position = (*coll_itr)->gameobject->get_world_position() + vector3_quatrotate(local_position * (*coll_itr)->gameobject->get_world_scale(), world_orientation);

	ALPphysics->update_inertial();
}

void ALP_Collider::refresh_ALP_from_data() {

	Collider_data Cdata = (*coll_itr)->get_Colliderdata();

	local_position = Cdata.local_position;
	local_orientation = Cdata.local_orientation;
	local_scale = Cdata.local_scale;

	half_size = Cdata.half_size;
	shape = Cdata.shape;

	meshcoll_data = Cdata.meshcoll_data;

	ALPphysics->refresh_ALP_from_data();
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
	for (auto& mesh : collider_meshes) {
		mesh.dop14.pos = world_position;
		for (int i = 0; i < DOP::DOP_size; i++) {
			mesh.dop14.max[i] = +world_scale.x * 1.0000001f;
			mesh.dop14.min[i] = -world_scale.x * 1.0000001f;
		}
	}
}

void ALP_Collider::update_dop14_as_box() {
	for (auto& mesh : collider_meshes) {
		mesh.dop14.pos = world_position;

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
					mesh.dop14.max[i] = +dis * 1.00000001f;//確実にするためちょっと大きめにとる
					mesh.dop14.min[i] = -dis * 1.00000001f;//確実にするためちょっと大きめにとる
					max_len = dis;
				}

			}
		}
	}
}

void ALP_Collider::update_dop14_as_plane() {
	for (auto& mesh : collider_meshes) {
		mesh.dop14.pos = world_position;
		for (int i = 0; i < DOP::DOP_size; i++) {
			mesh.dop14.max[i] = FLT_MAX;
		}
	}
}

void ALP_Collider::update_dop14_as_mesh() {
	for (auto& mesh : collider_meshes) {
		mesh.dop14.pos = world_position;

		Vector3 rotated_axis[DOP::DOP_size];
		for (int i = 0; i < DOP::DOP_size; i++) {
			rotated_axis[i] = vector3_quatrotate(DOP::DOP_14_axis[i], world_orientation.conjugate()).unit_vect();
			mesh.dop14.max[i] = -FLT_MAX;
			mesh.dop14.min[i] = +FLT_MAX;
		}

		Vector3 half[DOP::DOP_size * 2];
		int sum = 0;
		for (int i = 0; i < DOP::DOP_size; i++) {
			half[sum] = mesh.mesh->dopbase.max[i] * DOP::DOP_14_axis[i] * world_scale;
			sum++;
			half[sum] = mesh.mesh->dopbase.min[i] * DOP::DOP_14_axis[i] * world_scale;
			sum++;
		}

		//DOPの更新
		for (int i = 0; i < DOP::DOP_size; i++) {
			for (int o = 0; o < DOP::DOP_size * 2; o++) {
				float dis = vector3_dot(rotated_axis[i], half[o]);
				if (mesh.dop14.max[i] < dis) mesh.dop14.max[i] = dis * 1.00000001f;//確実にするためちょっと大きめにとる
				if (mesh.dop14.min[i] > dis) mesh.dop14.min[i] = dis * 1.00000001f;//確実にするためちょっと大きめにとる

			}
		}
	}
}

#pragma endregion
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::