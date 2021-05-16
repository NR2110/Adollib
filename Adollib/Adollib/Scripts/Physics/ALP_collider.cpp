#include "ALP_Collider.h"

#include "collider__base.h"
#include "../Object/gameobject.h"

#include "ALP__physics_manager.h"

using namespace Adollib;
using namespace Physics_function;


//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
#pragma region Collider
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
const bool ALP_Collider::concoll_enter(const unsigned int tag_name) {
	oncoll_check_bits |= tag_name;
	return (oncoll_bits & tag_name);
}

void ALP_Collider::solv_resolve() {
	//offset_CollGO_quat = world_orientation() * (*coll_itr)->gameobject->get_world_orientate().inverse() * local_orientation.inverse();
	//offset_CollGO_quat = world_orientation() * (local_orientation * (*coll_itr)->gameobject->get_world_orientate()).inverse();
	offset_CollGO_quat = (local_orientation * (*coll_itr)->gameobject->get_world_orientate()).inverse() * world_orientation();
	//offset_CollGO_quat = local_orientation.inverse() * (*coll_itr)->gameobject->get_world_orientate().inverse() * world_orientation();
	offset_CollGO_pos = world_position() - vector3_quatrotate(local_position * (*coll_itr)->gameobject->get_world_scale(), world_orientation()) - (*coll_itr)->gameobject->get_world_position();
}

void ALP_Collider::resolve_gameobject() {
	(*coll_itr)->gameobject->transform->local_orient = (*coll_itr)->gameobject->transform->local_orient * offset_CollGO_quat;
	(*coll_itr)->gameobject->transform->local_pos += offset_CollGO_pos;
}

void ALP_Collider::update_world_trans() {
	old_world_position_ = world_position_;
	old_world_orientation_ = world_orientation_;
	old_world_scale_ = world_scale_;

	world_orientation_ = local_orientation * (*coll_itr)->gameobject->get_world_orientate();
	//world_orientation_ = (*coll_itr)->gameobject->get_world_orientate() * local_orientation;
	world_scale_ = (*coll_itr)->gameobject->get_world_scale() * local_scale;
	world_position_ = (*coll_itr)->gameobject->get_world_position() + vector3_quatrotate(local_position * (*coll_itr)->gameobject->get_world_scale(), world_orientation());

	if (old_world_position_ != world_position_ ||
		old_world_orientation_ != world_orientation_ ||
		old_world_scale_ != world_scale_
		)
		Phyisics_manager::add_moved(ALPcollider);

	ALPphysics->update_inertial();
}

void ALP_Collider::refresh_ALP_from_data() {

	Collider_data Cdata = (*coll_itr)->get_Colliderdata();

	local_position = Cdata.local_position;
	local_orientation = Cdata.local_orientation;
	local_scale = Cdata.local_scale;

	half_size = Cdata.half_size;
	shape = Cdata.shape;

	tag = Cdata.tag;
	nohit_tag = Cdata.nohit_tag;

	ALPphysics->refresh_ALP_from_data();
}

#pragma endregion
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
#pragma region Update_dop14
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
void ALP_Collider::update_dop14() {

	collider_meshes[0].dop14.max[2];

	switch (shape)
	{
	case ALP_Collider_shape::Sphere:
		ALP_Collider::update_dop14_as_sphere();

		break;
	case ALP_Collider_shape::BOX:
		ALP_Collider::update_dop14_as_box();

		break;
	case ALP_Collider_shape::Capsule:
		ALP_Collider::update_dop14_as_capsule();

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
		mesh.dop14.pos = world_position();
		for (int i = 0; i < DOP::DOP14_size; i++) {
			mesh.dop14.max[i] = +world_scale().x * 1.0000001f;
			mesh.dop14.min[i] = -world_scale().x * 1.0000001f;
		}
	}
}

