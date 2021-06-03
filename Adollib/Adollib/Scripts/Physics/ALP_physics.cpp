#include "ALP_physics.h"

#include "collider__base.h"
#include "../Object/gameobject.h"
#include "ALP__physics_manager.h"

#include "collider__base.h"

using namespace Adollib;
using namespace Physics_function;

void ALP_Physics::set_default() {

	inertial_mass = PhysicsParams_default::inertial_mass; //����
	linear_drag = PhysicsParams_default::linear_drag;//��C��R
	anglar_drag = PhysicsParams_default::anglar_drag;//��C��R
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

Matrix ALP_Physics::inverse_inertial_tensor() const {
	Matrix inverse_inertial_tensor;
	if (is_movable()) {
		inverse_inertial_tensor = matrix_inverse(inertial_tensor);

		Matrix rotation, transposed_rotation;
		rotation = gameobject->world_orientate().get_rotate_matrix();
		transposed_rotation = matrix_trans(rotation);
		inverse_inertial_tensor = rotation * inverse_inertial_tensor * transposed_rotation;
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
		//inv_rotate = Quaternion(1, 0, 0, 0);

		const float inv_mass = 1 / inertial_mass;
		if (is_fallable) accumulated_force += Vector3(0, -Phyisics_manager::physicsParams.gravity, 0) * inertial_mass; //����

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
		const float k = linear_drag * inv_mass; //��C��R���Ȃ�����l�������l �̂͂������ǓK���Ɋȗ���
		linear_velocity = linear_velocity * exp(-k * duration); // ��C��R

		//���i�ړ��ɉ������(accumulated_force)��������x���o���ĕ��i���x���X�V���� �������ԈႦ�Ȃ��悤��!!
		linear_acceleration += accumulated_force * inv_mass;
		linear_velocity += linear_acceleration * duration;

		//�e��]�ɉ������(accumulated_torque)��������x���o���Ċp���x���X�V����
		Matrix inverse_inertia_tensor = matrix_inverse(inertial_tensor);
		//Matrix rotation = ALPcollider->local_orientation.get_rotate_matrix();
		Matrix rotation = gameobject->world_orientate().get_rotate_matrix();
		Matrix transposed_rotation = matrix_trans(rotation);
		inverse_inertia_tensor = transposed_rotation * inverse_inertia_tensor * rotation;
		angula_acceleration += vector3_trans(accumulated_torque, inverse_inertia_tensor);

		anglar_velocity += angula_acceleration * duration;
		if (anglar_velocity.norm() < FLT_EPSILON)anglar_velocity = Vector3(0, 0, 0);
	}
	else reset_force();

	//������0�ɂ���
	accumulated_force = Vector3(0, 0, 0);
	accumulated_torque = Vector3(0, 0, 0);

	linear_acceleration = Vector3(0, 0, 0);
	angula_acceleration = Vector3(0, 0, 0);
}
void ALP_Physics::integrate(float duration) {

	//if (is_movable() == false)return;

	////�ʒu�̍X�V
	//if (linear_velocity.norm() >= FLT_EPSILON)
	//	ALP_coll->offset_CollGO_pos += linear_velocity * duration;

	//ALP_coll->offset_CollGO_quat *= quaternion_radian_axis(anglar_velocity.norm_sqr() * duration * 0.5f, anglar_velocity.unit_vect());
	//ALP_coll->offset_CollGO_quat = ALP_coll->world_orientation().unit_vect();

	if (is_movable() == false)return;

	////�ʒu�̍X�V
	//if (linear_velocity.norm() >= FLT_EPSILON)
	//	ALP_coll->world_position_ += linear_velocity * duration;

	//ALP_coll->world_orientation_ *= quaternion_radian_axis(anglar_velocity.norm_sqr() * duration * 0.5f, anglar_velocity.unit_vect());
	//ALP_coll->world_orientation_ = ALP_coll->world_orientation().unit_vect();

	ALPcollider->integrate(duration, linear_velocity, anglar_velocity);

}

void ALP_Physics::add_tensor_type(Tensor_type type) {
	switch (tensor_type)
	{
	case Adollib::Physics_function::Tensor_type::None:
		tensor_type = type;
		break;

	case Adollib::Physics_function::Tensor_type::Sphere:
		//�A�^�b�`���ꂽshape��sphere�݂̂̎��A�]����₷�����邽�ߊ������[�����g��sphere�p�ɂ���
		tensor_type = type;
		break;

	case Adollib::Physics_function::Tensor_type::Box:
		//��ł�sphere�ȊO���������Ă����ꍇ �Ƃ肠�����֗���Box�p�̊������[�����g�ɂ���
		break;

	default:
		break;
	}
}
void ALP_Physics::refresh_tensor_type(const std::vector<Collider_shape*>& shapes) {
	tensor_type = Tensor_type::None;
	for (const auto& shape : shapes) {
		add_tensor_type(shape->get_tensor_type());
		if (tensor_type == Adollib::Physics_function::Tensor_type::Box)return;
	}
}

//�T�C�Y�ύX�ȂǂɑΉ����邽�ߖ��t���[�������e���\���Ȃǂ��X�V
void ALP_Physics::update_tensor(const std::vector<Collider_shape*>& shapes) {

	const int shapes_size = shapes.size();

	if (shapes_size == 0)return;
	if (shapes_size == 1) {
		shapes.at(0)->update_inertial_tensor(inertial_tensor, inertial_mass);
		return;
	}

	switch (tensor_type)
	{
	case Adollib::Physics_function::Tensor_type::None:
		return;
		break;

	case Adollib::Physics_function::Tensor_type::Sphere:
		break;

	case Adollib::Physics_function::Tensor_type::Box:
		break;

	default:
		break;
	}


	//if (tensor_type == Physics_function::Tensor_type::None)return;
	//else if (tensor_type == Physics_function::Tensor_type::Shape)shapes.at(1)->update_inertial_tensor(inertial_tensor, inertial_mass);
	//else if (shapes_size >= 2){
	//	switch (ALPcollider->shape)
	//	{
	//	case ALP_Collider_inertial_shape::box:
	//		inertial_tensor = matrix_identity();
	//		inertial_tensor._11 = 0.3333333f * inertial_mass * ((Wsize.y * Wsize.y) + (Wsize.z * Wsize.z));
	//		inertial_tensor._22 = 0.3333333f * inertial_mass * ((Wsize.z * Wsize.z) + (Wsize.x * Wsize.x));
	//		inertial_tensor._33 = 0.3333333f * inertial_mass * ((Wsize.x * Wsize.x) + (Wsize.y * Wsize.y));
	//		break;

	//	case ALP_Collider_inertial_shape::sphere:
	//		inertial_tensor = matrix_identity();
	//		inertial_tensor._11 = 0.4f * inertial_mass * Wsize.x * Wsize.x;
	//		inertial_tensor._22 = 0.4f * inertial_mass * Wsize.x * Wsize.x;
	//		inertial_tensor._33 = 0.4f * inertial_mass * Wsize.x * Wsize.x;
	//		break;
	//	}
	//}

	//Vector3 Wsize = gameobject->world_scale();

	////�������[�����g�̍X�V
	//switch (ALPcollider->shape)
	//{
	//case ALP_Collider_inertial_shape::box:
	//	inertial_tensor = matrix_identity();
	//	inertial_tensor._11 = 0.3333333f * inertial_mass * ((Wsize.y * Wsize.y) + (Wsize.z * Wsize.z));
	//	inertial_tensor._22 = 0.3333333f * inertial_mass * ((Wsize.z * Wsize.z) + (Wsize.x * Wsize.x));
	//	inertial_tensor._33 = 0.3333333f * inertial_mass * ((Wsize.x * Wsize.x) + (Wsize.y * Wsize.y));
	//	break;

	//case ALP_Collider_inertial_shape::sphere:
	//	inertial_tensor = matrix_identity();
	//	inertial_tensor._11 = 0.4f * inertial_mass * Wsize.x * Wsize.x;
	//	inertial_tensor._22 = 0.4f * inertial_mass * Wsize.x * Wsize.x;
	//	inertial_tensor._33 = 0.4f * inertial_mass * Wsize.x * Wsize.x;
	//	break;
	//}
}

void ALP_Physics::update_physics_data() {

	Physics_data Cdata = (*ALPcollider->get_collitr())->physics_data;

	inertial_mass = Cdata.inertial_mass;
	linear_drag = Cdata.drag;
	anglar_drag = Cdata.anglar_drag;
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