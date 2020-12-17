#include "crossing_func.h"

using namespace Adollib;
using namespace Crossing_func;

const bool Crossing_func::getCrossingLine_two_plane(
	const Vector3& l_nA, const float& dA,
	const Vector3& l_nB, const float& dB,
	Vector3& ray_P, Vector3& ray_d
) {
	Vector3 nA = l_nA.unit_vect();
	Vector3 nB = l_nB.unit_vect();

	float N = vector3_dot(nA, nB);
	if (fabsf(fabsf(N) - 1) < FLT_EPSILON)return false;//平面が平行の時falseを返す

	float denom = 1.f / (1 - (N * N));

	ray_P = nA * ((dA - dB * N) * denom) + nB * ((-dA * N + dB) * denom);
	ray_d = vector3_cross(nA, nB);
	ray_d = ray_d.unit_vect();

	return true;
}

const bool Crossing_func::getCrossingP_plane_line(
	const Vector3& l_plane_n, const float& plane_dis,
	const Vector3& line_p, const Vector3& l_line_dir,
	float& crossing_t, bool Consider_sp
) {

	if (Consider_sp && (fabsf(vector3_dot(l_plane_n, l_line_dir) < FLT_EPSILON)))return false; //平面と光線が交わらない時falseを返す

	crossing_t = (plane_dis - vector3_dot(line_p, l_plane_n)) / vector3_dot(l_plane_n, l_line_dir);

	return true;

}

const bool Crossing_func::getCrossingP_three_plane(
	const Vector3& nA, const float& dA,
	const Vector3& nB, const float& dB,
	const Vector3& nC, const float& dC,
	Vector3& crossing_p
) {
	Vector3 ray_P, ray_d;
	//2平面の交線を求めて
	if (getCrossingLine_two_plane(nA, dA, nB, dB, ray_P, ray_d) == false)return false;

	float A1 = vector3_dot(ray_P, nA) - dA;
	float B1 = vector3_dot(ray_P, nB) - dB;

	float A2 = vector3_dot(ray_P + ray_d * 100, nA) - dA;
	float B2 = vector3_dot(ray_P + ray_d * 100, nB) - dB;



	//交線と平面の交差判定を行う
	float t = 0;
	if (getCrossingP_plane_line(nC, dC, ray_P, ray_d, t) == false)return false;
	crossing_p = ray_P + t * ray_d;

	float A = vector3_dot(crossing_p, nA) - dA;
	float B = vector3_dot(crossing_p, nB) - dB;
	float C = vector3_dot(crossing_p, nC) - dC;

	return true;
}

const bool Crossing_func::getCrossingP_AABB_ray(
	const Vector3& AABB_pos, const Vector3& AABB_size,
	const Vector3& ray_p, const  Vector3& ray_dir,
	float& tmin, float& tmax
) {
	tmin = -FLT_MAX;
	tmax = +FLT_MAX;

	Vector3 axis[3] = {
		{Vector3(1,0,0)},
		{Vector3(0,1,0)},
		{Vector3(0,0,1)}
	};

	for (int i = 0; i < 3; i++) {
		float D = vector3_dot(axis[i], ray_dir);
		float P = vector3_dot(axis[i], ray_p - AABB_pos);
		{
			//各軸の二つのslabにrayが交差するtを求める
			float ood = 1.0f / D;
			float t1 = (-AABB_size[i] - P) * ood;
			float t2 = (+AABB_size[i] - P) * ood;

			//もしt2のほうに大きい方を保存
			if (t1 > t2) std::swap(t1, t2);

			//tmin,tmaxを更新
			tmin = std::max<float>(tmin, t1);
			tmax = std::min<float>(tmax, t2);

			//もしtmaxがtminより小さくなった時、rayはDOPと交差していない
			if (tmin > tmax) return false;
		}
	}

	return true;

}