#include "crossing_func.h"

using namespace Adollib;
using namespace DirectX;

const bool Crossing_func_SIM::getCrossingLine_two_plane(
	const XMVECTOR& l_nA, const float& dA,
	const XMVECTOR& l_nB, const float& dB,
	XMVECTOR& ray_P, XMVECTOR& ray_d
) {
	const XMVECTOR nA = XMVector3Normalize(l_nA);
	const XMVECTOR nB = XMVector3Normalize(l_nB);

	const float N = XMVectorGetX(XMVector3Dot(nA, nB));
	if (fabsf(fabsf(N) - 1) < FLT_EPSILON)return false;//平面が平行の時falseを返す

	const float denom = 1.f / (1 - (N * N));

	ray_P = XMVectorScale(nA, ((dA - dB * N) * denom)) + XMVectorScale(nB, ((-dA * N + dB) * denom));
	ray_d = XMVector3Cross(nA, nB);
	ray_d = XMVector3Normalize(ray_d);

	return true;
}

const bool Crossing_func_SIM::getCrossingP_plane_line(
	const XMVECTOR& l_plane_n, const float& plane_dis,
	const XMVECTOR& line_p, const XMVECTOR& l_line_dir,
	float& crossing_t, bool Consider_sp
) {

	if (Consider_sp && (fabsf(XMVectorGetX(XMVector3Dot(l_plane_n, l_line_dir)) < FLT_EPSILON)))return false; //平面と光線が交わらない時falseを返す

	crossing_t = (plane_dis - XMVectorGetX(XMVector3Dot(line_p, l_plane_n))) / XMVectorGetX(XMVector3Dot(l_plane_n, l_line_dir));

	return true;

}

const bool Crossing_func_SIM::getCrossingP_three_plane(
	const XMVECTOR& nA, const float& dA,
	const XMVECTOR& nB, const float& dB,
	const XMVECTOR& nC, const float& dC,
	XMVECTOR& crossing_p
) {
	XMVECTOR ray_P, ray_d;
	//2平面の交線を求めて
	if (getCrossingLine_two_plane(nA, dA, nB, dB, ray_P, ray_d) == false)return false;

	const float A1 = XMVectorGetX(XMVector3Dot(ray_P, nA)) - dA;
	const float B1 = XMVectorGetX(XMVector3Dot(ray_P, nB)) - dB;

	const float A2 = XMVectorGetX(XMVector3Dot(ray_P + XMVectorScale(ray_d, 100), nA)) - dA;
	const float B2 = XMVectorGetX(XMVector3Dot(ray_P + XMVectorScale(ray_d, 100), nB)) - dB;



	//交線と平面の交差判定を行う
	float t = 0;
	if (getCrossingP_plane_line(nC, dC, ray_P, ray_d, t) == false)return false;
	crossing_p = ray_P + XMVectorScale(ray_d, t);

	float A = XMVectorGetX(XMVector3Dot(crossing_p, nA)) - dA;
	float B = XMVectorGetX(XMVector3Dot(crossing_p, nB)) - dB;
	float C = XMVectorGetX(XMVector3Dot(crossing_p, nC)) - dC;

	return true;
}

const bool Crossing_func_SIM::getCrossingP_AABB_ray(
	const XMVECTOR& AABB_pos, const XMVECTOR& AABB_size,
	const XMVECTOR& ray_p, const  XMVECTOR& ray_dir,
	float& tmin, float& tmax
) {
	tmin = -FLT_MAX;
	tmax = +FLT_MAX;

	XMVECTOR axis[3] = {
		{XMVectorSet(1,0,0,0)},
		{XMVectorSet(0,1,0,0)},
		{XMVectorSet(0,0,1,0)}
	};

	for (int i = 0; i < 3; i++) {
		const float D = XMVectorGetX(XMVector3Dot(axis[i], ray_dir));
		const float P = XMVectorGetX(XMVector3Dot(axis[i], ray_p - AABB_pos));
		{
			//各軸の二つのslabにrayが交差するtを求める
			float ood = 1.0f / D;
			float t1 = (-AABB_size.m128_f32[i] - P) * ood;
			float t2 = (+AABB_size.m128_f32[i] - P) * ood;

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