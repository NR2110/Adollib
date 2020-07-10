#include "closest_func.h"

using namespace Adollib;


bool Closest_func::get_closestP_two_line(
	const vector3& l_pA, const vector3& base_l_vecA,
	const vector3& l_pB, const vector3& base_l_vecB,
	float& s, float&t
) {
	vector3 l_vecA, l_vecB;
	l_vecA = base_l_vecA.unit_vect();
	l_vecB = base_l_vecB.unit_vect();
	float D1 = vector3_dot(l_pB - l_pA, l_vecA);
	float D2 = vector3_dot(l_pB - l_pA, l_vecB);
	float Dv = vector3_dot(l_vecA, l_vecB);

	if (Dv == 1 || Dv == -1)return false; //2���������s

	s = (D1 - D2 * Dv) / (1 - Dv * Dv);
	t = (D2 - D1 * Dv) / (Dv * Dv - 1);

	return true;
}

void Closest_func::get_closestP_two_segment(
	const vector3& segAs, const vector3& segAg,
	const vector3& segBs, const vector3& segBg,
	vector3& closestP_A, vector3& closestP_B
) {

	vector3 v1 = segAg - segAs; //A�����̃x�N�g��
	vector3 v2 = segBg - segBs; //B�����̃x�N�g��
	vector3 r  = segAs - segBs; //�e�n�_���Ȃ��x�N�g��

	float a = vector3_dot(v1, v1);
	float b = vector3_dot(v1, v2);
	float c = vector3_dot(v2, v2);
	float d = vector3_dot(v1, r);
	float e = vector3_dot(v2, r);
	float det = -a * c + b * b;
	float s = 0.0f, t = 0.0f; //�ŋߓ_�̏ꏊ s=0 : segAs
	

	// �t�s��̃`�F�b�N
	if (det * det > FLT_EPSILON) {
		s = (c * d - b * e) / det;
	}

	// s��0�`1�ɃN�����v
	s = s < 0.0f ? 0.0f : s > 1.0f ? 1.0f : s;

	t = (e + s * b) / c;

	// t��0�`1�ɃN�����v
	t = t < 0.0f ? 0.0f : t > 1.0f ? 1.0f : t;

	// �ēxs�����߂�
	s = (-d + t * b) / a;
	s = s < 0.0f ? 0.0f : s > 1.0f ? 1.0f : s;

	closestP_A = segAs + s * v1;
	closestP_B = segBs + t * v2;

}