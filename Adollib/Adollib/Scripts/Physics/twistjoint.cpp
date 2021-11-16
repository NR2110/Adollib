#include "twistjoint.h"

#include "ALP_joint.h"
#include "ALP_collider.h"

#include "../Object/gameobject_manager.h"

using namespace Adollib;
using namespace Physics_function;

bool TwistJoint::limit_effect(Vector3& contactP0, Vector3& contactP1, float& penetrate) const {
	constexpr float power = 2; //�� �Ȃ����Ȃ��ƒ���t��

	const world_trans* transforms[2]{
		&ALPjoint->ALPcollider[0]->transform,
		&ALPjoint->ALPcollider[1]->transform
	};
	Vector3 V = vector3_quatrotate(vec0, transforms[0]->orientation);
	const Vector3 vec[2]{
		V,
		V
	};

	float radian_local[2] = { 0 };

	for (int i = 0; i < 2; ++i)
	{

		Vector3 vec_dir = vector3_quatrotate(vec[i], transforms[i]->orientation); //��]���twist�̎�

		Vector3 axis = vector3_cross(vec[i], vec_dir); //��]��ƌ�̂ǂ���ɂ������ȃx�N�g��
		if (axis.norm() < FLT_EPSILON) {
			// ��]�O�A�オ���s�̎� �K���Ȑ����Ȃ��̂���������
			if (fabsf(vector3_dot(vec[i], Vector3(1, 0, 0))) < FLT_EPSILON)axis = vector3_cross(vec[i], Vector3(1, 0, 0));
			else axis = vector3_cross(vec[i], Vector3(0, 1, 0));
		}
		axis = axis.unit_vect();


		Vector3 rotated_axis = vector3_quatrotate(axis, transforms[i]->orientation).unit_vect(); //��]��̐����ȃx�N�g��

		// ��]�̊p�x�𓾂�
		radian_local[i] = vector3_radian(rotated_axis, axis);
		if (vector3_dot(vector3_cross(rotated_axis, axis), vec_dir) < 0) {
			radian_local[i] = DirectX::XM_PI + DirectX::XM_PI - radian_local[i]; //0~180~0 �� -360~0~360�Ɏ���
			axis *= -1;
			rotated_axis *= -1;
		}


		if (0 && i == 0) {
			Debug::set("vec[i]", vec[i]);
			Debug::set("vec_dir", vec_dir);
			Debug::set("axis", axis);
			Debug::set("rotated_axis", rotated_axis);
			Debug::set("radian_local[i]", ToAngle(radian_local[i]));

			static Gameobject* debug[4] = { nullptr };
			if (debug[0] == nullptr) {
				debug[0] = Gameobject_manager::createCube();
				debug[1] = Gameobject_manager::createCube();
				debug[2] = Gameobject_manager::createCube();
				debug[3] = Gameobject_manager::createCube();

				debug[0]->transform->local_scale = Vector3(0.2f, 0.2f, 1);
				debug[1]->transform->local_scale = Vector3(0.2f, 0.2f, 1);
				debug[2]->transform->local_scale = Vector3(0.2f, 0.2f, 1);
				debug[3]->transform->local_scale = Vector3(0.2f, 0.2f, 1);

				debug[0]->material->color = Vector4(1, 0, 0, 1);
				debug[1]->material->color = Vector4(0, 0, 1, 1);
				debug[2]->material->color = Vector4(1, 0, 1, 1);
				debug[3]->material->color = Vector4(0, 1, 0, 1);
			}
			{
				debug[0]->transform->local_orient = quaternion_from_to_rotate(Vector3(0, 0, 1), vec[i]);
				debug[1]->transform->local_orient = quaternion_from_to_rotate(Vector3(0, 0, 1), vec_dir);
				debug[2]->transform->local_orient = quaternion_from_to_rotate(Vector3(0, 0, 1), axis);
				debug[3]->transform->local_orient = quaternion_from_to_rotate(Vector3(0, 0, 1), rotated_axis);

				debug[0]->transform->local_pos = transforms[i]->position + Vector3(0, 3, 0) + vec[i] * 2;
				debug[1]->transform->local_pos = transforms[i]->position + Vector3(0, 3, 0) + vec_dir * 2;
				debug[2]->transform->local_pos = transforms[i]->position + Vector3(0, 3, 0) + axis * 2;
				debug[3]->transform->local_pos = transforms[i]->position + Vector3(0, 3, 0) + rotated_axis * 2;
			}

		}


	}

	const Vector2 limit_rad = Vector2(ToRadian(limit.x), ToRadian(limit.y)); //limit��radian�Ɏ�����
	float radian = radian_local[1] - radian_local[0]; //radian�̍������� collider[1]�̉�]�ʂ��v�Z
	if (radian < 0)radian += 2 * PI;
	if (radian > 2 * PI)radian -= 2 * PI;

	// ����limit�̉e�����󂯂�ʒu�ɓ��Ȃ����false��return
	if (limit_rad.x <= limit_rad.y) {
		if (limit_rad.x <= radian && radian <= limit_rad.y)return false;
	}
	else if (limit_rad.y <= limit_rad.x) {
		if (!(limit_rad.y <= radian && radian <= limit_rad.x))return false;
	}

	// radian�����cos�ň�ԋ߂�limit�����߂�
	Vector2 limit_rad_off = Vector2(limit_rad.x - radian, limit_rad.y - radian);
	if (fabsf(limit_rad_off.x) > ToRadian(max_radian_pow))limit_rad_off.x *= ToRadian(max_radian_pow) * limit_rad_off.x / fabsf(limit_rad_off.x);
	if (fabsf(limit_rad_off.y) > ToRadian(max_radian_pow))limit_rad_off.y *= ToRadian(max_radian_pow) * limit_rad_off.y / fabsf(limit_rad_off.y);

	Vector3 axis;
	{
		int i = 1;
		Vector3 vec_dir = vector3_quatrotate(vec[i], transforms[i]->orientation); //��]���twist�̎�
		axis = vector3_cross(vec[i], vec_dir).unit_vect(); //��]��ƌ�̂ǂ���ɂ������ȃx�N�g��
		if (axis.norm() == 0) {
			// ��]�O�A�オ���s�̎� �K���Ȑ����Ȃ��̂���������
			if (vec[i].x != 1)axis = vector3_cross(vec[i], Vector3(1, 0, 0));
			else axis = vector3_cross(vec[i], Vector3(0, 1, 0));
		}
	}
	Vector3 tangent = vector3_cross(axis, vec1);

	// contactP1�̊��
	// contactP0�������Ă���
	if (cosf(limit_rad_off.x) < cosf(limit_rad_off.y)) {

		penetrate = DirectX::XM_2PI * power * fabsf(limit_rad_off.y) * DirectX::XM_1DIV2PI; //�]���Ȍʂ̒���

		Vector3 contactP0_world = vector3_quatrotate((axis - tangent * penetrate), transforms[1]->orientation) + transforms[1]->position;
		// limit_x�̂ق����߂�
		contactP0 = vector3_quatrotate(contactP0_world - transforms[0]->position, transforms[0]->orientation.inverse());
		contactP1 = axis;

	}
	else {
		// Vector3 contactP0_world = vector3_quatrotate(axis_comp0coord, quaternion_axis_radian(vec1_comp0coord, -limit_rad_off.x));

		penetrate = DirectX::XM_2PI * power * fabsf(limit_rad_off.x) * DirectX::XM_1DIV2PI; //�]���Ȍʂ̒���

		Vector3 contactP0_world = vector3_quatrotate((axis + tangent * penetrate), transforms[1]->orientation) + transforms[1]->position;

		// limit_y�̂ق����߂�
		contactP0 = vector3_quatrotate(contactP0_world - transforms[0]->position, transforms[0]->orientation.inverse());
		contactP1 = axis;

	}

	return true;
}