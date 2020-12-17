#include "closest_func.h"

using namespace Adollib;


bool Closest_func::get_closestP_two_line(
	const Vector3& l_pA, const Vector3& base_l_vecA,
	const Vector3& l_pB, const Vector3& base_l_vecB,
	float& s, float&t
) {
	Vector3 l_vecA, l_vecB;
	l_vecA = base_l_vecA.unit_vect();
	l_vecB = base_l_vecB.unit_vect();
	float D1 = vector3_dot(l_pB - l_pA, l_vecA);
	float D2 = vector3_dot(l_pB - l_pA, l_vecB);
	float Dv = vector3_dot(l_vecA, l_vecB);

	if (Dv == 1 || Dv == -1)return false; //2直線が平行

	s = (D1 - D2 * Dv) / (1 - Dv * Dv);
	t = (D2 - D1 * Dv) / (Dv * Dv - 1);

	return true;
}

#pragma region  get_closestP_two_segment

void Closest_func::get_closestP_two_segment(
	const Vector3& segAs, const Vector3& segAg,
	const Vector3& segBs, const Vector3& segBg,
	Vector3& closestP_A, Vector3& closestP_B
) {

	Vector3 v1 = segAg - segAs; //A線分のベクトル
	Vector3 v2 = segBg - segBs; //B線分のベクトル
	Vector3 r  = segAs - segBs; //各始点をつなぐベクトル

	float a = vector3_dot(v1, v1);
	float b = vector3_dot(v1, v2);
	float c = vector3_dot(v2, v2);
	float d = vector3_dot(v1, r);
	float e = vector3_dot(v2, r);
	float det = -a * c + b * b;
	float s = 0.0f, t = 0.0f; //最近点の場所 s=0 : segAs


	// 逆行列のチェック
	if (det * det > FLT_EPSILON) {
		s = (c * d - b * e) / det;
	}

	// sを0～1にクランプ
	s = ALmax(ALmin(s, 1.f), 0.f);

	t = (e + s * b) / c;

	// tを0～1にクランプ
	t = ALmax(ALmin(t, 1.f), 0.f);

	// 再度sを求める
	s = (-d + t * b) / a;
	s = ALmax(ALmin(s, 1.f), 0.f);

	closestP_A = segAs + s * v1;
	closestP_B = segBs + t * v2;

}
void Closest_func::get_closestP_two_segment(
	const Vector3& segAs, const Vector3& segAg,
	const Vector3& segBs, const Vector3& segBg,
	float& s, float& t
) {

	Vector3 v1 = segAg - segAs; //A線分のベクトル
	Vector3 v2 = segBg - segBs; //B線分のベクトル
	Vector3 r = segAs - segBs; //各始点をつなぐベクトル

	float a = vector3_dot(v1, v1);
	float b = vector3_dot(v1, v2);
	float c = vector3_dot(v2, v2);
	float d = vector3_dot(v1, r);
	float e = vector3_dot(v2, r);
	float det = -a * c + b * b;
	//float s = 0.0f, t = 0.0f; //最近点の場所 s=0 : segAs


	// 逆行列のチェック
	if (det * det > FLT_EPSILON) {
		s = (c * d - b * e) / det;
	}

	// sを0～1にクランプ
	s = s < 0.0f ? 0.0f : s > 1.0f ? 1.0f : s;

	t = (e + s * b) / c;

	// tを0～1にクランプ
	t = t < 0.0f ? 0.0f : t > 1.0f ? 1.0f : t;

	// 再度sを求める
	s = (-d + t * b) / a;
	s = s < 0.0f ? 0.0f : s > 1.0f ? 1.0f : s;

	//closestP_A = segAs + s * v1;
	//closestP_B = segBs + t * v2;

}
void Closest_func::get_closestP_two_segment(
	const Vector3& segAs, const Vector3& segAg,
	const Vector3& segBs, const Vector3& segBg,
	Vector3& closestP_A, Vector3& closestP_B,
	float& s, float& t
) {

	Vector3 v1 = segAg - segAs; //A線分のベクトル
	Vector3 v2 = segBg - segBs; //B線分のベクトル
	Vector3 r = segAs - segBs; //各始点をつなぐベクトル

	float a = vector3_dot(v1, v1);
	float b = vector3_dot(v1, v2);
	float c = vector3_dot(v2, v2);
	float d = vector3_dot(v1, r);
	float e = vector3_dot(v2, r);
	float det = -a * c + b * b;
	//float s = 0.0f, t = 0.0f; //最近点の場所 s=0 : segAs


	// 逆行列のチェック
	if (det * det > FLT_EPSILON) {
		s = (c * d - b * e) / det;
	}

	// sを0～1にクランプ
	s = s < 0.0f ? 0.0f : s > 1.0f ? 1.0f : s;

	t = (e + s * b) / c;

	// tを0～1にクランプ
	t = t < 0.0f ? 0.0f : t > 1.0f ? 1.0f : t;

	// 再度sを求める
	s = (-d + t * b) / a;
	s = s < 0.0f ? 0.0f : s > 1.0f ? 1.0f : s;

	closestP_A = segAs + s * v1;
	closestP_B = segBs + t * v2;

}
#pragma endregion

