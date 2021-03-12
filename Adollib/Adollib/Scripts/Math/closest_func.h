#pragma once
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
			const Vector3& l_pA, const Vector3& l_vecA,
			const Vector3& l_pB, const Vector3& l_vecB,
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
		const Vector3& segAs, const Vector3& segAg,
		const Vector3& segBs, const Vector3& segBg,
		Vector3& closestP_A, Vector3& closestP_B
		);
		//:::::::::::::::::
		// 2線分の最近点を求める
		// segA_s - segA_g : 線分Aの始点終点
		// segB_s - segB_g : 線分Bの始点終点
		// s : 線分Aの最近点へのスカラ
		// t : 線分Bの最近点へのスカラ
		//:::::::::::::::::
		void get_closestP_two_segment(
			const Vector3& segAs, const Vector3& segAg,
			const Vector3& segBs, const Vector3& segBg,
			float& s, float& t
		);
		//:::::::::::::::::
		// 2線分の最近点を求める
		// segA_s - segA_g : 線分Aの始点終点
		// segB_s - segB_g : 線分Bの始点終点
				// closestP_A : 線分A上の線分Bへの最近点
		// closestP_B : 線分B上の線分Bへの最近点
		// s : 線分Aの最近点へのスカラ
		// t : 線分Bの最近点へのスカラ
		//:::::::::::::::::
		void get_closestP_two_segment(
			const Vector3& segAs, const Vector3& segAg,
			const Vector3& segBs, const Vector3& segBg,
			Vector3& closestP_A, Vector3& closestP_B,
			float& s, float& t
		);

		//:::::::::::::::::
		// 直線と線分の最近点を求める
		// line_p, line_d : 直線の始点と向き
		// segB_s - segB_g : 線分の始点終点
		// s : 直線の最近点へのスカラ
		// t : 線分の最近点へのスカラ
		//:::::::::::::::::
		void get_closestP_line_segment(
			const Vector3& segAs, const Vector3& segAg,
			const Vector3& segBs, const Vector3& segBg,
			float& s, float& t
		);


		//:::::::::::::::::
		// 光線と点の最近点を求める
		// point    : 点
		// l_point  : 光線上の一点
		// l_dir    : 光線の向き
		// closestP : 光線上の点への最近点
		//:::::::::::::::::
		void get_closestP_point_line(
			const Vector3& point,
			const Vector3& l_point,
			const Vector3& l_dir,
			Vector3& closestP
		);
		//:::::::::::::::::
		// 光線と点の最近点を求める
		// point    : 点
		// l_point  : 光線上の一点
		// l_dir    : 光線の向き
		// closests : 光線上の点へのスカラ
		//:::::::::::::::::
		void get_closestP_point_line(
			const Vector3& point,
			const Vector3& l_point,
			const Vector3& l_dir,
			float& closests
		);


		//:::::::::::::::::
		// 3頂点メッシュと点の最近点を求める
		// point       : 点
		// t_point0～2 : 3頂点
		// t_normal     : 3頂点メッシュの法線
		// closestP     : 3頂点メッシュ上の点への最近点
		//:::::::::::::::::
		void get_closestP_point_triangle(
			const Vector3& point,
			const Vector3& t_point0,
			const Vector3& t_point1,
			const Vector3& t_point2,
			const Vector3& t_normal,
			Vector3& closestP
		);

		//:::::::::::::::::
		// 線分とMeshの最近点を求める
		// segS ～ segG : 線分の始点終点
		// t_point0～2 : 3頂点
		// t_normal     : 3頂点メッシュの法線
		// closest_t	: 線分上の最近点への実数t segS + closest_t * (segS - segG)
		// closest_p     : 3頂点メッシュ上の線分への最近点
		//:::::::::::::::::
		void get_closestP_segment_triangle(
			const Vector3& segS, const Vector3& segG,
			const Vector3& t_point0,
			const Vector3& t_point1,
			const Vector3& t_point2,
			const Vector3& t_normal,
			float& closest_t,
			Vector3& closest_p
		);

	//	void gt_closestP_plane_OBB();









	}
}