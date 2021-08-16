#include "ALP_physics.h"

#include "../Object/gameobject.h"
#include "ALP__physics_manager.h"

#include "ALP_collider.h"

using namespace Adollib;
using namespace Physics_function;

void ALP_Physics::set_default() {

	inertial_mass = PhysicsParams_default::inertial_mass; //質量
	linear_drag = PhysicsParams_default::linear_drag;//空気抵抗
	angula_drag = PhysicsParams_default::anglar_drag;//空気抵抗
	dynamic_friction = PhysicsParams_default::dynamic_friction;//動摩擦
	static_friction = PhysicsParams_default::static_friction;//静摩擦
	restitution = PhysicsParams_default::restitution; //反発係数

	is_fallable = PhysicsParams_default::is_fallable;//落ちない
	is_kinmatic_anglar = PhysicsParams_default::is_kinmatic_anglar;//ほかの物体からの影響で回転速度が変化しない
	is_kinmatic_linear = PhysicsParams_default::is_kinmatic_linear;//ほかの物体からの影響で並進速度が変化しない
	is_moveable = PhysicsParams_default::is_moveable;//動かない
	is_hitable = PhysicsParams_default::is_hitable;

};

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

Matrix33 ALP_Physics::inverse_inertial_tensor() const {
	Matrix33 inverse_inertial_tensor;
	if (is_movable()) {
		inverse_inertial_tensor = matrix_inverse(inertial_tensor);

		if (isnan(matrix_determinant(inverse_inertial_tensor))) assert(0 && "toooooo laaarrrrrrrrgge");

		Matrix33 rotation, transposed_rotation;
		rotation = gameobject->world_orientate().get_rotate_matrix();
		transposed_rotation = matrix_trans(rotation);
		inverse_inertial_tensor = rotation * inverse_inertial_tensor * transposed_rotation;
	}
	else {
		inverse_inertial_tensor = matrix33_identity();

		inverse_inertial_tensor._11 = 0;
		inverse_inertial_tensor._22 = 0;
		inverse_inertial_tensor._33 = 0;
	}
	return inverse_inertial_tensor;
}

void ALP_Physics::reset_force() {
	linear_velocity = Vector3(0, 0, 0);
	angula_velocity = Vector3(0, 0, 0);

	accumulated_force = Vector3(0, 0, 0);
	accumulated_torque = Vector3(0, 0, 0);

	linear_acceleration = Vector3(0, 0, 0);
	angula_acceleration = Vector3(0, 0, 0);
}

void ALP_Physics::apply_external_force(float duration) {
	//前の速度を保存 (integrateで使う)
		old_angula_velocity = angula_velocity;
		old_linear_velocity = linear_velocity;

	if (is_movable()) {
		//inv_rotate = Quaternion(1, 0, 0, 0);

		//angula_velocity = angula_velocity * pow(1 - angula_drag, duration);
		//linear_velocity = linear_velocity * pow(1 - linear_drag, duration);

		const float inv_mass = 1 / inertial_mass;
		//if (is_fallable) accumulated_force += Vector3(0, -Phyisics_manager::physicsParams.gravity, 0) * inertial_mass; //落下

		//空気抵抗の求め方
		// k は流体の密度やらなんやらを考慮した定数
		//F(t) = 1/2 * k * v(t)
		//F(t) = m * a(t)
		//a(t) = dv/dt = - * k * v(t) / m
		//∫ 1 / v(t) * dv/dt * dt = ∫ -k / m * dt
		//ln v(t) = -k / m * t + C
		//v(t) = exp(-k / m * t + C)
		//v(t) = C´ * exp(-k / m * t)
		//t=0の時 C´ = V(0)より
		//v(t) = V(0) * exp(-k / m * t)
		//const float kl = linear_drag * inv_mass; //空気抵抗やらなんやらを考慮した値 のはずだけど適当に簡略化
		//linear_velocity = linear_velocity * exp(-kl * duration); // 空気抵抗

		//const float ka = angula_drag * inv_mass; //空気抵抗やらなんやらを考慮した値 のはずだけど適当に簡略化
		//angula_velocity = angula_velocity * exp(-ka * duration); // 空気抵抗

		//並進移動に加える力(accumulated_force)から加速度を出して並進速度を更新する 向きを間違えないように!!
		linear_acceleration += accumulated_force * inv_mass;
		if (is_fallable) linear_acceleration += Vector3(0, -Phyisics_manager::physicsParams.gravity, 0); //落下
		linear_velocity += linear_acceleration * duration;

		//各回転に加える力(accumulated_torque)から加速度を出して角速度を更新する
		Matrix33 inverse_inertia_tensor = matrix_inverse(inertial_tensor);
		//Matrix rotation = ALPcollider->local_orientation.get_rotate_matrix();
		Matrix33 rotation = gameobject->world_orientate().get_rotate_matrix();
		Matrix33 transposed_rotation = matrix_trans(rotation);
		inverse_inertia_tensor = rotation * inverse_inertia_tensor * rotation * transposed_rotation;
		angula_acceleration += vector3_trans(accumulated_torque, inverse_inertia_tensor);

		angula_velocity += angula_acceleration * duration;
		if (angula_velocity.norm() < FLT_EPSILON)angula_velocity = Vector3(0, 0, 0);
	}
	else reset_force();

	//加速を0にする
	accumulated_force = Vector3(0, 0, 0);
	accumulated_torque = Vector3(0, 0, 0);

	linear_acceleration = Vector3(0, 0, 0);
	angula_acceleration = Vector3(0, 0, 0);
}
void ALP_Physics::integrate(float duration) {

	if (is_movable() == false)return;

	if (linear_velocity.norm() < Phyisics_manager::physicsParams.linear_sleep_threrhold * Phyisics_manager::physicsParams.linear_sleep_threrhold &&
		angula_velocity.norm() < Phyisics_manager::physicsParams.angula_sleep_threrhold * Phyisics_manager::physicsParams.angula_sleep_threrhold) {
		sleep_timer += duration;
	}
	else sleep_timer = 0;

	if (sleep_timer > 0.032) {
		linear_velocity = Vector3(0);
		angula_velocity = Vector3(0);
		is_sleep = true;
		return;
	}
	else {
		is_sleep = false;
	}

	ALPcollider->integrate(duration, linear_velocity, angula_velocity, old_linear_velocity, old_angula_velocity);

}

