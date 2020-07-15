#include "rigit_body.h"

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

		angular_velocity += angular_acceleration * duration;
		if (angular_velocity.norm() < FLT_EPSILON)angular_velocity = vector3(0, 0, 0);

		//QUESTION : ��̎�����_���Ȃ�?
		//�e���x�ɂ��p���̍X�V
#if 1

#else 
		//�p���x�ɂ��p���̍X�V
		quaternion w(0, angular_velocity.x, angular_velocity.y, angular_velocity.z);
		w = orientation * w;
		orientation += 0.5f * w * duration;
		orientation = orientation.unit_vect();
#endif
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

	world_orientation *= quaternion_radian_axis(angular_velocity.norm_sqr() * duration * 0.5, angular_velocity.unit_vect());
	world_orientation = world_orientation.unit_vect();

	//quaternion dAng = quaternion(0, angular_velocity.x, angular_velocity.y, angular_velocity.z) * world_orientation * 0.5f;
	//world_orientation = (world_orientation + dAng * duration).unit_vect();

}


void Collider::resolve_gameobject() {
	gameobject->co_e.orient *= local_orientation.conjugate() * gameobject->get_world_orientate().conjugate() * world_orientation;
	gameobject->co_e.position += world_position -  vector3_be_rotated_by_quaternion(local_position * gameobject->get_world_scale(), world_orientation) - gameobject->get_world_position();
	
}
void Box::update_world_trans() {
	world_orientation = gameobject->get_world_orientate() * local_orientation;
	world_size = gameobject->get_world_scale() * local_scale * half_size;
	world_position = gameobject->get_world_position() + vector3_be_rotated_by_quaternion(local_position * gameobject->get_world_scale(), world_orientation);

	update_inertial(world_size, density);
}
void Sphere::update_world_trans() {
	world_orientation = gameobject->get_world_orientate() * local_orientation;
	world_size = gameobject->get_world_scale() * local_scale * r;
	world_position = gameobject->get_world_position() + vector3_be_rotated_by_quaternion(local_position * world_size, world_orientation);
	update_inertial(world_size, density);
}
void Plane::update_world_trans() {
	world_orientation = gameobject->get_world_orientate() * local_orientation;
	world_size = gameobject->get_world_scale() * local_scale;
	world_position = gameobject->get_world_position() + vector3_be_rotated_by_quaternion(local_position * world_size, world_orientation);

	update_inertial(world_size, density);
}
void Box::update_dop7() {
	dop7.pos = gameobject->get_world_position();

	//�e���_�̃��[�J�����W
	vector3 half[4]{
	{+half_size.x, -half_size.y, -half_size.z},
	{+half_size.x, -half_size.y, +half_size.z},
	{+half_size.x, +half_size.y, -half_size.z},
	{+half_size.x, +half_size.y, +half_size.z}
	};

	for (int i = 0; i < 4; i++) {
		half[i] = half[i] * world_size;
	}

	quaternion WO = gameobject->get_world_orientate();
	for (int i = 0; i < 4; i++) {
		half[i] = vector3_be_rotated_by_quaternion(half[i], WO);
	}

	//DOP_6�̍X�V
	float max_len = 0;
	for (int i = 0; i < DOP_size; i++) {
		max_len = 0;
		for (int o = 0; o < 4; o++) {
			float dis = fabsf(vector3_dot(DOP_7_axis[i], half[o]));
			if (max_len < dis) {
				dop7.max[i] = +dis * 1.01f;//�m���ɂ��邽�߂�����Ƒ傫�߂ɂƂ�
				dop7.min[i] = -dis * 1.01f;//�m���ɂ��邽�߂�����Ƒ傫�߂ɂƂ�
				max_len = dis;
			}

		}
	}

}
void Sphere::update_dop7() {
	dop7.pos = gameobject->get_world_position();	
	for (int i = 0; i < DOP_size; i++) {
		dop7.max[i] = r * 1.01 * world_size.x;
		dop7.min[i] = r * 1.01 * world_size.x;
	}
}
void Plane::update_dop7() {
	dop7.pos = gameobject->get_world_position();
	for (int i = 0; i < DOP_size; i++) {
		dop7.max[i] = FLT_MAX;
	}
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
		// QUESTION : �ゾ�ƃI�u�W�F�N�g�̒��S���痣�ꂽ�Փ˔���Ńo�O��@���ɂ��ׂ�?
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
