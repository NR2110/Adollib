#include "ALP_physics.h"

#include "../Object/gameobject.h"
#include "ALP__physics_manager.h"

#include "ALP_collider.h"

using namespace Adollib;
using namespace Physics_function;

void ALP_Physics::set_default() {

	inertial_mass = PhysicsParams_default::inertial_mass; //����
	linear_drag = PhysicsParams_default::linear_drag;//��C��R
	angula_drag = PhysicsParams_default::anglar_drag;//��C��R
	dynamic_friction = PhysicsParams_default::dynamic_friction;//�����C
	static_friction = PhysicsParams_default::static_friction;//�Ö��C
	restitution = PhysicsParams_default::restitution; //�����W��

	is_fallable = PhysicsParams_default::is_fallable;//�����Ȃ�
	is_kinmatic_anglar = PhysicsParams_default::is_kinmatic_anglar;//�ق��̕��̂���̉e���ŉ�]���x���ω����Ȃ�
	is_kinmatic_linear = PhysicsParams_default::is_kinmatic_linear;//�ق��̕��̂���̉e���ŕ��i���x���ω����Ȃ�
	is_moveable = PhysicsParams_default::is_moveable;//�����Ȃ�
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
	//����I�u�W�F�N�g�Ȃ�0��Ԃ�
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
	//�O�̑��x��ۑ� (integrate�Ŏg��)
		old_angula_velocity = angula_velocity;
		old_linear_velocity = linear_velocity;

	if (is_movable()) {
		//inv_rotate = Quaternion(1, 0, 0, 0);

		//angula_velocity = angula_velocity * pow(1 - angula_drag, duration);
		//linear_velocity = linear_velocity * pow(1 - linear_drag, duration);

		const float inv_mass = 1 / inertial_mass;
		//if (is_fallable) accumulated_force += Vector3(0, -Phyisics_manager::physicsParams.gravity, 0) * inertial_mass; //����

		//��C��R�̋��ߕ�
		// k �͗��̖̂��x���Ȃ�����l�������萔
		//F(t) = 1/2 * k * v(t)
		//F(t) = m * a(t)
		//a(t) = dv/dt = - * k * v(t) / m
		//�� 1 / v(t) * dv/dt * dt = �� -k / m * dt
		//ln v(t) = -k / m * t + C
		//v(t) = exp(-k / m * t + C)
		//v(t) = C�L * exp(-k / m * t)
		//t=0�̎� C�L = V(0)���
		//v(t) = V(0) * exp(-k / m * t)
		//const float kl = linear_drag * inv_mass; //��C��R���Ȃ�����l�������l �̂͂������ǓK���Ɋȗ���
		//linear_velocity = linear_velocity * exp(-kl * duration); // ��C��R

		//const float ka = angula_drag * inv_mass; //��C��R���Ȃ�����l�������l �̂͂������ǓK���Ɋȗ���
		//angula_velocity = angula_velocity * exp(-ka * duration); // ��C��R

		//���i�ړ��ɉ������(accumulated_force)��������x���o���ĕ��i���x���X�V���� �������ԈႦ�Ȃ��悤��!!
		linear_acceleration += accumulated_force * inv_mass;
		if (is_fallable) linear_acceleration += Vector3(0, -Phyisics_manager::physicsParams.gravity, 0); //����
		linear_velocity += linear_acceleration * duration;

		//�e��]�ɉ������(accumulated_torque)��������x���o���Ċp���x���X�V����
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

	//������0�ɂ���
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

//�A�^�b�`���ꂽshapes���犵�����[�����g�Ǝ��ʁA���łɏd�S�̍X�V
void ALP_Physics::update_tensor_and_barycenter(const std::vector<Collider_shape*>& shapes, const std::list<ALP_Joint*>& joints) {

	if (shapes.size() == 0) {
		barycenter = Vector3(0, 0, 0);
		inertial_tensor = matrix33_identity();
		return;
	}

	float sum_valume = 0;

	{
		//�d�S�̍X�V
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


	//���[�U�[�ɒ�`���ꂽ�������[�����g�������Ƃ�
	if (is_user_tensor == false) {

		//�������[�����g�̍X�V
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


	//���ʂƏd�S�̍X�V
	//inertial_mass = 0;
	//barycenter = Vector3(0);
	//for (const auto& shape : shapes) {
	//	inertial_mass += shape->mass;
	//	barycenter += shape->local_position * shape->mass;
	//}

	//barycenter /= inertial_mass;

	////�d�S�̍X�V
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
	is_kinmatic_anglar = Cdata.is_kinmatic_anglar; //�ق��̕��̂���̉e���ŉ�]���x���ω����Ȃ�
	is_kinmatic_linear = Cdata.is_kinmatic_linear; //�ق��̕��̂���̉e���ŕ��i���x���ω����Ȃ�
	is_moveable = Cdata.is_moveable;
	is_hitable = Cdata.is_hitable;
}

void ALP_Physics::destroy() {
	Phyisics_manager::remove_ALPphysics(scene, this_itr);
}