void ALP_Collider::update_dop14_as_box() {
	for (auto& mesh : collider_meshes) {
		mesh.dop14.pos = world_position();

		//各頂点のローカル座標
		Vector3 half[4]{
		{+world_scale().x, -world_scale().y, -world_scale().z},
		{+world_scale().x, -world_scale().y, +world_scale().z},
		{+world_scale().x, +world_scale().y, -world_scale().z},
		{+world_scale().x, +world_scale().y, +world_scale().z}
		};

		Quaternion WO = world_orientation();
		for (int i = 0; i < 4; i++) {
			half[i] = vector3_quatrotate(half[i], WO);
		}

		//DOPの更新
		float max_len = 0;
		for (int i = 0; i < DOP::DOP14_size; i++) {
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

void ALP_Collider::update_dop14_as_capsule() {
	for (auto& mesh : collider_meshes) {
		Vector3 p = vector3_quatrotate(Vector3(0, world_scale().y, 0), world_orientation());
		mesh.dop14.pos = world_position();
		for (int i = 0; i < DOP::DOP14_size; i++) {
			float v0, v1, v2, v3;
			v0 = vector3_dot(+p, DOP::DOP_14_axis[i]) + +world_scale().x * 1.0000001f;
			v1 = vector3_dot(+p, DOP::DOP_14_axis[i]) + -world_scale().x * 1.0000001f;
			v2 = vector3_dot(-p, DOP::DOP_14_axis[i]) + +world_scale().x * 1.0000001f;
			v3 = vector3_dot(-p, DOP::DOP_14_axis[i]) + -world_scale().x * 1.0000001f;
			mesh.dop14.max[i] = ALmax(ALmax(v0, v1), ALmax(v2, v3));
			mesh.dop14.min[i] = ALmin(ALmin(v0, v1), ALmin(v2, v3));
		}
	}
}

void ALP_Collider::update_dop14_as_plane() {
	for (auto& mesh : collider_meshes) {
		mesh.dop14.pos = world_position();
		for (int i = 0; i < DOP::DOP14_size; i++) {
			mesh.dop14.max[i] = FLT_MAX;
		}
	}
}

void ALP_Collider::update_dop14_as_mesh() {
	for (auto& mesh : collider_meshes) {
		mesh.dop14.pos = world_position();

		Vector3 rotated_axis[DOP::DOP14_size];
		for (int i = 0; i < DOP::DOP14_size; i++) {
			rotated_axis[i] = vector3_quatrotate(DOP::DOP_14_axis[i], world_orientation().inverse()).unit_vect();
			mesh.dop14.max[i] = -FLT_MAX;
			mesh.dop14.min[i] = +FLT_MAX;
		}

		for (int v_num = 0; v_num < 8; v_num++) {
			const Vector3& pos = mesh.mesh_data->base_pos[v_num] * world_scale();

			//DOPの更新
			for (int i = 0; i < DOP::DOP14_size; i++) {
				const float dis = vector3_dot(rotated_axis[i], pos);
				if (mesh.dop14.min[i] > dis) mesh.dop14.min[i] = dis * 1.00000001f;//確実にするためちょっと大きめにとる
				if (mesh.dop14.max[i] < dis) mesh.dop14.max[i] = dis * 1.00000001f;//確実にするためちょっと大きめにとる

			}
		}

	}
}

#pragma endregion
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::


void ALP_Collider::integrate(float duration, Vector3 linear_velocity, Vector3 anglar_velocity) {
	if (linear_velocity.norm() == 0 && anglar_velocity.norm() == 0)return;

	Quaternion pearent_orientate_inv = Quaternion(1, 0, 0, 0);
	if ((*ALPcollider->coll_itr)->gameobject->pearent() != nullptr) {
		pearent_orientate_inv = (*ALPcollider->coll_itr)->gameobject->pearent()->get_world_orientate();
		pearent_orientate_inv = pearent_orientate_inv.inverse();
	}

	//位置の更新
	//if (linear_velocity.norm() >= FLT_EPSILON)

	Vector3 local_linear_velocity = vector3_quatrotate(linear_velocity, pearent_orientate_inv);
	Vector3 local_anglar_velocity = vector3_quatrotate(anglar_velocity, pearent_orientate_inv);

	world_position_ += local_linear_velocity * duration;

	world_orientation_ *= quaternion_radian_axis(local_anglar_velocity.norm_sqr() * duration * 0.5f, local_anglar_velocity.unit_vect());
	world_orientation_ = world_orientation().unit_vect();
}