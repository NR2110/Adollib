#include "twistjoint.h"

#include "ALP_joint.h"
#include "ALP_collider.h"

#include "../Object/gameobject_manager.h"

using namespace Adollib;
using namespace Physics_function;

bool TwistJoint::limit_effect(Vector3& contactP0, Vector3& contactP1, float& penetrate) const {
	constexpr float power = 2; //謎 なぜかないと張り付く

	const world_trans* transforms[2]{
		&ALPjoint->ALPcollider[0]->transform,
		&ALPjoint->ALPcollider[1]->transform
	};

	float radian = 0;
	{
		// quaternoinのoffset
		Quaternion off = transforms[1]->orientation * transforms[0]->orientation.inverse();

		// 回転後のtwistの軸
		Vector3 vec_dir = vector3_quatrotate(vec0, off);

		Vector3 axis = vector3_cross(vec0, vec_dir); //回転後と後のどちらにも垂直なベクトル
		if (axis.norm() < FLT_EPSILON) {
			// 回転前、後が平行の時 適当な垂直なものを準備する
			if (fabsf(vector3_dot(vec0, Vector3(1, 0, 0))) < FLT_EPSILON)axis = vector3_cross(vec0, Vector3(1, 0, 0));
			else axis = vector3_cross(vec0, Vector3(0, 1, 0));
		}
		axis = axis.unit_vect();

		Vector3 rotated_axis = vector3_quatrotate(axis, off).unit_vect(); //回転後の垂直なベクトル

		// 回転の角度を得る
		radian = vector3_radian(rotated_axis, axis);
		if (vector3_dot(vector3_cross(rotated_axis, axis), vec_dir) < 0) {
			radian = DirectX::XM_PI + DirectX::XM_PI - radian; //0~180~0 を -360~0~360に治す
			axis *= -1;
			rotated_axis *= -1;
		}


		if (0) {
			Debug::set("1_vec[i]", vec0);
			Debug::set("1_vec_dir", vec_dir);
			Debug::set("1_axis", axis);
			Debug::set("1_rotated_axis", rotated_axis);
			Debug::set("1_radian_local[i]", ToAngle(radian));

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
				debug[0]->transform->local_orient = quaternion_from_to_rotate(Vector3(0, 0, 1), vec0);
				debug[1]->transform->local_orient = quaternion_from_to_rotate(Vector3(0, 0, 1), vec_dir);
				debug[2]->transform->local_orient = quaternion_from_to_rotate(Vector3(0, 0, 1), axis);
				debug[3]->transform->local_orient = quaternion_from_to_rotate(Vector3(0, 0, 1), rotated_axis);

				debug[0]->transform->local_pos = transforms[1]->position + Vector3(0, 3, 0) + vec0 * 2;
				debug[1]->transform->local_pos = transforms[1]->position + Vector3(0, 3, 0) + vec_dir * 2;
				debug[2]->transform->local_pos = transforms[1]->position + Vector3(0, 3, 0) + axis * 2;
				debug[3]->transform->local_pos = transforms[1]->position + Vector3(0, 3, 0) + rotated_axis * 2;
			}

		}

	}

	const Vector2 limit_rad = Vector2(ToRadian(limit.x), ToRadian(limit.y)); //limitをradianに治した
	//float radian = radian_local[1] - radian_local[0]; //radianの差分から collider[1]の回転量を計算
	if (radian < 0)radian += 2 * PI;
	if (radian > 2 * PI)radian -= 2 * PI;

	Debug::set("radian", ToAngle(radian));

	// もしlimitの影響を受ける位置に入なければfalseをreturn
	if (limit_rad.x <= limit_rad.y) {
		if (limit_rad.x <= radian && radian <= limit_rad.y)return false;
	}
	else if (limit_rad.y <= limit_rad.x) {
		if (!(limit_rad.y <= radian && radian <= limit_rad.x))return false;
	}

	// radianを基準にcosで一番近いlimitを求める
	Vector2 limit_rad_off = Vector2(limit_rad.x - radian, limit_rad.y - radian);
	if (fabsf(limit_rad_off.x) > ToRadian(max_radian_pow))limit_rad_off.x *= ToRadian(max_radian_pow) * limit_rad_off.x / fabsf(limit_rad_off.x);
	if (fabsf(limit_rad_off.y) > ToRadian(max_radian_pow))limit_rad_off.y *= ToRadian(max_radian_pow) * limit_rad_off.y / fabsf(limit_rad_off.y);

	Vector3 axis;
	{
		int i = 1;
		Vector3 vec_dir = vector3_quatrotate(vec0, transforms[i]->orientation); //回転後のtwistの軸
		axis = vector3_cross(vec0, vec_dir).unit_vect(); //回転後と後のどちらにも垂直なベクトル
		if (axis.norm() == 0) {
			// 回転前、後が平行の時 適当な垂直なものを準備する
			if (vec0.x != 1)axis = vector3_cross(vec0, Vector3(1, 0, 0));
			else axis = vector3_cross(vec0, Vector3(0, 1, 0));
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

	return true;
}