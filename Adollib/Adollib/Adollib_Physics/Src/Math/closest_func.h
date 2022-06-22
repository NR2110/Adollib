#pragma once
#include "vector.h"
#include "quaternion.h"
#include "matrix.h"

#include <math.h>

namespace Adollib {

	namespace Physics_function {

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
				const Vector3& l_pA, const Vector3& l_vecA,
				const Vector3& l_pB, const Vector3& l_vecB,
				float& s, float& t
			);

			//:::::::::::::::::
			// 2�����̍ŋߓ_�����߂�
			// segA_s - segA_g : ����A�̎n�_�I�_
			// segB_s - segB_g : ����B�̎n�_�I�_
			// closestP_A : ����A��̐���B�ւ̍ŋߓ_
			// closestP_B : ����B��̐���B�ւ̍ŋߓ_
			//:::::::::::::::::
			void get_closestP_two_segment(
				const Vector3& segAs, const Vector3& segAg,
				const Vector3& segBs, const Vector3& segBg,
				Vector3& closestP_A, Vector3& closestP_B
			);
			//:::::::::::::::::
			// 2�����̍ŋߓ_�����߂�
			// segA_s - segA_g : ����A�̎n�_�I�_
			// segB_s - segB_g : ����B�̎n�_�I�_
			// s : ����A�̍ŋߓ_�ւ̃X�J��
			// t : ����B�̍ŋߓ_�ւ̃X�J��
			//:::::::::::::::::
			void get_closestP_two_segment(
				const Vector3& segAs, const Vector3& segAg,
				const Vector3& segBs, const Vector3& segBg,
				float& s, float& t
			);
			//:::::::::::::::::
			// 2�����̍ŋߓ_�����߂�
			// segA_s - segA_g : ����A�̎n�_�I�_
			// segB_s - segB_g : ����B�̎n�_�I�_
					// closestP_A : ����A��̐���B�ւ̍ŋߓ_
			// closestP_B : ����B��̐���B�ւ̍ŋߓ_
			// s : ����A�̍ŋߓ_�ւ̃X�J��
			// t : ����B�̍ŋߓ_�ւ̃X�J��
			//:::::::::::::::::
			void get_closestP_two_segment(
				const Vector3& segAs, const Vector3& segAg,
				const Vector3& segBs, const Vector3& segBg,
				Vector3& closestP_A, Vector3& closestP_B,
				float& s, float& t
			);

			//:::::::::::::::::
			// �����Ɛ����̍ŋߓ_�����߂�
			// line_p, line_d : �����̎n�_�ƌ���
			// segB_s - segB_g : �����̎n�_�I�_
			// s : �����̍ŋߓ_�ւ̃X�J��
			// t : �����̍ŋߓ_�ւ̃X�J��
			//:::::::::::::::::
			void get_closestP_line_segment(
				const Vector3& segAs, const Vector3& segAg,
				const Vector3& segBs, const Vector3& segBg,
				float& s, float& t
			);


			//:::::::::::::::::
			// �����Ɠ_�̍ŋߓ_�����߂�
			// point    : �_
			// l_point  : ������̈�_
			// l_dir    : �����̌���
			// closestP : ������̓_�ւ̍ŋߓ_
			//:::::::::::::::::
			void get_closestP_point_line(
				const Vector3& point,
				const Vector3& l_point,
				const Vector3& l_dir,
				Vector3& closestP
			);
			//:::::::::::::::::
			// �����Ɠ_�̍ŋߓ_�����߂�
			// point    : �_
			// l_point  : ������̈�_
			// l_dir    : �����̌���
			// closests : ������̓_�ւ̃X�J��
			//:::::::::::::::::
			void get_closestP_point_line(
				const Vector3& point,
				const Vector3& l_point,
				const Vector3& l_dir,
				float& closests
			);


