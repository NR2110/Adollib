#include "collider__base.h"

using namespace Adollib;
using namespace Contacts;

#include "ALP__manager.h"
#include "../Object/gameobject.h"
#include "../Main/Adollib.h"
#include "contact.h"

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
		#pragma region Rigitbody
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
void Collider::reset_force() {
	linear_velocity = Vector3(0, 0, 0);
	angula_velocity = Vector3(0, 0, 0);

	accumulated_force = Vector3(0, 0, 0);
	accumulated_torque = Vector3(0, 0, 0);

	linear_acceleration = Vector3(0, 0, 0);
	angula_acceleration = Vector3(0, 0, 0);
}

void Collider::apply_external_force(float duration) {
	if (is_movable()) {

		if(is_fallable()) linear_acceleration += Vector3(0, -physics_g::gravity, 0); //TODO : time_stepを考慮していない

		//並進移動に加える力(accumulated_force)から加速度を出して並進速度を更新する
		linear_acceleration += accumulated_force / inertial_mass;
		linear_velocity += linear_acceleration * duration;
		

		//各回転に加える力(accumulated_torque)から加速度を出して角速度を更新する
		matrix inverse_inertia_tensor = matrix_inverse(inertial_tensor);
		matrix rotation = local_orientation.get_rotate_matrix();
		matrix transposed_rotation = matrix_trans(rotation);
		inverse_inertia_tensor = transposed_rotation * inverse_inertia_tensor * rotation;
		angula_acceleration += vector3_trans(accumulated_torque, inverse_inertia_tensor);

		angula_velocity += angula_acceleration * duration;
		if (angula_velocity.norm() < FLT_EPSILON)angula_velocity = Vector3(0, 0, 0);
	}

	//加速を0にする
	accumulated_force = Vector3(0, 0, 0);
	accumulated_torque = Vector3(0, 0, 0);

	linear_acceleration = Vector3(0, 0, 0);
	angula_acceleration = Vector3(0, 0, 0);
}
void Collider::integrate(float duration) {
	//位置の更新
	if (linear_velocity.norm() >= FLT_EPSILON)
		world_position += linear_velocity * duration;

	world_orientation *= quaternion_radian_axis(angula_velocity.norm_sqr() * duration * 0.5, angula_velocity.unit_vect());
	world_orientation = world_orientation.unit_vect();

}

void Collider::solv_resolve() {
	offset_CollGO_quat = local_orientation.conjugate() * gameobject->get_world_orientate().conjugate() * world_orientation;
	offset_CollGO_pos = world_position - vector3_quatrotate(local_position * gameobject->get_world_scale(), world_orientation) - gameobject->get_world_position();
}

void Collider::resolve_gameobject() {
	gameobject->transform->local_orient *= offset_CollGO_quat;
	gameobject->transform->local_pos += offset_CollGO_pos;
}

bool Collider::concoll_enter(std::string tag_name) {
	if (oncoll_checkmap.count(tag_name) == 0) {
		oncoll_checkmap[tag_name] = false;
		oncoll_enter_names.push_back(tag_name);
	}
	bool ret = oncoll_checkmap[tag_name];
	return ret;
}

void Collider::add_force(const Vector3& force) {
	accumulated_force += force;
}
void Collider::add_torque(const Vector3& force) {
	accumulated_torque += force;
}

bool Collider::is_movable() const {
	return (move && inertial_mass > 0 && inertial_mass < FLT_MAX);
}
bool Collider::is_fallable() const {
	return fall;
}
float Collider::inverse_mass() const {
	if (is_movable()) return 1.0f / inertial_mass;
	//非可動オブジェクトなら0を返す
	else return 0;
}

matrix Collider::inverse_inertial_tensor() const{
	matrix inverse_inertial_tensor;
	if (is_movable()) {
		inverse_inertial_tensor = matrix_inverse(inertial_tensor);
		if(1){
			matrix rotation, transposed_rotation;
			rotation = world_orientation.get_rotate_matrix();
			transposed_rotation = matrix_trans(rotation);
			inverse_inertial_tensor = transposed_rotation * inverse_inertial_tensor * rotation;
		}
	}
	else {
		inverse_inertial_tensor = matrix_identity();
		//上だとオブジェクトの中心から離れた衝突判定でバグる　下にすべき
		//inverse_inertial_tensor._11 = FLT_EPSILON;
		//inverse_inertial_tensor._22 = FLT_EPSILON;
		//inverse_inertial_tensor._33 = FLT_EPSILON;

		inverse_inertial_tensor._11 = 0;
		inverse_inertial_tensor._22 = 0;
		inverse_inertial_tensor._33 = 0;
	}
	return inverse_inertial_tensor;
}

#pragma endregion
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

void Collider::awake() {
	std::list<Collider*>::iterator C = Rigitbody_manager::add_collider(this);
}
void Collider::finalize() {
	Rigitbody_manager::remove_collider(this);
}





//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