//:::::::::::::::::
// 光線と点の最近点を求める
// point    : 点
// l_point  : 光線上の一点
// l_dir    : 光線の向き
// closestP : 光線上の点への最近点
//:::::::::::::::::
void Closest_func::get_closestP_point_line(
	const Vector3& point,
	const Vector3& l_point,
	const Vector3& l_dir,
	Vector3& closestP
) {
	float s = vector3_dot(point - l_point, l_dir) / vector3_dot(l_dir, l_dir);
	closestP = l_point + s * l_dir;
}
//:::::::::::::::::
// 光線と点の最近点を求める
// point    : 点
// l_point  : 光線上の一点
// l_dir    : 光線の向き
// closestP : 光線上の点への最近点
//:::::::::::::::::
void Closest_func::get_closestP_point_line(
	const Vector3& point,
	const Vector3& l_point,
	const Vector3& l_dir,
	float& closests
) {
	closests = vector3_dot(point - l_point, l_dir) / vector3_dot(l_dir, l_dir);
}

//:::::::::::::::::
// 3頂点メッシュと点の最近点を求める
// point       : 点
// t_point0～2 : 3頂点
// t_normal     : 3頂点メッシュの法線
// closestP     : 3頂点メッシュ上の点への最近点
//:::::::::::::::::
void Closest_func::get_closestP_point_triangle(
	const Vector3& point,
	const Vector3& t_point0,
	const Vector3& t_point1,
	const Vector3& t_point2,
	const Vector3& t_normal,
	Vector3& closestP
) {

	// ３角形面上の投影点
	Vector3 proj = point - vector3_dot(t_normal, point - t_point0) * t_normal;

	// エッジP0,P1のボロノイ領域
	Vector3 edgeP01 = t_point1 - t_point0;
	Vector3 edgeP01_normal = vector3_cross(edgeP01, t_normal);

	float voronoiEdgeP01_check1 = vector3_dot(proj - t_point0, edgeP01_normal);
	float voronoiEdgeP01_check2 = vector3_dot(proj - t_point0, edgeP01);
	float voronoiEdgeP01_check3 = vector3_dot(proj - t_point1, -edgeP01);

	if (voronoiEdgeP01_check1 > 0.0f && voronoiEdgeP01_check2 > 0.0f && voronoiEdgeP01_check3 > 0.0f) {
		Closest_func::get_closestP_point_line(proj, t_point0, edgeP01, closestP);
		return;
	}

	// エッジP1,P2のボロノイ領域
	Vector3 edgeP12 = t_point2 - t_point1;
	Vector3 edgeP12_normal = vector3_cross(edgeP12, t_normal);

	float voronoiEdgeP12_check1 = vector3_dot(proj - t_point1, edgeP12_normal);
	float voronoiEdgeP12_check2 = vector3_dot(proj - t_point1, edgeP12);
	float voronoiEdgeP12_check3 = vector3_dot(proj - t_point2, -edgeP12);

	if (voronoiEdgeP12_check1 > 0.0f && voronoiEdgeP12_check2 > 0.0f && voronoiEdgeP12_check3 > 0.0f) {
		Closest_func::get_closestP_point_line(proj, t_point1, edgeP12, closestP);
		return;
	}

	// エッジP2,P0のボロノイ領域
	Vector3 edgeP20 = t_point0 - t_point2;
	Vector3 edgeP20_normal = vector3_cross(edgeP20, t_normal);

	float voronoiEdgeP20_check1 = vector3_dot(proj - t_point2, edgeP20_normal);
	float voronoiEdgeP20_check2 = vector3_dot(proj - t_point2, edgeP20);
	float voronoiEdgeP20_check3 = vector3_dot(proj - t_point0, -edgeP20);

	if (voronoiEdgeP20_check1 > 0.0f && voronoiEdgeP20_check2 > 0.0f && voronoiEdgeP20_check3 > 0.0f) {
		Closest_func::get_closestP_point_line(proj, t_point2, edgeP20, closestP);
		return;
	}

	// ３角形面の内側
	if (voronoiEdgeP01_check1 <= 0.0f && voronoiEdgeP12_check1 <= 0.0f && voronoiEdgeP20_check1 <= 0.0f) {
		closestP = proj;
		return;
	}

	// 頂点P0のボロノイ領域
	if (voronoiEdgeP01_check2 <= 0.0f && voronoiEdgeP20_check3 <= 0.0f) {
		closestP = t_point0;
		return;
	}

	// 頂点P1のボロノイ領域
	if (voronoiEdgeP01_check3 <= 0.0f && voronoiEdgeP12_check2 <= 0.0f) {
		closestP = t_point1;
		return;
	}

	// 頂点P2のボロノイ領域
	if (voronoiEdgeP20_check2 <= 0.0f && voronoiEdgeP12_check3 <= 0.0f) {
		closestP = t_point2;
		return;
	}

}