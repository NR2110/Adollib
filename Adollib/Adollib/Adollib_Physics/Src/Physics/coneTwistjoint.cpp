#include "coneTwistjoint.h"

#include "ALP_joint.h"
#include "ALP_collider.h"

using namespace Adollib;
using namespace Physics_function;

bool ConetwistJoint::limit_effect(Vector3& contactP0, Vector3& contactP1, float& penetrate) const {

	constexpr float power = 2; //�� �Ȃ����Ȃ��ƒ���t��

	const world_trans* transforms[2]{
		&ALPjoint->ALPcollider[0]->transform,
		&ALPjoint->ALPcollider[1]->transform
	};

	//limit_axis��world�Ɏ����Ă���
	const Vector3 limit_axis_world[2] = {
		vector3_quatrotate(limit_axis[0],transforms[0]->orientation) * power,
		vector3_quatrotate(limit_axis[1],transforms[1]->orientation) * power
	};

	Vector3 debug_limit_axis_world[2] = {
		vector3_quatrotate(limit_axis[0],transforms[0]->orientation),
		vector3_quatrotate(limit_axis[1],transforms[1]->orientation)
	};

	//�p�x�𓾂�
	const float radian = vector3_radian(limit_axis_world[0], limit_axis_world[1]); // 0 - ��

	const float limit_rad = ToRadian(limit); //limit����radian�Ɏ�����

	//Debug::set("angle", ToAngle(radian));
	//Debug::set("limit_axis_world[0]", limit_axis_world[0].unit_vect());
	//Debug::set("limit_axis_world[1]", limit_axis_world[1].unit_vect());

	// ����limit�̉e�����󂯂�ʒu�ɓ��Ȃ����false��return
	if (radian <= limit_rad + FLT_EPSILON) return false;

	//collider_comp[0]��axis����ɂ��čl����

	//limit_axis_world[0]����ɂ���limit��vector_worldcoord
	//Vector3 limit_world = vector3_quatrotate(limit_axis_world[0], quaternion_radian_axis(limit_rad, vector3_cross(limit_axis_world[0], limit_axis_world[1])));


	penetrate = DirectX::XM_2PI * power * (radian - limit_rad) * DirectX::XM_1DIV2PI; //�]���Ȍʂ̒���

	const Vector3 p1_pos_world = vector3_quatrotate(anchor.posB + limit_axis[1] * power, transforms[1]->orientation) + transforms[1]->position;
	const Vector3 rotate_axis = vector3_cross(limit_axis_world[0], limit_axis_world[1]).unit_vect();

	const Vector3 tangent = vector3_cross(limit_axis_world[1], rotate_axis).unit_vect();


	//Vector3 p1_limit_pos_world = vector3_quatrotate((anchor.posB + limit_axis[1]), transforms[1]->orientation) + transforms[1]->position;
	const Vector3 p1_limit_pos_0coord = vector3_quatrotate((p1_pos_world + tangent * penetrate) - transforms[0]->position, transforms[0]->orientation.inverse());
	{

		//contactP0 = anchor.posA + vector3_quatrotate(p1_limit_vec_world, transforms[0]->orientation.inverse());
		contactP0 = p1_limit_pos_0coord;
		contactP1 = anchor.posB + limit_axis[1] * power;
		//contactP1 = vector3_quatrotate(limit_world, collider_comp[1]->transform->orientation.inverse());
		//contactP0 = limit_axis[0];


		//penetrate = (p1_limit_vec_world - limit_axis_world[1]).norm_sqr() * 10;
	}

	return true;
}