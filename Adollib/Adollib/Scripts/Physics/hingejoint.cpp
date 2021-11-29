#include "hingejoint.h"

#include "ALP_joint.h"
#include "ALP_collider.h"

using namespace Adollib;
using namespace Physics_function;

bool HingeJoint::limit_effect(Vector3& contactP0, Vector3& contactP1, float& penetrate) const {
	constexpr float power = 2; //謎 なぜかないと張り付く

	// transform
	const world_trans* transforms[2]{
		&ALPjoint->ALPcollider[0]->transform,
		&ALPjoint->ALPcollider[1]->transform
	};

	//Hingeの向き
	const Vector3 hinge_vec[2] = {
		(anchor_s.posA - anchor_g.posA).unit_vect(),
		(anchor_s.posB - anchor_g.posB).unit_vect()
	};
	//Hingeのworld_vec
	const Vector3 hinge_vec_world[2] = {
		vector3_quatrotate(hinge_vec[0],transforms[0]->orientation),
		vector3_quatrotate(hinge_vec[1],transforms[1]->orientation),
	};
	//Hingeを90度回転させたlocal_vec
	const Vector3 hinge_vec_rot90[2] = {
		vector3_quatrotate(Vector3(1,0,0),quaternion_from_to_rotate(Vector3(0,1,0),hinge_vec[0])),
		vector3_quatrotate(Vector3(1,0,0),quaternion_from_to_rotate(Vector3(0,1,0),hinge_vec[1]))
	};
	const Vector3 hinge_vec_rot90_world[2] = {
		vector3_quatrotate(hinge_vec_rot90[0],transforms[0]->orientation).unit_vect(),
		vector3_quatrotate(hinge_vec_rot90[1],transforms[1]->orientation).unit_vect(),
	};

	//角度を得る
	float radian = vector3_radian(hinge_vec_rot90_world[0], hinge_vec_rot90_world[1]);
	if (vector3_dot(hinge_vec_world[0], vector3_cross(hinge_vec_rot90_world[0], hinge_vec_rot90_world[1])) < 0) {
		radian = DirectX::XM_PI + DirectX::XM_PI - radian; //0~180~0 を 0~360に治す
	};

	const Vector2 limit_rad = Vector2(ToRadian(limit.x), ToRadian(limit.y)); //limitをradianに治した

	Debug::set("hinge_angle", ToAngle(radian));

	// もしlimitの影響を受ける位置に入なければfalseをreturn
	if (limit_rad.x <= limit_rad.y) {
		if (limit_rad.x <= radian && radian <= limit_rad.y)return false;
	}
	else if (limit_rad.y <= limit_rad.x) {
		if (!(limit_rad.y <= radian && radian <= limit_rad.x))return false;
	}

	//radianを基準にcosで一番近いlimitを求める
	const Vector2 limit_rad_off = Vector2(limit_rad.x - radian, limit_rad.y - radian);

	Vector3 tangent = vector3_cross(hinge_vec_rot90[1], hinge_vec[1]);

	//contactP1の基準に
	//contactP0を持ってくる

	if (cosf(limit_rad_off.x) < cosf(limit_rad_off.y)) {

		float off = fabsf(limit_rad_off.y);
		if (off > +DirectX::XM_PI) off = DirectX::XM_2PI - off;
		if (off < -DirectX::XM_PI) off = DirectX::XM_2PI + off;
		penetrate = DirectX::XM_2PI * power * off * DirectX::XM_1DIV2PI; //余分な弧の長さ

		Vector3 contactP0_world = vector3_quatrotate((hinge_vec_rot90[1] - tangent * penetrate), transforms[1]->orientation) + transforms[1]->position;

		//limit_xのほうが近い
		contactP0 = vector3_quatrotate(contactP0_world - transforms[0]->position, transforms[0]->orientation.inverse());
		contactP1 = hinge_vec_rot90[1];

	}
	else {

		float off = fabsf(limit_rad_off.x);
		if (off > +DirectX::XM_PI) off = DirectX::XM_2PI - off;
		if (off < -DirectX::XM_PI) off = DirectX::XM_2PI + off;
		penetrate = DirectX::XM_2PI * power * off * DirectX::XM_1DIV2PI; //余分な弧の長さ

		Vector3 contactP0_world = vector3_quatrotate((hinge_vec_rot90[1] + tangent * penetrate), transforms[1]->orientation) + transforms[1]->position;

		//limit_yのほうが近い
		contactP0 = vector3_quatrotate(contactP0_world - transforms[0]->position, transforms[0]->orientation.inverse());
		contactP1 = hinge_vec_rot90[1];

	}

	Debug::set("contactP1", contactP0);
	Debug::set("contactP0", contactP1);
	Debug::set("hinge_penetrate", penetrate);

	return true;
}