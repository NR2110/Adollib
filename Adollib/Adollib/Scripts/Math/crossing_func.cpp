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

	float denom = 1.f / (1 - (N * N));

	ray_P = nA * ((dA - dB * N) * denom) + nB * ((-dA * N + dB) * denom);
	ray_d = vector3_cross(nA, nB);
	ray_d = ray_d.unit_vect();

	return true;
}

const bool Crossing_func::getCrossingP_plane_line(
	const Vector3& l_plane_n, const float& plane_dis,
	const Vector3& line_p,  const Vector3& l_line_dir,
	float& crossing_t
) {
	Vector3 plane_n = l_plane_n.unit_vect();
	Vector3 line_dir = l_line_dir.unit_vect();
	if (fabsf(vector3_dot(plane_n, line_dir)) < FLT_EPSILON)return false; //•½–Ê‚ÆŒõü‚ªŒð‚í‚ç‚È‚¢Žžfalse‚ð•Ô‚·

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
	//2•½–Ê‚ÌŒðü‚ð‹‚ß‚Ä
	if (getCrossingLine_two_plane(nA, dA, nB, dB, ray_P, ray_d) == false)return false;

	float A1 = vector3_dot(ray_P, nA) - dA;
	float B1 = vector3_dot(ray_P, nB) - dB;

	float A2 = vector3_dot(ray_P + ray_d * 100, nA) - dA;
	float B2 = vector3_dot(ray_P + ray_d * 100, nB) - dB;



	//Œðü‚Æ•½–Ê‚ÌŒð·”»’è‚ðs‚¤
	float t = 0;
	if (getCrossingP_plane_line(nC, dC, ray_P, ray_d, t) == false)return false;
	crossing_p = ray_P + t * ray_d;

	float A = vector3_dot(crossing_p, nA) - dA;
	float B = vector3_dot(crossing_p, nB) - dB;
	float C = vector3_dot(crossing_p, nC) - dC;

	return true;
}