			//:::::::::::::::::
			// �����Ɠ_�̍ŋߓ_�����߂�
			// point    : �_
			// l_segS   : �����̎n�_
			// l_segG   : �����̏I�_
			// closestP : ������̓_�ւ̍ŋߓ_
			//:::::::::::::::::
			void get_closestP_point_segment(
				const Vector3& point,
				const Vector3& l_segS,
				const Vector3& l_segG,
				Vector3& closestP
			);
			////:::::::::::::::::
			//// �����Ɠ_�̍ŋߓ_�����߂�
			//// point    : �_
			//// l_point  : ������̈�_
			//// l_dir    : �����̌���
			//// closestP : ������̓_�ւ̍ŋߓ_
			////:::::::::::::::::
			//void get_closestP_point_segment(
			//	const Vector3& point,
			//	const Vector3& l_segS,
			//	const Vector3& l_segG,
			//	float& closests
			//);


			//:::::::::::::::::
			// 3���_���b�V���Ɠ_�̍ŋߓ_�����߂�
			// point       : �_
			// t_point0�`2 : 3���_
			// t_normal     : 3���_���b�V���̖@��
			// closestP     : 3���_���b�V����̓_�ւ̍ŋߓ_
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
			// ������Mesh�̍ŋߓ_�����߂�
			// segS �` segG : �����̎n�_�I�_
			// t_point0�`2 : 3���_
			// t_normal     : 3���_���b�V���̖@��
			// closest_t	: ������̍ŋߓ_�ւ̎���t segS + closest_t * (segS - segG)
			// closest_p     : 3���_���b�V����̐����ւ̍ŋߓ_
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









		}

		namespace Closest_func_SIM {

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
				const DirectX::XMVECTOR& l_pA, const DirectX::XMVECTOR& l_vecA,
				const DirectX::XMVECTOR& l_pB, const DirectX::XMVECTOR& l_vecB,
				float& s, float& t
			);

			//:::::::::::::::::
			// 2�����̍ŋߓ_�����߂�
			// segA_s - segA_g : ����A�̎n�_�I�_
			// segB_s - segB_g : ����B�̎n�_�I�_
			// closestP_A : ����A��̐���B�ւ̍ŋߓ_
			// closestP_B : ����B��̐���B�ւ̍ŋߓ_
			//:::::::::::::::::
			void get_closestP_two_segment(
				const DirectX::XMVECTOR& segAs, const DirectX::XMVECTOR& segAg,
				const DirectX::XMVECTOR& segBs, const DirectX::XMVECTOR& segBg,
				DirectX::XMVECTOR& closestP_A, DirectX::XMVECTOR& closestP_B
			);
			//:::::::::::::::::
			// 2�����̍ŋߓ_�����߂�
			// segA_s - segA_g : ����A�̎n�_�I�_
			// segB_s - segB_g : ����B�̎n�_�I�_
			// s : ����A�̍ŋߓ_�ւ̃X�J��
			// t : ����B�̍ŋߓ_�ւ̃X�J��
			//:::::::::::::::::
			void get_closestP_two_segment(
				const DirectX::XMVECTOR& segAs, const DirectX::XMVECTOR& segAg,
				const DirectX::XMVECTOR& segBs, const DirectX::XMVECTOR& segBg,
				float& s, float& t
			);
			//:::::::::::::::::
			// 2�����̍ŋߓ_�����߂�
			// segA_s - segA_g : ����A�̎n�_�I�_
			// segB_s - segB_g : ����B�̎n�_�I�_
					// closestP_A : ����A��̐���B�ւ̍ŋߓ_
			// closestP_B : ����B��̐���B�ւ̍ŋߓ_
			// s : ����A�̍ŋߓ_�ւ̃X�J��
			// t : ����B�̍ŋߓ_�ւ̃X�J��
			//:::::::::::::::::
			void get_closestP_two_segment(
				const DirectX::XMVECTOR& segAs, const DirectX::XMVECTOR& segAg,
				const DirectX::XMVECTOR& segBs, const DirectX::XMVECTOR& segBg,
				DirectX::XMVECTOR& closestP_A, DirectX::XMVECTOR& closestP_B,
				float& s, float& t
			);

			//:::::::::::::::::
			// �����Ɛ����̍ŋߓ_�����߂�
			// line_p, line_d : �����̎n�_�ƌ���
			// segB_s - segB_g : �����̎n�_�I�_
			// s : �����̍ŋߓ_�ւ̃X�J��
			// t : �����̍ŋߓ_�ւ̃X�J��
			//:::::::::::::::::
			void get_closestP_line_segment(
				const DirectX::XMVECTOR& segAs, const DirectX::XMVECTOR& segAg,
				const DirectX::XMVECTOR& segBs, const DirectX::XMVECTOR& segBg,
				float& s, float& t
			);


			//:::::::::::::::::
			// �����Ɠ_�̍ŋߓ_�����߂�
			// point    : �_
			// l_point  : ������̈�_
			// l_dir    : �����̌���
			// closestP : ������̓_�ւ̍ŋߓ_
			//:::::::::::::::::
			void get_closestP_point_line(
				const DirectX::XMVECTOR& point,
				const DirectX::XMVECTOR& l_point,
				const DirectX::XMVECTOR& l_dir,
				DirectX::XMVECTOR& closestP
			);
			//:::::::::::::::::
			// �����Ɠ_�̍ŋߓ_�����߂�
			// point    : �_
			// l_point  : ������̈�_
			// l_dir    : �����̌���
			// closests : ������̓_�ւ̃X�J��
			//:::::::::::::::::
			void get_closestP_point_line(
				const DirectX::XMVECTOR& point,
				const DirectX::XMVECTOR& l_point,
				const DirectX::XMVECTOR& l_dir,
				float& closests
			);


			//:::::::::::::::::
			// �����Ɠ_�̍ŋߓ_�����߂�
			// point    : �_
			// l_segS   : �����̎n�_
			// l_segG   : �����̏I�_
			// closestP : ������̓_�ւ̍ŋߓ_
			//:::::::::::::::::
			void get_closestP_point_segment(
				const DirectX::XMVECTOR& point,
				const DirectX::XMVECTOR& l_segS,
				const DirectX::XMVECTOR& l_segG,
				DirectX::XMVECTOR& closestP
			);
			////:::::::::::::::::
			//// �����Ɠ_�̍ŋߓ_�����߂�
			//// point    : �_
			//// l_point  : ������̈�_
			//// l_dir    : �����̌���
			//// closestP : ������̓_�ւ̍ŋߓ_
			////:::::::::::::::::
			//void get_closestP_point_segment(
			//	const DirectX::XMVECTOR& point,
			//	const DirectX::XMVECTOR& l_segS,
			//	const DirectX::XMVECTOR& l_segG,
			//	float& closests
			//);


			//:::::::::::::::::
			// 3���_���b�V���Ɠ_�̍ŋߓ_�����߂�
			// point       : �_
			// t_point0�`2 : 3���_
			// t_normal     : 3���_���b�V���̖@��
			// closestP     : 3���_���b�V����̓_�ւ̍ŋߓ_
			//:::::::::::::::::
			void get_closestP_point_triangle(
				const DirectX::XMVECTOR& point,
				const DirectX::XMVECTOR& t_point0,
				const DirectX::XMVECTOR& t_point1,
				const DirectX::XMVECTOR& t_point2,
				const DirectX::XMVECTOR& t_normal,
				DirectX::XMVECTOR& closestP
			);

			//:::::::::::::::::
			// ������Mesh�̍ŋߓ_�����߂�
			// segS �` segG : �����̎n�_�I�_
			// t_point0�`2 : 3���_
			// t_normal     : 3���_���b�V���̖@��
			// closest_t	: ������̍ŋߓ_�ւ̎���t segS + closest_t * (segS - segG)
			// closest_p     : 3���_���b�V����̐����ւ̍ŋߓ_
			//:::::::::::::::::
			void get_closestP_segment_triangle(
				const DirectX::XMVECTOR& segS, const DirectX::XMVECTOR& segG,
				const DirectX::XMVECTOR& t_point0,
				const DirectX::XMVECTOR& t_point1,
				const DirectX::XMVECTOR& t_point2,
				const DirectX::XMVECTOR& t_normal,
				float& closest_t,
				DirectX::XMVECTOR& closest_p
			);









		}
	}
}