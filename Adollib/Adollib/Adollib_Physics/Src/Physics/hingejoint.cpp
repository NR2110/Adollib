#include "hingejoint.h"

#include "ALP_joint.h"
#include "ALP_collider.h"
#include "ALP_physics.h"

using namespace Adollib;
using namespace Physics_function;


void HingeJoint::velocity_effect() const {
	// �Е����Œ肳��Ă���Ƃ��A�������̉�]���𒲐�����


	int moveable_num = -1;
	if (ALPjoint->ALPcollider[0]->get_ALPphysics()->is_movable() == true)moveable_num += 1;
	if (ALPjoint->ALPcollider[1]->get_ALPphysics()->is_movable() == true)moveable_num += 2;

	// �ǂ��������or�����Ȃ��̂ł���� return
	if (moveable_num == -1 || moveable_num == 2)return;

	// transform
	const world_trans* transforms[2]{
		&ALPjoint->ALPcollider[0]->transform,
		&ALPjoint->ALPcollider[1]->transform
	};
	//Hinge�̌���
	const Vector3 hinge_vec[2] = {
		(anchor_s.posA - anchor_g.posA).unit_vect(),
		(anchor_s.posB - anchor_g.posB).unit_vect()
	};
	//Hinge��world_vec
	const Vector3 hinge_vec_world[2] = {
		vector3_quatrotate(hinge_vec[0],transforms[0]->orientation),
		vector3_quatrotate(hinge_vec[1],transforms[1]->orientation),
	};
	//��]
	Vector3 angular_velocity[2] = {
		ALPjoint->ALPcollider[0]->get_ALPphysics()->angula_velocity(),
		ALPjoint->ALPcollider[1]->get_ALPphysics()->angula_velocity()
	};

	{
		angular_velocity[moveable_num] = hinge_vec_world[moveable_num] * vector3_dot(hinge_vec_world[moveable_num], angular_velocity[moveable_num]);
		ALPjoint->ALPcollider[moveable_num]->get_ALPphysics()->set_angula_velocity(angular_velocity[moveable_num]);
	}




}

bool HingeJoint::limit_effect(Vector3& contactP0, Vector3& contactP1, float& penetrate) const {
	constexpr float power = 2; //�� �Ȃ����Ȃ��ƒ���t��

	// transform
	const world_trans* transforms[2]{
		&ALPjoint->ALPcollider[0]->transform,
		&ALPjoint->ALPcollider[1]->transform
	};

	//Hinge�̌���
	const Vector3 hinge_vec[2] = {
		(anchor_s.posA - anchor_g.posA).unit_vect(),
		(anchor_s.posB - anchor_g.posB).unit_vect()
	};
	//Hinge��world_vec
	const Vector3 hinge_vec_world[2] = {
		vector3_quatrotate(hinge_vec[0],transforms[0]->orientation),
		vector3_quatrotate(hinge_vec[1],transforms[1]->orientation),
	};
	//Hinge��90�x��]������local_vec
	const Vector3 hinge_vec_rot90[2] = {
		vector3_quatrotate(Vector3(1,0,0),quaternion_from_to_rotate(Vector3(0,1,0),hinge_vec[0])),
		vector3_quatrotate(Vector3(1,0,0),quaternion_from_to_rotate(Vector3(0,1,0),hinge_vec[1]))
	};
	const Vector3 hinge_vec_rot90_world[2] = {
		vector3_quatrotate(hinge_vec_rot90[0],transforms[0]->orientation).unit_vect(),
		vector3_quatrotate(hinge_vec_rot90[1],transforms[1]->orientation).unit_vect(),
	};

	//�p�x�𓾂�
	float radian = vector3_radian(hinge_vec_rot90_world[0], hinge_vec_rot90_world[1]);
	if (vector3_dot(hinge_vec_world[0], vector3_cross(hinge_vec_rot90_world[0], hinge_vec_rot90_world[1])) < 0) {
		radian = DirectX::XM_PI + DirectX::XM_PI - radian; //0~180~0 �� 0~360�Ɏ���
	};

	const Vector2 limit_rad = Vector2(ToRadian(limit.x), ToRadian(limit.y)); //limit��radian�Ɏ�����

	// ����limit�̉e�����󂯂�ʒu�ɓ��Ȃ����false��return
	if (limit_rad.x <= limit_rad.y) {
		if (limit_rad.x <= radian && radian <= limit_rad.y)return false;
	}
	else if (limit_rad.y <= limit_rad.x) {
		if (!(limit_rad.y <= radian && radian <= limit_rad.x))return false;
	}

	//radian�����cos�ň�ԋ߂�limit�����߂�
	const Vector2 limit_rad_off = Vector2(limit_rad.x - radian, limit_rad.y - radian);

	Vector3 tangent = vector3_cross(hinge_vec_rot90[1], hinge_vec[1]);

	//contactP1�̊��
	//contactP0�������Ă���

	float scale = (fabsf(vector3_dot(transforms[1]->scale, hinge_vec[1])) + fabsf(vector3_dot(transforms[0]->scale, hinge_vec[0]))) * 0.5f * 10;

	if (cosf(limit_rad_off.x) < cosf(limit_rad_off.y)) {

		int p = 1;
		float off = fabsf(limit_rad_off.y);
		if (off > +DirectX::XM_PI) off = DirectX::XM_2PI - off;
		if (off < -DirectX::XM_PI) off = DirectX::XM_2PI + off;
		if (off != fabsf(limit_rad_off.y))p *= -1;
		penetrate = DirectX::XM_2PI * power * off * DirectX::XM_1DIV2PI * scale; //�]���Ȍʂ̒���

		Vector3 contactP0_world = vector3_quatrotate((hinge_vec_rot90[1] * scale + tangent * penetrate), transforms[1]->orientation) + transforms[1]->position;

		//limit_x�̂ق����߂�
		contactP0 = vector3_quatrotate(contactP0_world - transforms[0]->position, transforms[0]->orientation.inverse());
		contactP1 = hinge_vec_rot90[1] * scale;

	}
	else {

		int p = 1;
		float off = fabsf(limit_rad_off.x);
		if (off > +DirectX::XM_PI) off = DirectX::XM_2PI - off;
		if (off < -DirectX::XM_PI) off = DirectX::XM_2PI + off;
		if (off != fabsf(limit_rad_off.x))p *= -1;
		penetrate = DirectX::XM_2PI * power * off * DirectX::XM_1DIV2PI * scale; //�]���Ȍʂ̒���

		Vector3 contactP0_world = vector3_quatrotate((hinge_vec_rot90[1] * scale + p * tangent * penetrate), transforms[1]->orientation) + transforms[1]->position;

		//limit_y�̂ق����߂�
		contactP0 = vector3_quatrotate(contactP0_world - transforms[0]->position, transforms[0]->orientation.inverse());
		contactP1 = hinge_vec_rot90[1] * scale;

	}

	return true;
}