const Vector3 ALP_Physics::get_barycenter() const {
	if (is_user_barycnter) return barycenter;

	const auto shapes = ALPcollider->get_shapes();
	Vector3 val;
	float sum_valume = 0;
	{
		val = Vector3(0);
		for (const auto& shape : shapes) {
			const float shape_mass = shape->get_volume();
			val += shape_mass * shape->local_position;
			sum_valume += shape_mass;
		}
		val /= sum_valume;
	}

	return val;
}
void ALP_Physics::set_barycenter(const Vector3& cent) {
	barycenter = cent;
	is_user_barycnter = true;
}

//アタッチされたshapesから慣性モーメントと質量、ついでに重心の更新
void ALP_Physics::update_tensor_and_barycenter(const std::vector<Collider_shape*>& shapes, const std::list<ALP_Joint*>& joints) {

	if (shapes.size() == 0) {
		barycenter = Vector3(0, 0, 0);
		inertial_tensor = matrix33_identity();
		return;
	}

	float sum_valume = 0;

	{
		//重心の更新
		Vector3 bary = Vector3(0);
		for (const auto& shape : shapes) {
			const float shape_mass = shape->get_volume();
			bary += shape_mass * shape->local_position;
			sum_valume += shape_mass;
		}

		if (is_user_barycnter == false) {
			bary /= sum_valume;
			barycenter = bary;
		}
	}


	//ユーザーに定義された慣性モーメントが無いとき
	if (is_user_tensor == false) {

		//慣性モーメントの更新
		inertial_tensor = matrix33_zero();
		for (const auto& shape : shapes) {
			const float shape_mass = shape->get_volume();
			inertial_tensor += shape_mass / sum_valume * shape->get_tensor(barycenter);
		}

		inertial_tensor *= inertial_mass;


		for (auto& joint : joints) {
			//auto a = joint->joint->get_colliderA();
			inertial_tensor += joint->joint->tensor_effect(ALPcollider->get_collptr());
		}



		inertial_tensor._11 *= gameobject->world_scale().x;
		inertial_tensor._22 *= gameobject->world_scale().y;
		inertial_tensor._33 *= gameobject->world_scale().z;
	}


	//質量と重心の更新
	//inertial_mass = 0;
	//barycenter = Vector3(0);
	//for (const auto& shape : shapes) {
	//	inertial_mass += shape->mass;
	//	barycenter += shape->local_position * shape->mass;
	//}

	//barycenter /= inertial_mass;

	////重心の更新
	//barycenter = Vector3(0);
	//for (auto& shape : shapes) {
	//	barycenter += shape->local_position * shape->mass;
	//}

}

void ALP_Physics::update_physics_data() {

	Physics_data Cdata = ALPcollider->get_collptr()->physics_data;

	inertial_mass = Cdata.inertial_mass;
	linear_drag = Cdata.drag;
	angula_drag = Cdata.anglar_drag;
	dynamic_friction = Cdata.dynamic_friction;
	static_friction = Cdata.static_friction;
	restitution = Cdata.restitution;

	is_fallable = Cdata.is_fallable;
	is_kinmatic_anglar = Cdata.is_kinmatic_anglar; //ほかの物体からの影響で回転速度が変化しない
	is_kinmatic_linear = Cdata.is_kinmatic_linear; //ほかの物体からの影響で並進速度が変化しない
	is_moveable = Cdata.is_moveable;
	is_hitable = Cdata.is_hitable;
}

void ALP_Physics::destroy() {
	Phyisics_manager::remove_ALPphysics(scene, this_itr);
}