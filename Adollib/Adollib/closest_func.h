#pragma 
#include "vector.h"
#include "quaternion.h"
#include "matrix.h"

namespace Adollib {

	namespace Closest_func {

		//:::::::::::::::::
		// 2直線の最近点を求める
		// l_pA : 直線A上の任意の一点
		// l_vecA : 直線Aの方向ベクトル
		// l_pB : 直線B上の任意の一点
		// l_vecB : 直線Bの方向ベクトル
		// s : l_pA + l_vecA * s が直線A上の最近点となる値
		// t : l_pB + l_vecB * t が直線B上の最近点となる値
		//:::::::::::::::::
		bool get_closestP_two_line(
			const vector3& l_pA, const vector3& l_vecA,
			const vector3& l_pB, const vector3& l_vecB,
			float& s, float&t
		);

		//:::::::::::::::::
		// 2線分の最近点を求める
		// segA_s - segA_g : 線分Aの始点終点
		// segB_s - segB_g : 線分Bの始点終点
		// closestP_A : 線分A上の線分Bへの最近点
		// closestP_B : 線分B上の線分Bへの最近点
		//:::::::::::::::::
		void get_closestP_two_segment(
		const vector3& segAs, const vector3& segAg,
		const vector3& segBs, const vector3& segBg,
		vector3& closestP_A, vector3& closestP_B
		);

		//:::::::::::::::::
		// 光線と点の最近点を求める
		// point    : 点
		// l_point  : 光線上の一点
		// l_dir    : 光線の向き
		// closestP : 光線上の点への最近点
		//:::::::::::::::::
		void get_closestP_point_line(
			const vector3& point, 
			const vector3& l_point,
			const vector3& l_dir, 
			const vector3& closestP
		);


		//:::::::::::::::::
		// 3頂点メッシュと点の最近点を求める
		// point       : 点
		// t_point0～2 : 3頂点
		// t_normal     : 3頂点メッシュの法線
		// closestP     : 3頂点メッシュ上の点への最近点
		//:::::::::::::::::
		void get_closestP_point_triangle(
			const vector3& point,
			const vector3& t_point0,
			const vector3& t_point1,
			const vector3& t_point2,
			const vector3& t_normal,
			vector3& closestP
		);









	}
}