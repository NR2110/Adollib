#include "closest_func.h"

using namespace Adollib;


bool Closest_func::get_closestP_two_line(
	const Vector3& l_pA, const Vector3& base_l_vecA,
	const Vector3& l_pB, const Vector3& base_l_vecB,
	float& s, float& t
) {
	Vector3 l_vecA, l_vecB;
	l_vecA = base_l_vecA.unit_vect();
	l_vecB = base_l_vecB.unit_vect();
	const float D1 = vector3_dot(l_pB - l_pA, l_vecA);
	const float D2 = vector3_dot(l_pB - l_pA, l_vecB);
	const float Dv = vector3_dot(l_vecA, l_vecB);

	if (Dv == 1 || Dv == -1)return false; //2���������s

	s = (D1 - D2 * Dv) / (1 - Dv * Dv);
	t = (D2 - D1 * Dv) / (Dv * Dv - 1);

	s /= base_l_vecA.norm_sqr();
	t /= base_l_vecB.norm_sqr();

	return true;
}

#pragma region  get_closestP_two_segment
void Closest_func::get_closestP_two_segment(
	const Vector3& segAs, const Vector3& segAg,
	const Vector3& segBs, const Vector3& segBg,
	Vector3& closestP_A, Vector3& closestP_B,
	float& s, float& t
) {
	get_closestP_two_segment(
		segAs, segAg,
		segBs, segBg,
		s, t
	);
	const Vector3 v1 = segAg - segAs; //A�����̃x�N�g��
	const Vector3 v2 = segBg - segBs; //B�����̃x�N�g��
	closestP_A = segAs + s * v1;
	closestP_B = segBs + t * v2;

}
void Closest_func::get_closestP_two_segment(
	const Vector3& segAs, const Vector3& segAg,
	const Vector3& segBs, const Vector3& segBg,
	Vector3& closestP_A, Vector3& closestP_B
) {

	float s, t;
	get_closestP_two_segment(
		segAs, segAg,
		segBs, segBg,
		s, t
	);
	const Vector3 v1 = segAg - segAs; //A�����̃x�N�g��
	const Vector3 v2 = segBg - segBs; //B�����̃x�N�g��
	closestP_A = segAs + s * v1;
	closestP_B = segBs + t * v2;
}
void Closest_func::get_closestP_two_segment(
	const Vector3& segAs, const Vector3& segAg,
	const Vector3& segBs, const Vector3& segBg,
	float& s, float& t
) {

	Vector3 v1 = segAg - segAs; //A�����̃x�N�g��
	Vector3 v2 = segBg - segBs; //B�����̃x�N�g��

	// v1�܂���v2��0�̎� �_�Ɛ����ɂȂ�
	if (v1.norm() == 0 && v2.norm() != 0) {
		Vector3 p;
		get_closestP_point_segment(segAg, segBs, segBg, p);

		s = 0;
		t = vector3_dot(p - segBs, v2.unit_vect());
		return;
	}
	else if (v2.norm() == 0 && v1.norm() != 0) {
		Vector3 p;
		get_closestP_point_segment(segBg, segAs, segAg, p);

		s = vector3_dot(p - segAs, v1.unit_vect());
		t = 0;
		return;
	}
	else if (v1.norm() == 0 && v2.norm() == 0) {
		s = 0;
		t = 0;
		return;
	}

	Vector3 r = segAs - segBs; //�e�n�_���Ȃ��x�N�g��

	const float a = vector3_dot(v1, v1);
	const float b = vector3_dot(v1, v2);
	const float c = vector3_dot(v2, v2);
	const float d = vector3_dot(v1, r);
	const float e = vector3_dot(v2, r);
	const float det = -a * c + b * b;


	// �t�s��̃`�F�b�N
	if (det * det > FLT_EPSILON) {
		s = (c * d - b * e) / det;
	}

	// s��0�`1�ɃN�����v
	s = ALClamp(s, 0, 1);

	t = (e + s * b) / c;

	// t��0�`1�ɃN�����v
	t = ALClamp(t, 0, 1);

	// �ēxs�����߂�
	s = (-d + t * b) / a;
	s = ALClamp(s, 0, 1);

	//closestP_A = segAs + s * v1;
	//closestP_B = segBs + t * v2;

}
#pragma endregion

