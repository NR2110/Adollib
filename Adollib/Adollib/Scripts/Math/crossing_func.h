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
		// line_p	: 光線上の点
		// line_dir : 光線の向き
		// crossing_p : 交点
		//平面と光線が交わらない時falseを返す
		//:::::::::::::::::
		const bool getCrossingP_plane_line(
			const Vector3& plane_n, const float& plane_dis,
			const Vector3& line_p,  const Vector3& line_dir,
			Vector3& crossing_p
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


	}
}