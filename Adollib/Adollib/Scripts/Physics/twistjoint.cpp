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
	const Vector3 vec[2]{
		vec0,
		vec1
	};

	float radian_local[2] = { 0 };

	static Gameobject* go[4] = { nullptr };
	if (go[0] == nullptr) {
		go[0] = Gameobject_manager::createCube();
		go[1] = Gameobject_manager::createCube();
		go[2] = Gameobject_manager::createCube();
		go[3] = Gameobject_manager::createCube();

		go[0]->transform->local_scale = Vector3(0.2f, 0.2f, 1);
		go[1]->transform->local_scale = Vector3(0.2f, 0.2f, 1);
		go[2]->transform->local_scale = Vector3(0.2f, 0.2f, 1);
		go[3]->transform->local_scale = Vector3(0.2f, 0.2f, 1);

		go[0]->material->color = Vector4(1, 0, 0, 1);
		go[1]->material->color = Vector4(0, 1, 0, 1);
		go[2]->material->color = Vector4(1, 1, 0, 1);
		go[3]->material->color = Vector4(0, 0, 1, 1);
	}

	for (int i = 0; i < 2; ++i)
	{

		Vector3 vec_dir = vector3_quatrotate(vec[i], transforms[i]->orientation); //回転後のtwistの軸

		Vector3 axis = vector3_cross(vec[i], vec_dir); //回転後と後のどちらにも垂直なベクトル
		if (axis.norm() < FLT_EPSILON) {
			// 回転前、後が平行の時 適当な垂直なものを準備する
			if (fabsf(vector3_dot(vec[i], Vector3(1, 0, 0))) < FLT_EPSILON)axis = vector3_cross(vec[i], Vector3(1, 0, 0));
			else axis = vector3_cross(vec[i], Vector3(0, 1, 0));
		}
		axis = axis.unit_vect();


		Vector3 rotated_axis = vector3_quatrotate(axis, transforms[i]->orientation).unit_vect(); //回転後の垂直なベクトル

		if (vector3_dot(vec_dir, vec[i]) < 0) {

			//float d = vector3_dot(vec[i], vec_dir);
			//vec_dir -= vec[i] * d * 2;

			// 90度以上回っているため角度がおかしくなるのを防ぐ
			//rotated_axis *= vector3_quatrotate(rotated_axis, quaternion_axis_angle(vector3_cross(axis, vec[i]), 180));
			//vector3_quatrotate(axis, transforms[i]->orientation.inverse()).unit_vect(); //回転後の垂直なベクトル
			int s = 0;
		}



		//if (i == 0) {
		//	Debug::set("vec_dir[0]", vec_dir);
		//	Debug::set("axis[0]", axis);
		//	Debug::set("rotated_axis[0]", rotated_axis);
		//}

		if (i == 1) {
			Debug::set("vec[1]", vec[i]);
			Debug::set("vec_dir[1]", vec_dir);
			Debug::set("axis[1]", axis);
			Debug::set("rotated_axis[1]", rotated_axis);

			go[0]->transform->local_orient = quaternion_from_to_rotate(Vector3(0, 0, 1), vec[i]);
			go[1]->transform->local_orient = quaternion_from_to_rotate(Vector3(0, 0, 1), vec_dir);
			go[2]->transform->local_orient = quaternion_from_to_rotate(Vector3(0, 0, 1), axis);
			go[3]->transform->local_orient = quaternion_from_to_rotate(Vector3(0, 0, 1), rotated_axis);

			go[0]->transform->local_pos = transforms[1]->position + Vector3(0, 7, 0) + vec[i] * 2;
			go[1]->transform->local_pos = transforms[1]->position + Vector3(0, 7, 0) + vec_dir * 2;
			go[2]->transform->local_pos = transforms[1]->position + Vector3(0, 7, 0) + axis * 2;
			go[3]->transform->local_pos = transforms[1]->position + Vector3(0, 7, 0) + rotated_axis * 2;
		}

		// 回転の角度を得る
		radian_local[i] = vector3_radian(rotated_axis, axis);
		if (vector3_dot(vector3_cross(rotated_axis, axis), vec_dir) < 0) {
			radian_local[i] = DirectX::XM_PI + DirectX::XM_PI - radian_local[i]; //0~180~0 を -360~0~360に治す
			axis *= -1;
			rotated_axis *= -1;
		}
	}

	const Vector2 limit_rad = Vector2(ToRadian(limit.x), ToRadian(limit.y)); //limitをradianに治した
	float radian = radian_local[1] - radian_local[0]; //radianの差分から collider[1]の回転量を計算
	if (radian < 0)radian += 2 * PI;
	if (radian > 2 * PI)radian -= 2 * PI;
	Debug::set("radian_local[0]", ToAngle(radian_local[0]));
	Debug::set("radian_local[1]", ToAngle(radian_local[1]));
	Debug::set("angle", ToAngle(radian));

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