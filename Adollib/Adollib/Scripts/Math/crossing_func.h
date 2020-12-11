#pragma once
#include "vector.h"
#include "quaternion.h"
#include "matrix.h"

namespace Adollib {

	namespace Crossing_func {

		//:::::::::::::::::
		// 2���ʂ̌�������߂�
		// nA : ����A�̖@��
		// dA : ����A�̋���
		// nB : ����B�̖@��
		// dB : ����B�̋���
		// ray_P : �����̓_P
		// ray_d : ����̌���
		//���ʂ����s�̎�false��Ԃ�
		//:::::::::::::::::
		const bool getCrossingLine_two_plane(
			const Vector3& nA, const float& dA,
			const Vector3& nB, const float& dB,
			Vector3& ray_P, Vector3& ray_d
		);

		//:::::::::::::::::
		// �����ƕ��ʂ̌�_�����߂�
		// plane_n	 : ����A�̖@��
		// plane_dis : ����A�̋���
		// ray_p	: ������̓_
		// ray_dir : �����̌���
		// crossing_t : ��������ray_t
		//���ʂƌ����������Ȃ���false��Ԃ�
		//:::::::::::::::::
		const bool getCrossingP_plane_line(
			const Vector3& plane_n, const float& plane_dis,
			const Vector3& line_p,  const Vector3& line_dir,
			float& crossing_t
		);

		//:::::::::::::::::
		// 3���ʂ̌�_�����߂�
		// nA : ����A�̖@��
		// dA : ����A�̋���
		// nB : ����B�̖@��
		// dB : ����B�̋���
		// nC : ����C�̖@��
		// dC : ����C�̋���
		// crossing_p : ��_
		//���ʂ����s�̎�false��Ԃ�
		//:::::::::::::::::
		const bool getCrossingP_three_plane(
			const Vector3& nA, const float& dA,
			const Vector3& nB, const float& dB,
			const Vector3& nC, const float& dC,
			Vector3& crossing_p
		);

		//:::::::::::::::::
		// 3���ʂ̌�_�����߂�
		// ray_point : ������̓_
		// ray_dir   : �����̌���
		// t_point0�`2 : 3���_
		// t_normal    : 3���_���b�V���̖@��
		// crossingP_min  :  ��_min
		// crossingP_max  :  ��_max
		//���ʂ����s�̎�false��Ԃ�
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
		// AABB��Ray�̌����_�����߂�
		// AABB_min ,AABB_max : AABB�̑Ίp2�_
		// ray_p : ray�̎n�_
		// ray_dir : ray�̌���
		// float :��������ŋߓ_ray_p + ray_dir * min_t
		// float :��������ŉ��_ray_p + ray_dir * max_t
		//�������Ȃ��Ƃ�false��Ԃ�
		//:::::::::::::::::::
		const bool getCrossingP_AABB_ray(
			const Vector3& AABB_pos, const  Vector3& AABB_size,
			const Vector3& ray_p, const  Vector3& ray_dir,
			float& tmin,float& tmax
		);

		//:::::::::::::::::::
		// OBB��Ray�̌����_�����߂�
		// OBB_s , OBB_g : OBB�̑Ίp2�_
		// OBB_orient : OBB�̌���
		// ray_p : ray�̎n�_
		// ray_dir : ray�̌���
		// float :��������ŋߓ_ray_p + ray_dir * min_t
		// float :��������ŉ��_ray_p + ray_dir * max_t
		//�������Ȃ��Ƃ�false��Ԃ�
		//:::::::::::::::::::
		const bool getCrossingP_OBB_ray(
			const Vector3& OBB_s, const  Vector3& OBB_g,
			const Quaternion& OBB_orient,
			const Vector3& ray_p, const  Vector3& ray_dir,
			float& tmin, float& tmax
		);




	}
}