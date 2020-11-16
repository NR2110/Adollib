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

	float denom= 1 / (1 - (N * N));

	ray_P = nA * ((dA - dB * N) * denom) + nB * ((-dA * N + dB) * denom);
	ray_d = vector3_cross(nA, nB);

	return true;
}

const bool Crossing_func::getCrossingP_plane_line(
	const Vector3& l_plane_n, const float& plane_dis,
	const Vector3& line_p,  const Vector3& l_line_dir,
	float& crossing_t
) {
	Vector3 plane_n = l_plane_n.unit_vect();
	Vector3 line_dir = l_line_dir.unit_vect();
	if (fabsf(vector3_dot(plane_n, line_dir)) < FLT_EPSILON)return false; //平面と光線が交わらない時falseを返す

	crossing_t = (plane_dis - vector3_dot(line_p, plane_n)) / vector3_dot(plane_n, line_dir);

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
	//交線と平面の交差判定を行う
	float t = 0;
	if (getCrossingP_plane_line(nC, dC, ray_P, ray_d, t) == false)return false;
	crossing_p = ray_P + t * ray_d;

	return true;
}