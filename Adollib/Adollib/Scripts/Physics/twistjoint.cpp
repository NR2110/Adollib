#include "twistjoint.h"

#include "ALP_joint.h"
#include "ALP_collider.h"

using namespace Adollib;
using namespace Physics_function;

bool TwistJoint::limit_effect(Vector3& contactP0, Vector3& contactP1, float& penetrate) const {
	constexpr float power = 2; //�� �Ȃ����Ȃ��ƒ���t��

	const world_trans* transforms[2]{
		&ALPjoint->ALPcollider[0]->transform,
		&ALPjoint->ALPcollider[1]->transform
	};
	const Vector3 vec[2]{
		vec0,
		vec1
	};

	float radian_local[2] = {0};

	for(int i = 0;i < 2;++i)
	{
		Vector3 vec_dir = vector3_quatrotate(vec[i], transforms[i]->orientation); //��]���twist�̎�

		Vector3 axis = vector3_cross(vec[i], vec_dir).unit_vect(); //��]��ƌ�̂ǂ���ɂ������ȃx�N�g��
		if (axis.norm() == 0) {
			// ��]�O�A�オ���s�̎� �K���Ȑ����Ȃ��̂���������
			if (vec[i].x != 1)axis = vector3_cross(vec[i], Vector3(1, 0, 0));
			else axis = vector3_cross(vec[i], Vector3(0, 1, 0));
		}

		Vector3 rotated_axis = vector3_quatrotate(axis, transforms[i]->orientation).unit_vect(); //��]��̐����ȃx�N�g��

		//if (vector3_dot(vec1_dir, vec1) < 0) {
		//	// 90�x�ȏ����Ă��邽�ߊp�x�����������Ȃ�̂�h��
		//	rotated_axis *= -1;
		//}

		// ��]�̊p�x�𓾂�
		radian_local[i] = vector3_radian(rotated_axis, axis);
		if (vector3_dot(vector3_cross(rotated_axis, axis), vec_dir) < 0) {
			radian_local[i] = DirectX::XM_PI + DirectX::XM_PI - radian_local[i]; //0~180~0 �� 0~360�Ɏ���
			axis *= -1;
			rotated_axis *= -1;
		}
	}

	const Vector2 limit_rad = Vector2(ToRadian(limit.x), ToRadian(limit.y)); //limit��radian�Ɏ�����
	const float radian = radian_local[1] - radian_local[0]; //radian�̍������� collider[1]�̉�]�ʂ��v�Z
	// Debug::set("angle", ToAngle(radian));

	// ����limit�̉e�����󂯂�ʒu�ɓ��Ȃ����false��return
	if (limit_rad.x <= limit_rad.y) {
		if (limit_rad.x <= radian && radian <= limit_rad.y)return false;
	}
	else if (limit_rad.y <= limit_rad.x) {
		if (!(limit_rad.y <= radian && radian <= limit_rad.x))return false;
	}

	// radian�����cos�ň�ԋ߂�limit�����߂�
	const Vector2 limit_rad_off = Vector2(limit_rad.x - radian, limit_rad.y - radian);

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




	//// collider_comp[1]����collider_comp[0]�ւ̃I�t�Z�b�g (collider_comp[1]��local����collider_comp[0]�̍��W�n�ɂ���)
	//Quaternion off_comp1to0 = transforms[1]->orientation * transforms[0]->orientation.inverse();

	//// collider_comp[0]�̍��W�n�ł���
	//Vector3 vec1_comp0coord = vector3_quatrotate(vec1, off_comp1to0).unit_vect();

	//// ��]�O�A��]��̂��݂��ɐ����ȃx�N�g�� -> z��]�ȊO�̉e�����󂯂Ȃ�
	//Vector3 axis = vector3_cross(vec1, vec1_comp0coord);
	//{
	//	Vector3 rot_axis = vector3_cross(Vector3(0, 1, 0), vec1);
	//	if (vector3_cross(rot_axis, vec1).norm() < FLT_EPSILON) {
	//		rot_axis = vector3_cross(Vector3(1, 0, 0), vec1);
	//	}
	//	axis = vector3_quatrotate(vec1, quaternion_axis_angle(rot_axis, 90));
	//}

	//if (axis.norm() < FLT_EPSILON) {
	//	//2�x�N�g�������s�Ȃ�K���Ȑ����Ȏ��ł悢
	//	axis = vector3_cross(vec1, vec1_comp0coord + Vector3(1, 0, 0));
	//}
	//axis = axis.unit_vect();

	//// axis�̉�]��̃x�N�g��
	//Vector3 axis_comp0coord = vector3_quatrotate(axis, off_comp1to0);

	//// z��]�̊p�x�𓾂�
	//float radian = vector3_radian(axis_comp0coord, axis);
	//if (vector3_dot(vector3_cross(axis_comp0coord, axis), vec1_comp0coord) < 0) {
	//	radian = DirectX::XM_PI + DirectX::XM_PI - radian; //0~180~0 �� 0~360�Ɏ���
	//	axis *= -1;
	//	axis_comp0coord *= -1;
	//};

	//const Vector2 limit_rad = Vector2(ToRadian(limit.x), ToRadian(limit.y)); //limit��radian�Ɏ�����

	//// Debug::set("angle", ToAngle(radian));

	//// ����limit�̉e�����󂯂�ʒu�ɓ��Ȃ����false��return
	//if (limit_rad.x <= limit_rad.y) {
	//	if (limit_rad.x <= radian && radian <= limit_rad.y)return false;
	//}
	//else if (limit_rad.y <= limit_rad.x) {
	//	if (!(limit_rad.y <= radian && radian <= limit_rad.x))return false;
	//}

	//// radian�����cos�ň�ԋ߂�limit�����߂�
	//const Vector2 limit_rad_off = Vector2(limit_rad.x - radian, limit_rad.y - radian);

	//Vector3 tangent = vector3_cross(axis, vec1);

	//// contactP1�̊��
	//// contactP0�������Ă���
	//if (cosf(limit_rad_off.x) < cosf(limit_rad_off.y)) {

	//	penetrate = DirectX::XM_2PI * power * fabsf(limit_rad_off.y) * DirectX::XM_1DIV2PI; //�]���Ȍʂ̒���

	//	Vector3 contactP0_world = vector3_quatrotate((axis - tangent * penetrate), transforms[1]->orientation) + transforms[1]->position;
	//	// limit_x�̂ق����߂�
	//	contactP0 = vector3_quatrotate(contactP0_world - transforms[0]->position, transforms[0]->orientation.inverse());
	//	contactP1 = axis;

	//}
	//else {
	//	// Vector3 contactP0_world = vector3_quatrotate(axis_comp0coord, quaternion_axis_radian(vec1_comp0coord, -limit_rad_off.x));

	//	penetrate = DirectX::XM_2PI * power * fabsf(limit_rad_off.x) * DirectX::XM_1DIV2PI; //�]���Ȍʂ̒���

	//	Vector3 contactP0_world = vector3_quatrotate((axis + tangent * penetrate), transforms[1]->orientation) + transforms[1]->position;

	//	// limit_y�̂ق����߂�
	//	contactP0 = vector3_quatrotate(contactP0_world - transforms[0]->position, transforms[0]->orientation.inverse());
	//	contactP1 = axis;

	//}

	return true;
}