#pragma 
#include "vector.h"
#include "quaternion.h"
#include "matrix.h"

namespace Adollib {

	namespace Closest_func {

		//:::::::::::::::::
		// 2�����̍ŋߓ_�����߂�
		// l_pA : ����A��̔C�ӂ̈�_
		// l_vecA : ����A�̕����x�N�g��
		// l_pB : ����B��̔C�ӂ̈�_
		// l_vecB : ����B�̕����x�N�g��
		// s : l_pA + l_vecA * s ������A��̍ŋߓ_�ƂȂ�l
		// t : l_pB + l_vecB * t ������B��̍ŋߓ_�ƂȂ�l
		//:::::::::::::::::
		bool get_closestP_two_line(
			const vector3& l_pA, const vector3& l_vecA,
			const vector3& l_pB, const vector3& l_vecB,
			float& s, float&t
		);

		//:::::::::::::::::
		// 2�����̍ŋߓ_�����߂�
		// segA_s - segA_g : ����A�̎n�_�I�_
		// segB_s - segB_g : ����B�̎n�_�I�_
		// closestP_A : ����A��̐���B�ւ̍ŋߓ_
		// closestP_B : ����B��̐���B�ւ̍ŋߓ_
		//:::::::::::::::::
		void get_closestP_two_segment(
		const vector3& segAs, const vector3& segAg,
		const vector3& segBs, const vector3& segBg,
		vector3& closestP_A, vector3& closestP_B
		);

		//:::::::::::::::::
		// �����Ɠ_�̍ŋߓ_�����߂�
		// point    : �_
		// l_point  : ������̈�_
		// l_dir    : �����̌���
		// closestP : ������̓_�ւ̍ŋߓ_
		//:::::::::::::::::
		void get_closestP_point_line(
			const vector3& point, 
			const vector3& l_point,
			const vector3& l_dir, 
			const vector3& closestP
		);


		//:::::::::::::::::
		// 3���_���b�V���Ɠ_�̍ŋߓ_�����߂�
		// point       : �_
		// t_point0�`2 : 3���_
		// t_normal     : 3���_���b�V���̖@��
		// closestP     : 3���_���b�V����̓_�ւ̍ŋߓ_
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