//:::::::::::::::::
// �����Ɛ����̍ŋߓ_�����߂�
// line_p, line_d : �����̎n�_�ƌ���
// segB_s - segB_g : �����̎n�_�I�_
// s : �����̍ŋߓ_�ւ̃X�J��
// t : �����̍ŋߓ_�ւ̃X�J��
//:::::::::::::::::
void Closest_func::get_closestP_line_segment(
	const Vector3& segAs, const Vector3& l_dir,
	const Vector3& segBs, const Vector3& segBg,
	float& s, float& t
) {
	Vector3 segAg = segAs + l_dir;

	Vector3 v1 = segAg - segAs; //A�����̃x�N�g��
	Vector3 v2 = segBg - segBs; //B�����̃x�N�g��
	Vector3 r = segAs - segBs; //�e�n�_���Ȃ��x�N�g��

	const float a = vector3_dot(v1, v1);
	const float b = vector3_dot(v1, v2);
	const float c = vector3_dot(v2, v2);
	const float d = vector3_dot(v1, r);
	const float e = vector3_dot(v2, r);
	const float det = -a * c + b * b;
	//float s = 0.0f, t = 0.0f; //�ŋߓ_�̏ꏊ s=0 : segAs


	// �t�s��̃`�F�b�N
	if (det * det > FLT_EPSILON) {
		s = (c * d - b * e) / det;
	}

	// s��0�`1�ɃN�����v
	//s = ALClamp(s, 0, 1);

	t = (e + s * b) / c;

	// t��0�`1�ɃN�����v
	t = ALClamp(t, 0, 1);

	// �ēxs�����߂�
	s = (-d + t * b) / a;
	//s = ALClamp(s, 0, 1);
}

//:::::::::::::::::
// �����Ɠ_�̍ŋߓ_�����߂�
// point    : �_
// l_point  : ������̈�_
// l_dir    : �����̌���
// closestP : ������̓_�ւ̍ŋߓ_
//:::::::::::::::::
void Closest_func::get_closestP_point_line(
	const Vector3& point,
	const Vector3& l_point,
	const Vector3& l_dir,
	Vector3& closestP
) {
	const float s = vector3_dot(point - l_point, l_dir) / vector3_dot(l_dir, l_dir);
	closestP = l_point + s * l_dir;
}
//:::::::::::::::::
// �����Ɠ_�̍ŋߓ_�����߂�
// point    : �_
// l_point  : ������̈�_
// l_dir    : �����̌���
// closests : ������̓_�ւ̃X�J��
//:::::::::::::::::
void Closest_func::get_closestP_point_line(
	const Vector3& point,
	const Vector3& l_point,
	const Vector3& l_dir,
	float& closests
) {
	//float N = vector3_dot(l_dir.unit_vect(), point - l_point) / l_dir.norm_sqr();
	closests = vector3_dot(l_dir, point - l_point) / vector3_dot(l_dir, l_dir);
}

//:::::::::::::::::
// �����Ɠ_�̍ŋߓ_�����߂�
// point    : �_
// l_point  : ������̈�_
// l_dir    : �����̌���
// closestP : ������̓_�ւ̍ŋߓ_
//:::::::::::::::::
void Closest_func::get_closestP_point_segment(
	const Vector3& point,
	const Vector3& l_segS,
	const Vector3& l_segG,
	Vector3& closestP
) {
	const Vector3& l_dir = l_segG - l_segS;
	float N = vector3_dot(l_dir.unit_vect(), point - l_segS) / l_dir.norm_sqr();
	N = ALClamp(N, 0, 1);
	closestP = l_segS + N * l_dir;
}

