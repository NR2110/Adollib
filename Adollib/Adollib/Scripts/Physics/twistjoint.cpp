#include "twistjoint.h"

#include "ALP_joint.h"
#include "ALP_collider.h"

#include "../Object/gameobject_manager.h"

using namespace Adollib;
using namespace Physics_function;

bool TwistJoint::limit_effect(Vector3& contactP0, Vector3& contactP1, float& penetrate) const {
	constexpr float power = 1; //�� �Ȃ����Ȃ��ƒ���t��

	const world_trans* transforms[2]{
		&ALPjoint->ALPcollider[0]->transform,
		&ALPjoint->ALPcollider[1]->transform
	};

	float radian = 0;
	{
		// quaternoin��offset
		Quaternion off = transforms[1]->orientation * transforms[0]->orientation.inverse();

		// ��]���twist�̎�
		Vector3 vec_dir = vector3_quatrotate(vec0, off);

		Vector3 axis = vector3_cross(vec0, vec_dir); //��]��ƌ�̂ǂ���ɂ������ȃx�N�g��
		if (axis.norm() < FLT_EPSILON) {
			// ��]�O�A�オ���s�̎� �K���Ȑ����Ȃ��̂���������
			if (fabsf(vector3_dot(vec0, Vector3(1, 0, 0))) < FLT_EPSILON)axis = vector3_cross(vec0, Vector3(1, 0, 0));
			else axis = vector3_cross(vec0, Vector3(0, 1, 0));
		}
		axis = axis.unit_vect();

		Vector3 rotated_axis = vector3_quatrotate(axis, off).unit_vect(); //��]��̐����ȃx�N�g��

		// ��]�̊p�x�𓾂�
		radian = vector3_radian(rotated_axis, axis);
		if (vector3_dot(vector3_cross(rotated_axis, axis), vec_dir) < 0) {
			radian = DirectX::XM_PI + DirectX::XM_PI - radian; //0~180~0 �� -360~0~360�Ɏ���
			axis *= -1;
			rotated_axis *= -1;
		}

	}

	const Vector2 limit_rad = Vector2(ToRadian(limit.x), ToRadian(limit.y)); //limit��radian�Ɏ�����
	//float radian = radian_local[1] - radian_local[0]; //radian�̍������� collider[1]�̉�]�ʂ��v�Z
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
	//if (fabsf(limit_rad_off.x) > ToRadian(max_radian_pow))limit_rad_off.x = ToRadian(max_radian_pow) * limit_rad_off.x / fabsf(limit_rad_off.x);
	//if (fabsf(limit_rad_off.y) > ToRadian(max_radian_pow))limit_rad_off.y = ToRadian(max_radian_pow) * limit_rad_off.y / fabsf(limit_rad_off.y);

	Vector3 axis;
	{
		int i = 1;
		Vector3 vec_dir = vector3_quatrotate(vec0, transforms[i]->orientation); //��]���twist�̎�
		axis = vector3_cross(vec0, vec_dir).unit_vect(); //��]��ƌ�̂ǂ���ɂ������ȃx�N�g��
		if (axis.norm() == 0) {
			// ��]�O�A�オ���s�̎� �K���Ȑ����Ȃ��̂���������
			if (vec0.x != 1)axis = vector3_cross(vec0, Vector3(1, 0, 0));
			else axis = vector3_cross(vec0, Vector3(0, 1, 0));
		}
	}
	Vector3 tangent = vector3_cross(axis, vec1);

	// contactP1�̊��
	// contactP0�������Ă���
	if (cosf(limit_rad_off.x) < cosf(limit_rad_off.y)) {

		//penetrate = DirectX::XM_2PI * power * fabsf(limit_rad_off.y) * DirectX::XM_1DIV2PI; //�]���Ȍʂ̒���
		int p = 1;
		float off = limit_rad_off.y;
		if (off < 0)off *= -1;
		//if (off > +DirectX::XM_PI) off = DirectX::XM_2PI - off;
		//if (off < -DirectX::XM_PI) off = DirectX::XM_2PI + off;
		if (off != limit_rad_off.y)p *= -1;
		penetrate = DirectX::XM_2PI * power * off * DirectX::XM_1DIV2PI; //�]���Ȍʂ̒���

		Vector3 contactP0_world = vector3_quatrotate((axis - tangent * penetrate), transforms[1]->orientation) + transforms[1]->position;
		// limit_x�̂ق����߂�
		contactP0 = vector3_quatrotate(contactP0_world - transforms[0]->position, transforms[0]->orientation.inverse());
		contactP1 = axis;

	}
	else {

		//penetrate = DirectX::XM_2PI * power * fabsf(limit_rad_off.x) * DirectX::XM_1DIV2PI; //�]���Ȍʂ̒���
		int p = 1;
		float off = limit_rad_off.x;
		if (off < 0)off *= -1;
		//if (off > +DirectX::XM_PI) off = DirectX::XM_2PI - off;
		//if (off < -DirectX::XM_PI) off = DirectX::XM_2PI + off;
		if (off != limit_rad_off.x)p *= -1;
		penetrate = DirectX::XM_2PI * power * off * DirectX::XM_1DIV2PI; //�]���Ȍʂ̒���

		Vector3 contactP0_world = vector3_quatrotate((axis + tangent * penetrate), transforms[1]->orientation) + transforms[1]->position;

		// limit_y�̂ق����߂�
		contactP0 = vector3_quatrotate(contactP0_world - transforms[0]->position, transforms[0]->orientation.inverse());
		contactP1 = axis;

	}

	return true;
}