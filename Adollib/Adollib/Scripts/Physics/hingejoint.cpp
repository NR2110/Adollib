#include "hingejoint.h"

#include "ALP_joint.h"
#include "ALP_collider.h"

using namespace Adollib;
using namespace Physics_function;

bool HingeJoint::limit_effect(Vector3& contactP0, Vector3& contactP1, float& penetrate) const {
	constexpr float power = 2; //“ä ‚È‚º‚©‚È‚¢‚Æ’£‚è•t‚­

	// transform
	const world_trans* transforms[2]{
		&ALPjoint->ALPcollider[0]->transform,
		&ALPjoint->ALPcollider[1]->transform
	};

	//Hinge‚ÌŒü‚«
	const Vector3 hinge_vec[2] = {
		(anchor_s.posA - anchor_g.posA).unit_vect(),
		(anchor_s.posB - anchor_g.posB).unit_vect()
	};
	//Hinge‚Ìworld_vec
	const Vector3 hinge_vec_world[2] = {
		vector3_quatrotate(hinge_vec[0],transforms[0]->orientation),
		vector3_quatrotate(hinge_vec[1],transforms[1]->orientation),
	};
	//Hinge‚ð90“x‰ñ“]‚³‚¹‚½local_vec
	const Vector3 hinge_vec_rot90[2] = {
		vector3_quatrotate(Vector3(1,0,0),quaternion_from_to_rotate(Vector3(0,1,0),hinge_vec[0])),
		vector3_quatrotate(Vector3(1,0,0),quaternion_from_to_rotate(Vector3(0,1,0),hinge_vec[1]))
	};
	const Vector3 hinge_vec_rot90_world[2] = {
		vector3_quatrotate(hinge_vec_rot90[0],transforms[0]->orientation).unit_vect(),
		vector3_quatrotate(hinge_vec_rot90[1],transforms[1]->orientation).unit_vect(),
	};

	//Šp“x‚ð“¾‚é
	float radian = vector3_radian(hinge_vec_rot90_world[0], hinge_vec_rot90_world[1]);
	if (vector3_dot(hinge_vec_world[0], vector3_cross(hinge_vec_rot90_world[0], hinge_vec_rot90_world[1])) < 0) {
		radian = DirectX::XM_PI + DirectX::XM_PI - radian; //0~180~0 ‚ð 0~360‚ÉŽ¡‚·
	};

	const Vector2 limit_rad = Vector2(ToRadian(limit.x), ToRadian(limit.y)); //limit‚ðradian‚ÉŽ¡‚µ‚½

	Debug::set("hinge_angle", ToAngle(radian));

	// ‚à‚µlimit‚Ì‰e‹¿‚ðŽó‚¯‚éˆÊ’u‚É“ü‚È‚¯‚ê‚Îfalse‚ðreturn
	if (limit_rad.x <= limit_rad.y) {
		if (limit_rad.x <= radian && radian <= limit_rad.y)return false;
	}
	else if (limit_rad.y <= limit_rad.x) {
		if (!(limit_rad.y <= radian && radian <= limit_rad.x))return false;
	}

	//radian‚ðŠî€‚Écos‚Åˆê”Ô‹ß‚¢limit‚ð‹‚ß‚é
	const Vector2 limit_rad_off = Vector2(limit_rad.x - radian, limit_rad.y - radian);

	//contactP1‚ÌŠî€‚É
	//contactP0‚ðŽ‚Á‚Ä‚­‚é
	if (cosf(limit_rad_off.x) < cosf(limit_rad_off.y)) {
		Vector3 contactP0_world = vector3_quatrotate(hinge_vec_rot90_world[1], quaternion_axis_radian(hinge_vec_world[1], limit_rad_off.y)) + transforms[1]->position;

		//limit_x‚Ì‚Ù‚¤‚ª‹ß‚¢
		contactP0 = vector3_quatrotate(contactP0_world - transforms[0]->position, transforms[0]->orientation.inverse());
		contactP1 = hinge_vec_rot90[1];

		penetrate = 2 * cosf(DirectX::XM_PIDIV2 - fabsf(limit_rad_off.y) * 0.5f) * power;
	}
	else {
		Vector3 contactP0_world = vector3_quatrotate(hinge_vec_rot90_world[1], quaternion_axis_radian(hinge_vec_world[1], limit_rad_off.x)) + transforms[1]->position;

		//limit_y‚Ì‚Ù‚¤‚ª‹ß‚¢
		contactP0 = vector3_quatrotate(contactP0_world - transforms[0]->position, transforms[0]->orientation.inverse());
		contactP1 = hinge_vec_rot90[1];

		penetrate = 2 * cosf(DirectX::XM_PIDIV2 - fabsf(limit_rad_off.x) * 0.5f) * power;
	}

	return true;
}