//:::::::::::::::::
// 3���_���b�V���Ɠ_�̍ŋߓ_�����߂�
// point       : �_
// t_point0�`2 : 3���_
// t_normal     : 3���_���b�V���̖@��
// closestP     : 3���_���b�V����̓_�ւ̍ŋߓ_
//:::::::::::::::::
void Closest_func::get_closestP_point_triangle(
	const Vector3& point,
	const Vector3& t_point0,
	const Vector3& t_point1,
	const Vector3& t_point2,
	const Vector3& t_normal,
	Vector3& closestP
) {

	// �R�p�`�ʏ�̓��e�_
	const Vector3 proj = point - vector3_dot(t_normal, point - t_point0) * t_normal;

	// �G�b�WP0,P1�̃{���m�C�̈�
	const Vector3 edgeP01 = t_point1 - t_point0;
	const Vector3 edgeP01_normal = vector3_cross(edgeP01, t_normal);

	const float voronoiEdgeP01_check1 = vector3_dot(proj - t_point0, edgeP01_normal);
	const float voronoiEdgeP01_check2 = vector3_dot(proj - t_point0, edgeP01);
	const float voronoiEdgeP01_check3 = vector3_dot(proj - t_point1, -edgeP01);

	if (voronoiEdgeP01_check1 > 0.0f && voronoiEdgeP01_check2 > 0.0f && voronoiEdgeP01_check3 > 0.0f) {
		Closest_func::get_closestP_point_line(proj, t_point0, edgeP01, closestP);
		return;
	}

	// �G�b�WP1,P2�̃{���m�C�̈�
	const Vector3 edgeP12 = t_point2 - t_point1;
	const Vector3 edgeP12_normal = vector3_cross(edgeP12, t_normal);

	const float voronoiEdgeP12_check1 = vector3_dot(proj - t_point1, edgeP12_normal);
	const float voronoiEdgeP12_check2 = vector3_dot(proj - t_point1, edgeP12);
	const float voronoiEdgeP12_check3 = vector3_dot(proj - t_point2, -edgeP12);

	if (voronoiEdgeP12_check1 > 0.0f && voronoiEdgeP12_check2 > 0.0f && voronoiEdgeP12_check3 > 0.0f) {
		Closest_func::get_closestP_point_line(proj, t_point1, edgeP12, closestP);
		return;
	}

	// �G�b�WP2,P0�̃{���m�C�̈�
	const Vector3 edgeP20 = t_point0 - t_point2;
	const Vector3 edgeP20_normal = vector3_cross(edgeP20, t_normal);

	const float voronoiEdgeP20_check1 = vector3_dot(proj - t_point2, edgeP20_normal);
	const float voronoiEdgeP20_check2 = vector3_dot(proj - t_point2, edgeP20);
	const float voronoiEdgeP20_check3 = vector3_dot(proj - t_point0, -edgeP20);

	if (voronoiEdgeP20_check1 > 0.0f && voronoiEdgeP20_check2 > 0.0f && voronoiEdgeP20_check3 > 0.0f) {
		Closest_func::get_closestP_point_line(proj, t_point2, edgeP20, closestP);
		return;
	}

	// �R�p�`�ʂ̓���
	if (voronoiEdgeP01_check1 <= 0.0f && voronoiEdgeP12_check1 <= 0.0f && voronoiEdgeP20_check1 <= 0.0f) {
		closestP = proj;
		return;
	}

	// ���_P0�̃{���m�C�̈�
	if (voronoiEdgeP01_check2 <= 0.0f && voronoiEdgeP20_check3 <= 0.0f) {
		closestP = t_point0;
		return;
	}

	// ���_P1�̃{���m�C�̈�
	if (voronoiEdgeP01_check3 <= 0.0f && voronoiEdgeP12_check2 <= 0.0f) {
		closestP = t_point1;
		return;
	}

	// ���_P2�̃{���m�C�̈�
	if (voronoiEdgeP20_check2 <= 0.0f && voronoiEdgeP12_check3 <= 0.0f) {
		closestP = t_point2;
		return;
	}

}

