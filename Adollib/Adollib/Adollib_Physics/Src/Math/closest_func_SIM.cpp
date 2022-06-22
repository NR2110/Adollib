#include "closest_func.h"

using namespace Adollib;
using namespace DirectX;
using namespace Physics_function;


bool Closest_func_SIM::get_closestP_two_line(
	const XMVECTOR& l_pA, const XMVECTOR& base_l_vecA,
	const XMVECTOR& l_pB, const XMVECTOR& base_l_vecB,
	float& s, float& t
) {
	const float vecA_length = XMVectorGetX(XMVector3Length(base_l_vecA));
	const float vecB_length = XMVectorGetX(XMVector3Length(base_l_vecB));

	const XMVECTOR l_vecA = base_l_vecA / vecA_length;
	const XMVECTOR l_vecB = base_l_vecB / vecB_length;
	const float D1 = XMVectorGetX(XMVector3Dot(l_pB - l_pA, l_vecA));
	const float D2 = XMVectorGetX(XMVector3Dot(l_pB - l_pA, l_vecB));
	const float Dv = XMVectorGetX(XMVector3Dot(l_vecA, l_vecB));

	if (Dv == 1 || Dv == -1)return false; //2���������s

	s = (D1 - D2 * Dv) / (1 - Dv * Dv);
	t = (D2 - D1 * Dv) / (Dv * Dv - 1);

	s /= vecA_length;
	t /= vecB_length;

	return true;
}

