#include "collider__base.h"

using namespace Adollib;
using namespace Contacts;

#include "gameobject.h"
#include "Adollib.h"
#include "contact.h"

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
		#pragma region Rigitbody
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

void Collider::apply_external_force(float duration) {
	if (is_movable()) {

		//���i�ړ��ɉ������(accumulated_force)��������x���o���ĕ��i���x���X�V����
		liner_acceleration += accumulated_force / inertial_mass;
		linear_velocity += liner_acceleration * duration;
		

		//�e��]�ɉ������(accumulated_torque)��������x���o���Ċp���x���X�V����
		matrix inverse_inertia_tensor = matrix_inverse(inertial_tensor);
		matrix rotation = local_orientation.get_rotate_matrix();
		matrix transposed_rotation = matrix_trans(rotation);
		inverse_inertia_tensor = transposed_rotation * inverse_inertia_tensor * rotation;
		angular_acceleration += vector3_trans(accumulated_torque, inverse_inertia_tensor);

		angula_velocity += angular_acceleration * duration;
		if (angula_velocity.norm() < FLT_EPSILON)angula_velocity = vector3(0, 0, 0);
	}

	//������0�ɂ���
	accumulated_force = vector3(0, 0, 0);
	accumulated_torque = vector3(0, 0, 0);

	liner_acceleration = vector3(0, 0, 0);
	angular_acceleration = vector3(0, 0, 0);
}
void Collider::integrate(float duration) {
	//�ʒu�̍X�V
	if (linear_velocity.norm() >= FLT_EPSILON)
		world_position += linear_velocity * duration;

	world_orientation *= quaternion_radian_axis(angula_velocity.norm_sqr() * duration * 0.5, angula_velocity.unit_vect());
	world_orientation = world_orientation.unit_vect();

}


void Collider::resolve_gameobject() {
	gameobject->co_e.orient *= local_orientation.conjugate() * gameobject->get_world_orientate().conjugate() * world_orientation;
	gameobject->co_e.position += world_position -  vector3_be_rotated_by_quaternion(local_position * gameobject->get_world_scale(), world_orientation) - gameobject->get_world_position();
	
}

void Collider::add_force(const vector3& force) {
	accumulated_force += force;
}
void Collider::add_torque(const vector3& force) {
	accumulated_torque += force;
}

bool Collider::is_movable() const {
	return (move && inertial_mass > 0 && inertial_mass < FLT_MAX);
}
float Collider::inverse_mass() const {
	if (is_movable()) return 1.0f / inertial_mass;
	//����I�u�W�F�N�g�Ȃ�0��Ԃ�
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
		//�ゾ�ƃI�u�W�F�N�g�̒��S���痣�ꂽ�Փ˔���Ńo�O��@���ɂ��ׂ�
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





//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
