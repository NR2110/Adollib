#pragma once
#include "vector.h"
#include "quaternion.h"
#include "matrix.h"

namespace Adollib {

	namespace Crossing_func {

		//:::::::::::::::::
		// 2平面の交線を求める
		// nA : 平面Aの法線
		// dA : 平面Aの距離
		// nB : 平面Bの法線
		// dB : 平面Bの距離
		// ray_P : 交線上の点P
		// ray_d : 交線の向き
		//平面が平行の時falseを返す
		//:::::::::::::::::
		const bool getCrossingLine_two_plane(
			const Vector3& nA, const float& dA,
			const Vector3& nB, const float& dB,
			Vector3& ray_P, Vector3& ray_d
		);

		//:::::::::::::::::
		// 直線と平面の交点を求める
		// plane_n	 : 平面Aの法線
		// plane_dis : 平面Aの距離
		// ray_p	: 光線上の点
		// ray_dir : 光線の向き
		// crossing_t : 交差するray_t
		//平面と光線が交わらない時falseを返す
		//:::::::::::::::::
		const bool getCrossingP_plane_line(
			const Vector3& plane_n, const float& plane_dis,
			const Vector3& line_p,  const Vector3& line_dir,
			float& crossing_t
		);

		//:::::::::::::::::
		// 3平面の交点を求める
		// nA : 平面Aの法線
		// dA : 平面Aの距離
		// nB : 平面Bの法線
		// dB : 平面Bの距離
		// nC : 平面Cの法線
		// dC : 平面Cの距離
		// crossing_p : 交点
		//平面が平行の時falseを返す
		//:::::::::::::::::
		const bool getCrossingP_three_plane(
			const Vector3& nA, const float& dA,
			const Vector3& nB, const float& dB,
			const Vector3& nC, const float& dC,
			Vector3& crossing_p
		);

		//:::::::::::::::::
		// 3平面の交点を求める
		// ray_point : 直線状の点
		// ray_dir   : 直線の向き
		// t_point0～2 : 3頂点
		// t_normal    : 3頂点メッシュの法線
		// crossingP_min  :  交点min
		// crossingP_max  :  交点max
		//平面が平行の時falseを返す
		//:::::::::::::::::
		const bool getCrossing_line_triangle(
			const Vector3& ray_point, const Vector3& ray_dir,
			const Vector3& t_point0,
			const Vector3& t_point1,
			const Vector3& t_point2,
			const Vector3& t_normal,
			Vector3& closestP
		);

		//:::::::::::::::::::
		// AABBとRayの交差点を求める
		// AABB_min ,AABB_max : AABBの対角2点
		// ray_p : rayの始点
		// ray_dir : rayの向き
		// float :交差する最近点ray_p + ray_dir * min_t
		// float :交差する最遠点ray_p + ray_dir * max_t
		//交差しないときfalseを返す
		//:::::::::::::::::::
		const bool getCrossingP_AABB_ray(
			const Vector3& AABB_pos, const  Vector3& AABB_size,
			const Vector3& ray_p, const  Vector3& ray_dir,
			float& tmin,float& tmax
		);

		//:::::::::::::::::::
		// OBBとRayの交差点を求める
		// OBB_s , OBB_g : OBBの対角2点
		// OBB_orient : OBBの向き
		// ray_p : rayの始点
		// ray_dir : rayの向き
		// float :交差する最近点ray_p + ray_dir * min_t
		// float :交差する最遠点ray_p + ray_dir * max_t
		//交差しないときfalseを返す
		//:::::::::::::::::::
		const bool getCrossingP_OBB_ray(
			const Vector3& OBB_s, const  Vector3& OBB_g,
			const Quaternion& OBB_orient,
			const Vector3& ray_p, const  Vector3& ray_dir,
			float& tmin, float& tmax
		);




	}
}