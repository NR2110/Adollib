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
		// line_p	: ������̓_
		// line_dir : �����̌���
		// crossing_p : ��_
		//���ʂƌ����������Ȃ���false��Ԃ�
		//:::::::::::::::::
		const bool getCrossingP_plane_line(
			const Vector3& plane_n, const float& plane_dis,
			const Vector3& line_p,  const Vector3& line_dir,
			Vector3& crossing_p
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


	}
}