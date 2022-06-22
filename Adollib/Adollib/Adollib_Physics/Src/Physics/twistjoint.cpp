#include "twistjoint.h"

#include "ALP_joint.h"
#include "ALP_collider.h"

using namespace Adollib;
using namespace Physics_function;

bool TwistJoint::limit_effect(Vector3& contactP0, Vector3& contactP1, float& penetrate) const {
	constexpr float power = 1; //謎 なぜかないと張り付く

	const world_trans* transforms[2]{
		&ALPjoint->ALPcollider[0]->transform,
		&ALPjoint->ALPcollider[1]->transform
	};

	float radian = 0;
	{
		// quaternoinのoffset collider0の座標系に持ってくる
		const Quaternion off1to0 = transforms[1]->orientation * transforms[0]->orientation.inverse();

		// collider0の座標系のvec1
		const Vector3 vec1_coll0coord = vector3_quatrotate(vec1, off1to0);

		// off1to0の後、軸の向きをそろえるquaternion
		Quaternion off1to0_and_align_vecdir;
		{
			const float radian = vector3_radian(vec0, vec1_coll0coord);
			const Vector3 rotate_axis = vector3_cross(vec1_coll0coord, vec0).unit_vect();

			Quaternion align_vecdir = quaternion_axis_radian(rotate_axis, radian);
			off1to0_and_align_vecdir = off1to0 * align_vecdir;

			Vector3 debug_vec0 = vector3_quatrotate(vec1, off1to0_and_align_vecdir);
		}

		// 回転量を求めるための元となるベクトルを求める vec0,vec1に垂直なものにする
		Vector3 check_radian_vec_base = vector3_cross(vec0, vec1);
		if (check_radian_vec_base.norm() < FLT_EPSILON)check_radian_vec_base = vector3_cross(vec0, Vector3(0, 1, 0));
		if (check_radian_vec_base.norm() < FLT_EPSILON)check_radian_vec_base = vector3_cross(vec0, Vector3(1, 0, 0));
		check_radian_vec_base.unit_vect();

		// coll0座標系での
		Vector3 coll1_checK_vec = vector3_quatrotate(check_radian_vec_base, off1to0_and_align_vecdir);

		// 角度を求める
		radian = vector3_radian(check_radian_vec_base, coll1_checK_vec);

		if (vector3_dot(vector3_cross(check_radian_vec_base, coll1_checK_vec), vec0) < 0) {
			radian = DirectX::XM_PI + DirectX::XM_PI - radian; //0~180~0 を -360~0~360に治す
		}
	}

	const Vector2 limit_rad = Vector2(ToRadian(limit.x), ToRadian(limit.y)); //limitをradianに治した
	if (radian < 0)radian += 2 * PI;
	if (radian > 2 * PI)radian -= 2 * PI;

	// もしlimitの影響を受ける位置に入なければfalseをreturn
	if (limit_rad.x <= limit_rad.y) {
		if (limit_rad.x <= radian && radian <= limit_rad.y)return false;
	}
	else if (limit_rad.y <= limit_rad.x) {
		if (!(limit_rad.y <= radian && radian <= limit_rad.x))return false;
	}

	// radianを基準にcosで一番近いlimitを求める
	Vector2 limit_rad_off = Vector2(limit_rad.x - radian, limit_rad.y - radian);

	// vec1に垂直な適当なベクトル
	Vector3 axis;
	{
		axis = vector3_cross(vec1, Vector3(0, 1, 0));
		if (axis.norm() < FLT_EPSILON)axis = vector3_cross(vec1, Vector3(1, 0, 0));
		axis.unit_vect();
	}
	// めり込み量だけtangent方向に(円の接線方向に)力をかける
	Vector3 tangent = vector3_cross(axis, vec1);

	// contactP1の基準に
	// contactP0を持ってくる
	if (cosf(limit_rad_off.x) < cosf(limit_rad_off.y)) {

		float off = limit_rad_off.y;
		if (off < 0)off *= -1;

		penetrate = DirectX::XM_2PI * power * off * DirectX::XM_1DIV2PI; //余分な弧の長さ

		Vector3 contactP0_world = vector3_quatrotate((axis + tangent * penetrate), transforms[1]->orientation) + transforms[1]->position;
		// limit_xのほうが近い
		contactP0 = vector3_quatrotate(contactP0_world - transforms[0]->position, transforms[0]->orientation.inverse());
		contactP1 = axis;
	}
	else {

		float off = limit_rad_off.x;
		if (off < 0)off *= -1;
		penetrate = DirectX::XM_2PI * power * off * DirectX::XM_1DIV2PI; //余分な弧の長さ

		Vector3 contactP0_world = vector3_quatrotate((axis - tangent * penetrate), transforms[1]->orientation) + transforms[1]->position;

		// limit_yのほうが近い
		contactP0 = vector3_quatrotate(contactP0_world - transforms[0]->position, transforms[0]->orientation.inverse());
		contactP1 = axis;
	}

	return true;
}