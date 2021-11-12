#include "twistjoint.h"

#include "ALP_joint.h"
#include "ALP_collider.h"

using namespace Adollib;
using namespace Physics_function;

bool TwistJoint::limit_effect(Vector3& contactP0, Vector3& contactP1, float& penetrate) const {
	constexpr float power = 2; //謎 なぜかないと張り付く

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
		Vector3 vec_dir = vector3_quatrotate(vec[i], transforms[i]->orientation); //回転後のtwistの軸

		Vector3 axis = vector3_cross(vec[i], vec_dir).unit_vect(); //回転後と後のどちらにも垂直なベクトル
		if (axis.norm() == 0) {
			// 回転前、後が平行の時 適当な垂直なものを準備する
			if (vec[i].x != 1)axis = vector3_cross(vec[i], Vector3(1, 0, 0));
			else axis = vector3_cross(vec[i], Vector3(0, 1, 0));
		}

		Vector3 rotated_axis = vector3_quatrotate(axis, transforms[i]->orientation).unit_vect(); //回転後の垂直なベクトル

		//if (vector3_dot(vec1_dir, vec1) < 0) {
		//	// 90度以上回っているため角度がおかしくなるのを防ぐ
		//	rotated_axis *= -1;
		//}

		// 回転の角度を得る
		radian_local[i] = vector3_radian(rotated_axis, axis);
		if (vector3_dot(vector3_cross(rotated_axis, axis), vec_dir) < 0) {
			radian_local[i] = DirectX::XM_PI + DirectX::XM_PI - radian_local[i]; //0~180~0 を 0~360に治す
			axis *= -1;
			rotated_axis *= -1;
		}
	}

	const Vector2 limit_rad = Vector2(ToRadian(limit.x), ToRadian(limit.y)); //limitをradianに治した
	const float radian = radian_local[1] - radian_local[0]; //radianの差分から collider[1]の回転量を計算
	// Debug::set("angle", ToAngle(radian));

	// もしlimitの影響を受ける位置に入なければfalseをreturn
	if (limit_rad.x <= limit_rad.y) {
		if (limit_rad.x <= radian && radian <= limit_rad.y)return false;
	}
	else if (limit_rad.y <= limit_rad.x) {
		if (!(limit_rad.y <= radian && radian <= limit_rad.x))return false;
	}

	// radianを基準にcosで一番近いlimitを求める
	const Vector2 limit_rad_off = Vector2(limit_rad.x - radian, limit_rad.y - radian);

	Vector3 axis;
	{
		int i = 1;
		Vector3 vec_dir = vector3_quatrotate(vec[i], transforms[i]->orientation); //回転後のtwistの軸
		axis = vector3_cross(vec[i], vec_dir).unit_vect(); //回転後と後のどちらにも垂直なベクトル
		if (axis.norm() == 0) {
			// 回転前、後が平行の時 適当な垂直なものを準備する
			if (vec[i].x != 1)axis = vector3_cross(vec[i], Vector3(1, 0, 0));
			else axis = vector3_cross(vec[i], Vector3(0, 1, 0));
		}
	}
	Vector3 tangent = vector3_cross(axis, vec1);

	// contactP1の基準に
	// contactP0を持ってくる
	if (cosf(limit_rad_off.x) < cosf(limit_rad_off.y)) {

		penetrate = DirectX::XM_2PI * power * fabsf(limit_rad_off.y) * DirectX::XM_1DIV2PI; //余分な弧の長さ

		Vector3 contactP0_world = vector3_quatrotate((axis - tangent * penetrate), transforms[1]->orientation) + transforms[1]->position;
		// limit_xのほうが近い
		contactP0 = vector3_quatrotate(contactP0_world - transforms[0]->position, transforms[0]->orientation.inverse());
		contactP1 = axis;

	}
	else {
		// Vector3 contactP0_world = vector3_quatrotate(axis_comp0coord, quaternion_axis_radian(vec1_comp0coord, -limit_rad_off.x));

		penetrate = DirectX::XM_2PI * power * fabsf(limit_rad_off.x) * DirectX::XM_1DIV2PI; //余分な弧の長さ

		Vector3 contactP0_world = vector3_quatrotate((axis + tangent * penetrate), transforms[1]->orientation) + transforms[1]->position;

		// limit_yのほうが近い
		contactP0 = vector3_quatrotate(contactP0_world - transforms[0]->position, transforms[0]->orientation.inverse());
		contactP1 = axis;

	}




	//// collider_comp[1]からcollider_comp[0]へのオフセット (collider_comp[1]のlocal情報をcollider_comp[0]の座標系にする)
	//Quaternion off_comp1to0 = transforms[1]->orientation * transforms[0]->orientation.inverse();

	//// collider_comp[0]の座標系である
	//Vector3 vec1_comp0coord = vector3_quatrotate(vec1, off_comp1to0).unit_vect();

	//// 回転前、回転後のお互いに垂直なベクトル -> z回転以外の影響を受けない
	//Vector3 axis = vector3_cross(vec1, vec1_comp0coord);
	//{
	//	Vector3 rot_axis = vector3_cross(Vector3(0, 1, 0), vec1);
	//	if (vector3_cross(rot_axis, vec1).norm() < FLT_EPSILON) {
	//		rot_axis = vector3_cross(Vector3(1, 0, 0), vec1);
	//	}
	//	axis = vector3_quatrotate(vec1, quaternion_axis_angle(rot_axis, 90));
	//}

	//if (axis.norm() < FLT_EPSILON) {
	//	//2ベクトルが平行なら適当な垂直な軸でよい
	//	axis = vector3_cross(vec1, vec1_comp0coord + Vector3(1, 0, 0));
	//}
	//axis = axis.unit_vect();

	//// axisの回転後のベクトル
	//Vector3 axis_comp0coord = vector3_quatrotate(axis, off_comp1to0);

	//// z回転の角度を得る
	//float radian = vector3_radian(axis_comp0coord, axis);
	//if (vector3_dot(vector3_cross(axis_comp0coord, axis), vec1_comp0coord) < 0) {
	//	radian = DirectX::XM_PI + DirectX::XM_PI - radian; //0~180~0 を 0~360に治す
	//	axis *= -1;
	//	axis_comp0coord *= -1;
	//};

	//const Vector2 limit_rad = Vector2(ToRadian(limit.x), ToRadian(limit.y)); //limitをradianに治した

	//// Debug::set("angle", ToAngle(radian));

	//// もしlimitの影響を受ける位置に入なければfalseをreturn
	//if (limit_rad.x <= limit_rad.y) {
	//	if (limit_rad.x <= radian && radian <= limit_rad.y)return false;
	//}
	//else if (limit_rad.y <= limit_rad.x) {
	//	if (!(limit_rad.y <= radian && radian <= limit_rad.x))return false;
	//}

	//// radianを基準にcosで一番近いlimitを求める
	//const Vector2 limit_rad_off = Vector2(limit_rad.x - radian, limit_rad.y - radian);

	//Vector3 tangent = vector3_cross(axis, vec1);

	//// contactP1の基準に
	//// contactP0を持ってくる
	//if (cosf(limit_rad_off.x) < cosf(limit_rad_off.y)) {

	//	penetrate = DirectX::XM_2PI * power * fabsf(limit_rad_off.y) * DirectX::XM_1DIV2PI; //余分な弧の長さ

	//	Vector3 contactP0_world = vector3_quatrotate((axis - tangent * penetrate), transforms[1]->orientation) + transforms[1]->position;
	//	// limit_xのほうが近い
	//	contactP0 = vector3_quatrotate(contactP0_world - transforms[0]->position, transforms[0]->orientation.inverse());
	//	contactP1 = axis;

	//}
	//else {
	//	// Vector3 contactP0_world = vector3_quatrotate(axis_comp0coord, quaternion_axis_radian(vec1_comp0coord, -limit_rad_off.x));

	//	penetrate = DirectX::XM_2PI * power * fabsf(limit_rad_off.x) * DirectX::XM_1DIV2PI; //余分な弧の長さ

	//	Vector3 contactP0_world = vector3_quatrotate((axis + tangent * penetrate), transforms[1]->orientation) + transforms[1]->position;

	//	// limit_yのほうが近い
	//	contactP0 = vector3_quatrotate(contactP0_world - transforms[0]->position, transforms[0]->orientation.inverse());
	//	contactP1 = axis;

	//}

	return true;
}