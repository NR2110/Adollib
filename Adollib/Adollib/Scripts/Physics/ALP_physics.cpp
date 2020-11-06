#include "ALP_physics.h"

#include "collider__base.h"
#include "../Object/gameobject.h"
#include "ALP__physics_manager.h"

using namespace Adollib;
using namespace physics_function;

void ALP_Physics::add_force(const Vector3& force) {
	accumulated_force += force;
}
void ALP_Physics::add_torque(const Vector3& force) {
	accumulated_torque += force;
}

bool ALP_Physics::is_movable() const {
	return (is_moveable && inertial_mass > 0 && inertial_mass < FLT_MAX);
}

float ALP_Physics::inverse_mass() const {
	if (is_movable()) return 1.0f / inertial_mass;
	//非可動オブジェクトなら0を返す
	else return 0;
}

Matrix ALP_Physics::inverse_inertial_tensor() const {
	Matrix inverse_inertial_tensor;
	if (is_movable()) {
		inverse_inertial_tensor = matrix_inverse(inertial_tensor);
		if (1) {
			Matrix rotation, transposed_rotation;
			rotation = ALP_coll->world_orientation.get_rotate_matrix();
			transposed_rotation = matrix_trans(rotation);
			inverse_inertial_tensor = transposed_rotation * inverse_inertial_tensor * rotation;
		}
	}
	else {
		inverse_inertial_tensor = matrix_identity();

		inverse_inertial_tensor._11 = 0;
		inverse_inertial_tensor._22 = 0;
		inverse_inertial_tensor._33 = 0;
	}
	return inverse_inertial_tensor;
}

void ALP_Physics::reset_force() {
	linear_velocity = Vector3(0, 0, 0);
	anglar_velocity = Vector3(0, 0, 0);

	accumulated_force = Vector3(0, 0, 0);
	accumulated_torque = Vector3(0, 0, 0);

	linear_acceleration = Vector3(0, 0, 0);
	angula_acceleration = Vector3(0, 0, 0);
}

void ALP_Physics::apply_external_force(float duration) {
	if (is_movable()) {

		if (is_fallable) linear_acceleration += Vector3(0, -Phyisics_manager::gravity, 0);

		//並進移動に加える力(accumulated_force)から加速度を出して並進速度を更新する
		linear_acceleration += accumulated_force / inertial_mass;
		linear_velocity += linear_acceleration * duration;


		//各回転に加える力(accumulated_torque)から加速度を出して角速度を更新する
		Matrix inverse_inertia_tensor = matrix_inverse(inertial_tensor);
		Matrix rotation = ALP_coll->local_orientation.get_rotate_matrix();
		Matrix transposed_rotation = matrix_trans(rotation);
		inverse_inertia_tensor = transposed_rotation * inverse_inertia_tensor * rotation;
		angula_acceleration += vector3_trans(accumulated_torque, inverse_inertia_tensor);

		anglar_velocity += angula_acceleration * duration;
		if (anglar_velocity.norm() < FLT_EPSILON)anglar_velocity = Vector3(0, 0, 0);
	}

	//加速を0にする
	accumulated_force = Vector3(0, 0, 0);
	accumulated_torque = Vector3(0, 0, 0);

	linear_acceleration = Vector3(0, 0, 0);
	angula_acceleration = Vector3(0, 0, 0);
}
void ALP_Physics::integrate(float duration) {
	//位置の更新
	if (linear_velocity.norm() >= FLT_EPSILON)
		ALP_coll->world_position += linear_velocity * duration;

	ALP_coll->world_orientation *= quaternion_radian_axis(anglar_velocity.norm_sqr() * duration * 0.5, anglar_velocity.unit_vect());
	ALP_coll->world_orientation = ALP_coll->world_orientation.unit_vect();

}

//サイズ変更などに対応するため毎フレーム慣性テンソルなどを更新
void ALP_Physics::update_inertial() {

	Vector3 Wsize = ALP_coll->world_scale;

	//慣性モーメントの更新
	switch (ALP_coll->shape)
	{
	case ALP_Collider_shape::Sphere:
		inertial_tensor = matrix_identity();
		inertial_tensor._11 = 0.4f * inertial_mass * Wsize.x * Wsize.x;
		inertial_tensor._22 = 0.4f * inertial_mass * Wsize.x * Wsize.x;
		inertial_tensor._33 = 0.4f * inertial_mass * Wsize.x * Wsize.x;

		break;
	case ALP_Collider_shape::BOX:
		inertial_tensor = matrix_identity();
		inertial_tensor._11 = 0.3333333f * inertial_mass * ((Wsize.y * Wsize.y) + (Wsize.z * Wsize.z));
		inertial_tensor._22 = 0.3333333f * inertial_mass * ((Wsize.z * Wsize.z) + (Wsize.x * Wsize.x));
		inertial_tensor._33 = 0.3333333f * inertial_mass * ((Wsize.x * Wsize.x) + (Wsize.y * Wsize.y));

		break;
	case ALP_Collider_shape::Plane:

		break;
	case ALP_Collider_shape::Mesh:
		inertial_tensor = matrix_identity();
		inertial_tensor._11 = 0.3333333f * inertial_mass * ((Wsize.y * Wsize.y) + (Wsize.z * Wsize.z));
		inertial_tensor._22 = 0.3333333f * inertial_mass * ((Wsize.z * Wsize.z) + (Wsize.x * Wsize.x));
		inertial_tensor._33 = 0.3333333f * inertial_mass * ((Wsize.x * Wsize.x) + (Wsize.y * Wsize.y));

		break;
	default:
		break;
	}
}

void ALP_Physics::refresh_ALP_from_data() {

	Physics_data Cdata = (*ALP_coll->coll_itr)->get_Physicsdata();

	inertial_mass = Cdata.inertial_mass;
	drag = Cdata.drag;
	anglar_drag = Cdata.anglar_drag;
	dynamic_friction = Cdata.dynamic_friction;
	static_friction = Cdata.static_friction;
	restitution = Cdata.restitution;

	is_fallable = Cdata.is_fallable;
	is_kinematic = Cdata.is_kinematic;
	is_moveable = Cdata.is_moveable;
	is_hitable = Cdata.is_hitable;

	refresh_physics_data();
}