#pragma region  get_closestP_two_segment
void Closest_func_SIM::get_closestP_two_segment(
	const XMVECTOR& segAs, const XMVECTOR& segAg,
	const XMVECTOR& segBs, const XMVECTOR& segBg,
	XMVECTOR& closestP_A, XMVECTOR& closestP_B,
	float& s, float& t
) {
	get_closestP_two_segment(
		segAs, segAg,
		segBs, segBg,
		s, t
	);
	const XMVECTOR v1 = segAg - segAs; //A�����̃x�N�g��
	const XMVECTOR v2 = segBg - segBs; //B�����̃x�N�g��
	closestP_A = segAs + XMVectorScale(v1, s);
	closestP_B = segBs + XMVectorScale(v2, t);

}
void Closest_func_SIM::get_closestP_two_segment(
	const XMVECTOR& segAs, const XMVECTOR& segAg,
	const XMVECTOR& segBs, const XMVECTOR& segBg,
	XMVECTOR& closestP_A, XMVECTOR& closestP_B
) {

	float s, t;
	get_closestP_two_segment(
		segAs, segAg,
		segBs, segBg,
		s, t
	);
	const XMVECTOR v1 = segAg - segAs; //A�����̃x�N�g��
	const XMVECTOR v2 = segBg - segBs; //B�����̃x�N�g��
	closestP_A = segAs + XMVectorScale(v1, s);
	closestP_B = segBs + XMVectorScale(v2, t);
}
void Closest_func_SIM::get_closestP_two_segment(
	const XMVECTOR& segAs, const XMVECTOR& segAg,
	const XMVECTOR& segBs, const XMVECTOR& segBg,
	float& s, float& t
) {

	const XMVECTOR v1 = segAg - segAs; //A�����̃x�N�g��
	const XMVECTOR v2 = segBg - segBs; //B�����̃x�N�g��

	const float v1_length = XMVectorGetX(XMVector3Length(v1));
	const float v2_length = XMVectorGetX(XMVector3Length(v2));

	// v1�܂���v2��0�̎� �_�Ɛ����ɂȂ�
	if (v1_length == 0 && v2_length != 0) {
		XMVECTOR p;
		get_closestP_point_segment(segAg, segBs, segBg, p);

		s = 0;
		t = XMVectorGetX(XMVector3Dot(p - segBs, v2 / v2_length));
		return;
	}
	else if (v2_length == 0 && v1_length != 0) {
		XMVECTOR p;
		get_closestP_point_segment(segBg, segAs, segAg, p);

		s = XMVectorGetX(XMVector3Dot(p - segAs, v1 / v1_length));
		t = 0;
		return;
	}
	else if (v1_length == 0 && v2_length == 0) {
		s = 0;
		t = 0;
		return;
	}

	XMVECTOR r = segAs - segBs; //�e�n�_���Ȃ��x�N�g��

	const float a = XMVectorGetX(XMVector3Dot(v1, v1));
	const float b = XMVectorGetX(XMVector3Dot(v1, v2));
	const float c = XMVectorGetX(XMVector3Dot(v2, v2));
	const float d = XMVectorGetX(XMVector3Dot(v1, r));
	const float e = XMVectorGetX(XMVector3Dot(v2, r));
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
void Closest_func_SIM::get_closestP_line_segment(
	const XMVECTOR& segAs, const XMVECTOR& l_dir,
	const XMVECTOR& segBs, const XMVECTOR& segBg,
	float& s, float& t
) {
	const XMVECTOR segAg = segAs + l_dir;

	const XMVECTOR v1 = segAg - segAs; //A�����̃x�N�g��
	const XMVECTOR v2 = segBg - segBs; //B�����̃x�N�g��
	const XMVECTOR r = segAs - segBs; //�e�n�_���Ȃ��x�N�g��

	const float a = XMVectorGetX(XMVector3Dot(v1, v1));
	const float b = XMVectorGetX(XMVector3Dot(v1, v2));
	const float c = XMVectorGetX(XMVector3Dot(v2, v2));
	const float d = XMVectorGetX(XMVector3Dot(v1, r));
	const float e = XMVectorGetX(XMVector3Dot(v2, r));
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
void Closest_func_SIM::get_closestP_point_line(
	const XMVECTOR& point,
	const XMVECTOR& l_point,
	const XMVECTOR& l_dir,
	XMVECTOR& closestP
) {
	const float s = XMVectorGetX(XMVector3Dot(point - l_point, l_dir) / XMVector3Dot(l_dir, l_dir));
	closestP = l_point + XMVectorScale(l_dir, s);
}
//:::::::::::::::::
// �����Ɠ_�̍ŋߓ_�����߂�
// point    : �_
// l_point  : ������̈�_
// l_dir    : �����̌���
// closests : ������̓_�ւ̃X�J��
//:::::::::::::::::
void Closest_func_SIM::get_closestP_point_line(
	const XMVECTOR& point,
	const XMVECTOR& l_point,
	const XMVECTOR& l_dir,
	float& closests
) {
	//float N = vector3_dot(l_dir.unit_vect(), point - l_point) / l_dir.norm_sqr();
	closests = XMVectorGetX(XMVector3Dot(l_dir, point - l_point) / XMVector3Dot(l_dir, l_dir));
}

//:::::::::::::::::
// �����Ɠ_�̍ŋߓ_�����߂�
// point    : �_
// l_point  : ������̈�_
// l_dir    : �����̌���
// closestP : ������̓_�ւ̍ŋߓ_
//:::::::::::::::::
void Closest_func_SIM::get_closestP_point_segment(
	const XMVECTOR& point,
	const XMVECTOR& l_segS,
	const XMVECTOR& l_segG,
	XMVECTOR& closestP
) {
	const XMVECTOR l_dir = l_segG - l_segS;
	const XMVECTOR l_dir_length = XMVector3Length(l_dir);
	const XMVECTOR l_dir_unitvect = l_dir / l_dir_length;



	float N = XMVectorGetX(XMVector3Dot(l_dir_unitvect, point - l_segS) / l_dir_length);
	N = ALClamp(N, 0, 1);
	closestP = l_segS + XMVectorScale(l_dir, N);
}

//:::::::::::::::::
// 3���_���b�V���Ɠ_�̍ŋߓ_�����߂�
// point       : �_
// t_point0�`2 : 3���_
// t_normal     : 3���_���b�V���̖@��
// closestP     : 3���_���b�V����̓_�ւ̍ŋߓ_
//:::::::::::::::::
void Closest_func_SIM::get_closestP_point_triangle(
	const XMVECTOR& point,
	const XMVECTOR& t_point0,
	const XMVECTOR& t_point1,
	const XMVECTOR& t_point2,
	const XMVECTOR& t_normal,
	XMVECTOR& closestP
) {

	// �R�p�`�ʏ�̓��e�_
	const XMVECTOR proj = point - XMVectorMultiply(XMVector3Dot(t_normal, point - t_point0), t_normal);

	// �G�b�WP0,P1�̃{���m�C�̈�
	const XMVECTOR edgeP01 = t_point1 - t_point0;
	const XMVECTOR edgeP01_normal = XMVector3Cross(edgeP01, t_normal);

	const float voronoiEdgeP01_check1 = XMVectorGetX(XMVector3Dot(proj - t_point0, edgeP01_normal));
	const float voronoiEdgeP01_check2 = XMVectorGetX(XMVector3Dot(proj - t_point0, edgeP01));
	const float voronoiEdgeP01_check3 = XMVectorGetX(XMVector3Dot(proj - t_point1, -edgeP01));

	if (voronoiEdgeP01_check1 > 0.0f && voronoiEdgeP01_check2 > 0.0f && voronoiEdgeP01_check3 > 0.0f) {
		Closest_func_SIM::get_closestP_point_line(proj, t_point0, edgeP01, closestP);
		return;
	}

	// �G�b�WP1,P2�̃{���m�C�̈�
	const XMVECTOR edgeP12 = t_point2 - t_point1;
	const XMVECTOR edgeP12_normal = XMVector3Cross(edgeP12, t_normal);

	const float voronoiEdgeP12_check1 = XMVectorGetX(XMVector3Dot(proj - t_point1, edgeP12_normal));
	const float voronoiEdgeP12_check2 = XMVectorGetX(XMVector3Dot(proj - t_point1, edgeP12));
	const float voronoiEdgeP12_check3 = XMVectorGetX(XMVector3Dot(proj - t_point2, -edgeP12));

	if (voronoiEdgeP12_check1 > 0.0f && voronoiEdgeP12_check2 > 0.0f && voronoiEdgeP12_check3 > 0.0f) {
		Closest_func_SIM::get_closestP_point_line(proj, t_point1, edgeP12, closestP);
		return;
	}

	// �G�b�WP2,P0�̃{���m�C�̈�
	const XMVECTOR edgeP20 = t_point0 - t_point2;
	const XMVECTOR edgeP20_normal = XMVector3Cross(edgeP20, t_normal);

	const float voronoiEdgeP20_check1 = XMVectorGetX(XMVector3Dot(proj - t_point2, edgeP20_normal));
	const float voronoiEdgeP20_check2 = XMVectorGetX(XMVector3Dot(proj - t_point2, edgeP20));
	const float voronoiEdgeP20_check3 = XMVectorGetX(XMVector3Dot(proj - t_point0, -edgeP20));

	if (voronoiEdgeP20_check1 > 0.0f && voronoiEdgeP20_check2 > 0.0f && voronoiEdgeP20_check3 > 0.0f) {
		Closest_func_SIM::get_closestP_point_line(proj, t_point2, edgeP20, closestP);
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
void Closest_func_SIM::get_closestP_segment_triangle(
	const XMVECTOR& segS, const XMVECTOR& segG,
	const XMVECTOR& t_point0,
	const XMVECTOR& t_point1,
	const XMVECTOR& t_point2,
	const XMVECTOR& t_normal,
	float& closest_t,
	XMVECTOR& closest_p
) {
	const XMVECTOR plane_dis = XMVector3Dot(t_normal, t_point0); //���ʂ̖@��
	const XMVECTOR line_dir = segG - segS; //�����̌���

	if ((XMVector3LengthSq(line_dir).m128_f32[0]) == 0) {
		get_closestP_point_triangle(segG, t_point0, t_point1, t_point2, t_normal, closest_p);
		closest_t = 0;
		return;
	}

	closest_t = ((plane_dis - XMVector3Dot(segS, t_normal)) / XMVector3Dot(t_normal, line_dir)).m128_f32[0];
	//�����𒼐��Ƃ������ʏ�̌�_
	XMVECTOR proj = segS + XMVectorScale(line_dir, closest_t);

	const XMVECTOR edgeP01 = t_point1 - t_point0; //Mesh�̕�01
	const XMVECTOR edgeP12 = t_point2 - t_point1; //Mesh�̕�12
	const XMVECTOR edgeP20 = t_point0 - t_point2; //Mesh�̕�20

	const XMVECTOR edgeP01_normal = XMVector3Cross(edgeP01, t_normal);
	const XMVECTOR edgeP12_normal = XMVector3Cross(edgeP12, t_normal);
	const XMVECTOR edgeP20_normal = XMVector3Cross(edgeP20, t_normal);

	const float voronoiEdgeP01_check1 = XMVector3Dot(proj - t_point0, edgeP01_normal).m128_f32[0];
	const float voronoiEdgeP12_check1 = XMVector3Dot(proj - t_point1, edgeP12_normal).m128_f32[0];
	const float voronoiEdgeP20_check1 = XMVector3Dot(proj - t_point2, edgeP20_normal).m128_f32[0];

	// ��_���R�p�`Mesh�̓����̎�
	if (voronoiEdgeP01_check1 <= 0.0f && voronoiEdgeP12_check1 <= 0.0f && voronoiEdgeP20_check1 <= 0.0f) {
		closest_t = ALClamp(closest_t, 0, 1);

		proj = segS + XMVectorScale(line_dir, closest_t);
		Closest_func_SIM::get_closestP_point_triangle(
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
		const XMVECTOR& t_pS = t_point0;
		const XMVECTOR& t_pG = t_point1;

		const XMVECTOR t_line_dir = (t_pG - t_pS);

		Closest_func_SIM::get_closestP_line_segment(
			segS, line_dir,
			t_pS, t_pG,
			t, s
		);
		float dis = XMVector3LengthSq((segS + XMVectorScale(line_dir, t)) - (t_pS + XMVectorScale(t_line_dir, s))).m128_f32[0];
		if (dis < min_dis) {
			min_dis = dis;
			closest_t = t;
			closest_p = t_pS + XMVectorScale(t_line_dir, s);
		}
	}

	{
		const XMVECTOR& t_pS = t_point1;
		const XMVECTOR& t_pG = t_point2;

		const XMVECTOR t_line_dir = (t_pG - t_pS);

		Closest_func_SIM::get_closestP_line_segment(
			segS, line_dir,
			t_pS, t_pG,
			t, s
		);
		float dis = XMVectorGetX(XMVector3LengthSq((segS + XMVectorScale(line_dir, t)) - (t_pS + XMVectorScale(t_line_dir, s))));
		if (dis < min_dis) {
			min_dis = dis;
			closest_t = t;
			closest_p = t_pS + XMVectorScale(t_line_dir, s);
		}
	}

	{
		const XMVECTOR& t_pS = t_point2;
		const XMVECTOR& t_pG = t_point0;

		const XMVECTOR t_line_dir = (t_pG - t_pS);

		Closest_func_SIM::get_closestP_line_segment(
			segS, line_dir,
			t_pS, t_pG,
			t, s
		);
		float dis = XMVectorGetX(XMVector3LengthSq((segS + XMVectorScale(line_dir, t)) - (t_pS + XMVectorScale(t_line_dir, s))));
		if (dis < min_dis) {
			min_dis = dis;
			closest_t = t;
			closest_p = t_pS + XMVectorScale(t_line_dir, s);
		}
	}

	// closest_t��������ɂ���ΐ�����Mesh�̕ӂōŋߓ_�ƂȂ�
	if (closest_t == ALClamp(closest_t, 0, 1))
		return;

	// closest_t��������ɂȂ���� ���̓_��Mesh�̍ŋߓ_�����߂�
	else {
		closest_t = ALClamp(closest_t, 0, 1);

		proj = segS + XMVectorScale(line_dir, closest_t);
		Closest_func_SIM::get_closestP_point_triangle(
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