//:::::::::::::::::
// ������Mesh�̍ŋߓ_�����߂�
// segS �` segG : �����̎n�_�I�_
// t_point0�`2 : 3���_
// t_normal     : 3���_���b�V���̖@��
// closest_t	: ������̍ŋߓ_�ւ̎���t segS + closest_t * (segS - segG)
// closest_p     : 3���_���b�V����̐����ւ̍ŋߓ_
//:::::::::::::::::
void Closest_func::get_closestP_segment_triangle(
	const Vector3& segS, const Vector3& segG,
	const Vector3& t_point0,
	const Vector3& t_point1,
	const Vector3& t_point2,
	const Vector3& t_normal,
	float& closest_t,
	Vector3& closest_p
) {
	const float plane_dis = vector3_dot(t_normal, t_point0); //���ʂ̖@��
	Vector3 line_dir = segG - segS; //�����̌���

	if (line_dir.norm() == 0) {
		get_closestP_point_triangle(segG, t_point0, t_point1, t_point2, t_normal, closest_p);
		closest_t = 0;
		return;
	}

	closest_t = (plane_dis - vector3_dot(segS, t_normal)) / vector3_dot(t_normal, line_dir);
	//�����𒼐��Ƃ������ʏ�̌�_
	Vector3 proj = segS + line_dir * closest_t;

	const Vector3 edgeP01 = t_point1 - t_point0; //Mesh�̕�01
	const Vector3 edgeP12 = t_point2 - t_point1; //Mesh�̕�12
	const Vector3 edgeP20 = t_point0 - t_point2; //Mesh�̕�20

	const Vector3 edgeP01_normal = vector3_cross(edgeP01, t_normal);
	const Vector3 edgeP12_normal = vector3_cross(edgeP12, t_normal);
	const Vector3 edgeP20_normal = vector3_cross(edgeP20, t_normal);

	const float voronoiEdgeP01_check1 = vector3_dot(proj - t_point0, edgeP01_normal);
	const float voronoiEdgeP12_check1 = vector3_dot(proj - t_point1, edgeP12_normal);
	const float voronoiEdgeP20_check1 = vector3_dot(proj - t_point2, edgeP20_normal);

	// ��_���R�p�`Mesh�̓����̎�
	if (voronoiEdgeP01_check1 <= 0.0f && voronoiEdgeP12_check1 <= 0.0f && voronoiEdgeP20_check1 <= 0.0f) {
		closest_t = ALClamp(closest_t, 0, 1);

		proj = segS + line_dir * closest_t;
		Closest_func::get_closestP_point_triangle(
			proj,
			t_point0,
			t_point1,
			t_point2,
			t_normal,
			closest_p
		);

		return;
	}


	//������Mesh�ӂƂ̍ŋߓ_���Ƃ�
	float min_dis = FLT_MAX;
	float t, s;

	{
		const Vector3& t_pS = t_point0;
		const Vector3& t_pG = t_point1;

		const Vector3 t_line_dir = (t_pG - t_pS);

		Closest_func::get_closestP_line_segment(
			segS, line_dir,
			t_pS, t_pG,
			t, s
		);
		float dis = ((segS + line_dir * t) - (t_pS + t_line_dir * s)).norm();
		if (dis < min_dis) {
			min_dis = dis;
			closest_t = t;
			closest_p = t_pS + t_line_dir * s;
		}
	}

	{
		const Vector3& t_pS = t_point1;
		const Vector3& t_pG = t_point2;

		const Vector3 t_line_dir = (t_pG - t_pS);

		Closest_func::get_closestP_line_segment(
			segS, line_dir,
			t_pS, t_pG,
			t, s
		);
		float dis = ((segS + line_dir * t) - (t_pS + t_line_dir * s)).norm();
		if (dis < min_dis) {
			min_dis = dis;
			closest_t = t;
			closest_p = t_pS + t_line_dir * s;
		}
	}

	{
		const Vector3& t_pS = t_point2;
		const Vector3& t_pG = t_point0;

		const Vector3 t_line_dir = (t_pG - t_pS);

		Closest_func::get_closestP_line_segment(
			segS, line_dir,
			t_pS, t_pG,
			t, s
		);
		float dis = ((segS + line_dir * t) - (t_pS + t_line_dir * s)).norm();
		if (dis < min_dis) {
			min_dis = dis;
			closest_t = t;
			closest_p = t_pS + t_line_dir * s;
		}
	}

	// closest_t��������ɂ���ΐ�����Mesh�̕ӂōŋߓ_�ƂȂ�
	if (closest_t == ALClamp(closest_t, 0, 1))
		return;

	// closest_t��������ɂȂ���� ���̓_��Mesh�̍ŋߓ_�����߂�
	else {
		closest_t = ALClamp(closest_t, 0, 1);

		proj = segS + line_dir * closest_t;
		Closest_func::get_closestP_point_triangle(
			proj,
			t_point0,
			t_point1,
			t_point2,
			t_normal,
			closest_p
		);

		return;
	}




	return;
}