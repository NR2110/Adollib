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
	if (fabsf(fabsf(N) - 1) < FLT_EPSILON)return false;//•½–Ê‚ª•½s‚ÌŽžfalse‚ð•Ô‚·

	float denom= 1 / (1 - (N * N));

	ray_P = nA * ((dA - dB * N) * denom) + nB * ((-dA * N + dB) * denom);
	ray_d = vector3_cross(nA, nB);

	return true;
}

const bool Crossing_func::getCrossingP_plane_line(
	const Vector3& l_plane_n, const float& plane_dis,
	const Vector3& line_p,  const Vector3& l_line_dir,
	Vector3& crossing_p
) {
	Vector3 plane_n = l_plane_n.unit_vect();
	Vector3 line_dir = l_line_dir.unit_vect();
	if (fabsf(vector3_dot(plane_n, line_dir)) < FLT_EPSILON)return false; //•½–Ê‚ÆŒõü‚ªŒð‚í‚ç‚È‚¢Žžfalse‚ð•Ô‚·

	float t = (plane_dis - vector3_dot(line_p, plane_n)) / vector3_dot(plane_n, line_dir);

	crossing_p = line_p + t * line_dir;
	return true;

}

const bool Crossing_func::getCrossingLine_three_plane(
	const Vector3& nA, const float& dA,
	const Vector3& nB, const float& dB,
	const Vector3& nC, const float& dC,
	Vector3& crossing_p
) {
	Vector3 ray_P, ray_d;
	//2•½–Ê‚ÌŒðü‚ð‹‚ß‚Ä
	if (getCrossingLine_two_plane(nA, dA, nB, dB, ray_P, ray_d) == false)return false;
	//Œðü‚Æ•½–Ê‚ÌŒð·”»’è‚ðs‚¤
	if (getCrossingP_plane_line(nC, dC, ray_P, ray_d, crossing_p) == false)return false;

	return true;
}