#pragma once
#include <algorithm>
#include "vector.h"

using namespace Adollib;

#if _DEBUG || 1
#pragma region operator

inline Matrix Matrix::operator+ () const {
	return *this;
}
inline Matrix Matrix::operator- () const {
	Matrix M = *this;
	M._11 *= -1; 	M._12 *= -1;	M._13 *= -1;	M._14 *= -1;
	M._21 *= -1; 	M._22 *= -1;	M._23 *= -1;	M._24 *= -1;
	M._31 *= -1; 	M._32 *= -1;	M._33 *= -1;	M._34 *= -1;
	M._41 *= -1; 	M._42 *= -1;	M._43 *= -1;	M._44 *= -1;

	return M;
}

inline Matrix Matrix::operator+ (const Matrix& M) const {
	Matrix R;
	R._11 = _11 + M._11; 	R._12 = _12 + M._12;	R._13 = _13 + M._13;	R._14 = _14 + M._14;
	R._21 = _21 + M._21; 	R._22 = _22 + M._22;	R._23 = _23 + M._23;	R._24 = _24 + M._24;
	R._31 = _31 + M._31; 	R._32 = _32 + M._32;	R._33 = _33 + M._33;	R._34 = _34 + M._34;
	R._41 = _41 + M._41; 	R._42 = _42 + M._42;	R._43 = _43 + M._43;	R._44 = _44 + M._44;
	return R;
};
inline Matrix Matrix::operator- (const Matrix& M) const {
	Matrix R;
	R._11 = _11 + M._11; 	R._12 = _12 + M._12;	R._13 = _13 + M._13;	R._14 = _14 + M._14;
	R._21 = _21 + M._21; 	R._22 = _22 + M._22;	R._23 = _23 + M._23;	R._24 = _24 + M._24;
	R._31 = _31 + M._31; 	R._32 = _32 + M._32;	R._33 = _33 + M._33;	R._34 = _34 + M._34;
	R._41 = _41 + M._41; 	R._42 = _42 + M._42;	R._43 = _43 + M._43;	R._44 = _44 + M._44;
	return -R;
}
inline Matrix Matrix::operator* (const Matrix& M) const {
	Matrix R;

	R._11 = _11 * M._11 + _21 * M._12 + _31 * M._13 + _41 * M._14;
	R._12 = _12 * M._11 + _22 * M._12 + _32 * M._13 + _42 * M._14;
	R._13 = _13 * M._11 + _23 * M._12 + _33 * M._13 + _43 * M._14;
	R._14 = _14 * M._11 + _24 * M._12 + _34 * M._13 + _44 * M._14;

	R._21 = _11 * M._21 + _21 * M._22 + _31 * M._23 + _41 * M._24;
	R._22 = _12 * M._21 + _22 * M._22 + _32 * M._23 + _42 * M._24;
	R._23 = _13 * M._21 + _23 * M._22 + _33 * M._23 + _43 * M._24;
	R._24 = _14 * M._21 + _24 * M._22 + _34 * M._23 + _44 * M._24;

	R._31 = _11 * M._31 + _21 * M._32 + _31 * M._33 + _41 * M._34;
	R._32 = _12 * M._31 + _22 * M._32 + _32 * M._33 + _42 * M._34;
	R._33 = _13 * M._31 + _23 * M._32 + _33 * M._33 + _43 * M._34;
	R._34 = _14 * M._31 + _24 * M._32 + _34 * M._33 + _44 * M._34;

	R._41 = _11 * M._41 + _21 * M._42 + _31 * M._43 + _41 * M._44;
	R._42 = _12 * M._41 + _22 * M._42 + _32 * M._43 + _42 * M._44;
	R._43 = _13 * M._41 + _23 * M._42 + _33 * M._43 + _43 * M._44;
	R._44 = _14 * M._41 + _24 * M._42 + _34 * M._43 + _44 * M._44;

	return R;
}
inline Matrix Matrix::operator* (float S) const {
	Matrix R;

	R._11 = _11 * S; 	R._12 = _12 * S;	R._13 = _13 * S;	R._14 = _14 * S;
	R._21 = _21 * S; 	R._22 = _22 * S;	R._23 = _23 * S;	R._24 = _24 * S;
	R._31 = _31 * S; 	R._32 = _32 * S;	R._33 = _33 * S;	R._34 = _34 * S;
	R._41 = _41 * S; 	R._42 = _42 * S;	R._43 = _43 * S;	R._44 = _44 * S;

	return R;
}
inline Matrix Adollib::operator*(float S, const Matrix& M) {
	return M * S;
}

inline Matrix Matrix::operator/ (float S) const {
	Matrix R;
	R._11 = _11 / S; 	R._12 = _12 / S;	R._13 = _13 / S;	R._14 = _14 / S;
	R._21 = _21 / S; 	R._22 = _22 / S;	R._23 = _23 / S;	R._24 = _24 / S;
	R._31 = _31 / S; 	R._32 = _32 / S;	R._33 = _33 / S;	R._34 = _34 / S;
	R._41 = _41 / S; 	R._42 = _42 / S;	R._43 = _43 / S;	R._44 = _44 / S;
	return R;
}

inline Matrix& Matrix::operator+= (const Matrix& M) {
	(*this) = (*this) + M;
	return *this;
}
inline Matrix& Matrix::operator-= (const Matrix& M) {
	(*this) = (*this) - M;
	return *this;
}
inline Matrix& Matrix::operator*= (const Matrix& M) {
	(*this) = (*this) * M;
	return *this;
}
inline Matrix& Matrix::operator*= (float S) {
	(*this) = (*this) * S;
	return *this;
}
inline Matrix& Matrix::operator/= (float S) {
	(*this) = (*this) / S;
	return *this;
}
inline bool Matrix::operator== (const Matrix& M) {

	return (
		_11 == M._11 && _12 == M._12 && _13 == M._13 && _14 == M._14 &&
		_21 == M._21 && _22 == M._22 && _23 == M._23 && _24 == M._24 &&
		_31 == M._31 && _32 == M._32 && _33 == M._33 && _34 == M._34 &&
		_41 == M._41 && _42 == M._42 && _43 == M._43 && _44 == M._44
		);
}
inline bool Matrix::operator!= (const Matrix& M) {

	return (!(*this == M));
}

inline DirectX::XMMATRIX Matrix::get_XMMATRIX() {
	DirectX::XMFLOAT4X4 V = get_XMFLOAT4X4();
	return DirectX::XMLoadFloat4x4(&V);
}
inline DirectX::XMFLOAT4X4 Matrix::get_XMFLOAT4X4() {
	DirectX::XMFLOAT4X4 ret;
	ret._11 = _11; ret._12 = _12; ret._13 = _13; ret._14 = _14;
	ret._21 = _21; ret._22 = _22; ret._23 = _23; ret._24 = _24;
	ret._31 = _31; ret._32 = _32; ret._33 = _33; ret._34 = _34;
	ret._41 = _41; ret._42 = _42; ret._43 = _43; ret._44 = _44;
	return ret;
}

#pragma endregion
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

inline Matrix Adollib::matrix_identity() {
	Matrix M;
	M._11 = M._22 = M._33 = M._44 = 1;
	return M;
}

inline Matrix Adollib::matrix_trans(const Matrix& M) {
	Matrix R = M;
	std::swap(R._21, R._12);
	std::swap(R._31, R._13);	std::swap(R._32, R._23);
	std::swap(R._41, R._14);	std::swap(R._42, R._24);	std::swap(R._43, R._34);
	return R;
}

//3x3の行列式
inline float m3x3_determinant(const Matrix& M) {
	return
		M._11 * M._22 * M._33 + M._12 * M._23 * M._31 + M._13 * M._21 * M._32
		- M._11 * M._23 * M._32 - M._12 * M._21 * M._33 - M._13 * M._22 * M._31;
}

inline float Adollib::matrix_determinant(const Matrix& M) {

	float V1 = M._11 * (M._22 * M._33 * M._44 + M._23 * M._34 * M._42 + M._24 * M._32 * M._43
		- M._24 * M._33 * M._42 - M._23 * M._32 * M._44 - M._22 * M._34 * M._43
		);
	float V2 = M._21 * (M._12 * M._33 * M._44 + M._13 * M._34 * M._42 + M._14 * M._32 * M._43
		- M._14 * M._33 * M._42 - M._13 * M._32 * M._44 - M._12 * M._34 * M._43
		);
	float V3 = M._31 * (M._12 * M._23 * M._44 + M._13 * M._24 * M._42 + M._14 * M._22 * M._43
		- M._14 * M._23 * M._42 - M._13 * M._22 * M._44 - M._12 * M._24 * M._43
		);
	float V4 = M._41 * (M._12 * M._23 * M._34 + M._13 * M._24 * M._32 + M._14 * M._22 * M._33
		- M._14 * M._23 * M._32 - M._13 * M._22 * M._34 - M._12 * M._24 * M._33
		);

	return V1 - V2 + V3 - V4;

}


inline Matrix Adollib::matrix_inverse(const Matrix& M) {

	Matrix R;

#if MATRIX_IS_TRANSFORMATION
	matrix M_v;
	M_v._11 = M._11;	M_v._12 = M._12;	M_v._13 = M._13;
	M_v._21 = M._21;	M_v._22 = M._22;	M_v._23 = M._23;
	M_v._31 = M._31;	M_v._32 = M._32;	M_v._33 = M._33;
	M_v._44 = 1;

	R = matrix_trans(M_v);
	R._41 = -1 * M._41;
	R._42 = -1 * M._42;
	R._43 = -1 * M._43;
#else
	Matrix M_11;
	M_11._11 = M._22;	M_11._12 = M._23;	M_11._13 = M._24;
	M_11._21 = M._32;	M_11._22 = M._33;	M_11._23 = M._34;
	M_11._31 = M._42;	M_11._32 = M._43;	M_11._33 = M._44;
	R._11 = m3x3_determinant(M_11);
	Matrix M_21;
	M_21._11 = M._12;	M_21._12 = M._13;	M_21._13 = M._14;
	M_21._21 = M._32;	M_21._22 = M._33;	M_21._23 = M._34;
	M_21._31 = M._42;	M_21._32 = M._43;	M_21._33 = M._44;
	R._21 = m3x3_determinant(M_21);
	Matrix M_31;
	M_31._11 = M._12;	M_31._12 = M._13;	M_31._13 = M._14;
	M_31._21 = M._22;	M_31._22 = M._23;	M_31._23 = M._24;
	M_31._31 = M._42;	M_31._32 = M._43;	M_31._33 = M._44;
	R._31 = m3x3_determinant(M_31);
	Matrix M_41;
	M_41._11 = M._12;	M_41._12 = M._13;	M_41._13 = M._14;
	M_41._21 = M._22;	M_41._22 = M._23;	M_41._23 = M._24;
	M_41._31 = M._32;	M_41._32 = M._33;	M_41._33 = M._34;
	R._41 = m3x3_determinant(M_41);

	float det = R._11 * M._11 - R._21 * M._21 + R._31 * M._31 - R._41 * M._41;
	if (det == 0) {
		Matrix RR;
		return RR;
	}
	R._21 *= -1;	R._41 *= -1;

	Matrix M_12;
	M_12._11 = M._21;	M_12._12 = M._23;	M_12._13 = M._24;
	M_12._21 = M._31;	M_12._22 = M._33;	M_12._23 = M._34;
	M_12._31 = M._41;	M_12._32 = M._43;	M_12._33 = M._44;
	R._12 = m3x3_determinant(M_12) * -1;
	Matrix M_22;
	M_22._11 = M._11;	M_22._12 = M._13;	M_22._13 = M._14;
	M_22._21 = M._31;	M_22._22 = M._33;	M_22._23 = M._34;
	M_22._31 = M._41;	M_22._32 = M._43;	M_22._33 = M._44;
	R._22 = m3x3_determinant(M_22);
	Matrix M_32;
	M_32._11 = M._11;	M_32._12 = M._13;	M_32._13 = M._14;
	M_32._21 = M._21;	M_32._22 = M._23;	M_32._23 = M._24;
	M_32._31 = M._41;	M_32._32 = M._43;	M_32._33 = M._44;
	R._32 = m3x3_determinant(M_32) * -1;
	Matrix M_42;
	M_42._11 = M._11;	M_42._12 = M._13;	M_42._13 = M._14;
	M_42._21 = M._21;	M_42._22 = M._23;	M_42._23 = M._24;
	M_42._31 = M._31;	M_42._32 = M._33;	M_42._33 = M._34;
	R._42 = m3x3_determinant(M_42);

	Matrix M_13;
	M_13._11 = M._21;	M_13._12 = M._22;	M_13._13 = M._24;
	M_13._21 = M._31;	M_13._22 = M._32;	M_13._23 = M._34;
	M_13._31 = M._41;	M_13._32 = M._42;	M_13._33 = M._44;
	R._13 = m3x3_determinant(M_13);
	Matrix M_23;
	M_23._11 = M._11;	M_23._12 = M._12;	M_23._13 = M._14;
	M_23._21 = M._31;	M_23._22 = M._32;	M_23._23 = M._34;
	M_23._31 = M._41;	M_23._32 = M._42;	M_23._33 = M._44;
	R._23 = m3x3_determinant(M_23) * -1;
	Matrix M_33;
	M_33._11 = M._11;	M_33._12 = M._12;	M_33._13 = M._14;
	M_33._21 = M._21;	M_33._22 = M._22;	M_33._23 = M._24;
	M_33._31 = M._41;	M_33._32 = M._42;	M_33._33 = M._44;
	R._33 = m3x3_determinant(M_33);
	Matrix M_43;
	M_43._11 = M._11;	M_43._12 = M._12;	M_43._13 = M._14;
	M_43._21 = M._21;	M_43._22 = M._22;	M_43._23 = M._24;
	M_43._31 = M._31;	M_43._32 = M._32;	M_43._33 = M._34;
	R._43 = m3x3_determinant(M_43) * -1;

	Matrix M_14;
	M_14._11 = M._21;	M_14._12 = M._22;	M_14._13 = M._23;
	M_14._21 = M._31;	M_14._22 = M._32;	M_14._23 = M._33;
	M_14._31 = M._41;	M_14._32 = M._42;	M_14._33 = M._43;
	R._14 = m3x3_determinant(M_14) * -1;
	Matrix M_24;
	M_24._11 = M._11;	M_24._12 = M._12;	M_24._13 = M._13;
	M_24._21 = M._31;	M_24._22 = M._32;	M_24._23 = M._33;
	M_24._31 = M._41;	M_24._32 = M._42;	M_24._33 = M._43;
	R._24 = m3x3_determinant(M_24);
	Matrix M_34;
	M_34._11 = M._11;	M_34._12 = M._12;	M_34._13 = M._13;
	M_34._21 = M._21;	M_34._22 = M._22;	M_34._23 = M._23;
	M_34._31 = M._41;	M_34._32 = M._42;	M_34._33 = M._43;
	R._34 = m3x3_determinant(M_34) * -1;
	Matrix M_44;
	M_44._11 = M._11;	M_44._12 = M._12;	M_44._13 = M._13;
	M_44._21 = M._21;	M_44._22 = M._22;	M_44._23 = M._23;
	M_44._31 = M._31;	M_44._32 = M._32;	M_44._33 = M._33;
	R._44 = m3x3_determinant(M_44);

	R /= det;
	R = matrix_trans(R);
#endif
	return R;
}

inline Vector3 Adollib::matrix_to_euler(const Matrix& M) {
	Vector3 R;
	if (M._32 >= 1.0f) {
		R.x = DirectX::XM_PIDIV2;
		R.y = 0;
		R.z = atan2f(M._21, M._11);
	}
	else if (M._32 <= -1.0f) {
		R.x = -DirectX::XM_PIDIV2;
		R.y = 0;
		R.z = atan2f(M._21, M._11);
	}
	else {
		R.x = asinf(-M._32);
		R.y = atan2f(M._31, M._33);
		R.z = atan2f(M._12, M._22);
	}
	R.x = ToAngle(R.x);
	R.y = ToAngle(R.y);
	R.z = ToAngle(R.z);

	return R;
}
inline Vector3 Adollib::matrix_to_euler_radian(const Matrix& M) {
	Vector3 R;
	if (M._32 >= 1.0f) {
		R.x = DirectX::XM_PIDIV2;
		R.y = 0;
		R.z = atan2f(M._21, M._11);
	}
	else if (M._32 <= -1.0f) {
		R.x = -DirectX::XM_PIDIV2;
		R.y = 0;
		R.z = atan2f(M._21, M._11);
	}
	else {
		R.x = asinf(-M._32);
		R.y = atan2f(M._31, M._33);
		R.z = atan2f(M._12, M._22);
	}
	return R;
}

inline Vector3 Adollib::vector3_trans(const Vector3& V, const Matrix& M) {
	Vector3 R;
	R.x = M._11 * V.x + M._21 * V.y + M._31 * V.z + M._41 * 1;
	R.y = M._12 * V.x + M._22 * V.y + M._32 * V.z + M._42 * 1;
	R.z = M._13 * V.x + M._23 * V.y + M._33 * V.z + M._43 * 1;

	if (isinf(R.x)) R.x = R.x > 0 ? R.x = FLT_MAX : -FLT_MAX;
	if (isinf(R.y)) R.y = R.y > 0 ? R.y = FLT_MAX : -FLT_MAX;
	if (isinf(R.z)) R.z = R.z > 0 ? R.z = FLT_MAX : -FLT_MAX;

	return R;
}

inline Matrix Adollib::matrix_world(const Vector3& scale, const Matrix& rotate, const Vector3& trans) {
	Matrix ret = rotate;
	ret._11 *= scale.x; ret._12 *= scale.x; ret._13 *= scale.x;
	ret._21 *= scale.y; ret._22 *= scale.y; ret._23 *= scale.y;
	ret._31 *= scale.z; ret._32 *= scale.z; ret._33 *= scale.z;
	ret._41 += trans.x;
	ret._42 += trans.y;
	ret._43 += trans.z;
	ret._44 = 1;

	return ret;
}
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

inline DirectX::XMMATRIX Adollib::matrix_to_XMMATRIX(const Matrix& M) {
	DirectX::XMFLOAT4X4 V;
	V._11 = M._11;	V._12 = M._12;	V._13 = M._13;	V._14 = M._14;
	V._21 = M._21;	V._22 = M._22;	V._23 = M._23;	V._24 = M._24;
	V._31 = M._31;	V._32 = M._32;	V._33 = M._33;	V._34 = M._34;
	V._41 = M._41;	V._42 = M._42;	V._43 = M._43;	V._44 = M._44;

	return DirectX::XMLoadFloat4x4(&V);
}

inline Matrix Adollib::XMMATRIX_to_matrix(const DirectX::XMMATRIX& X) {
	DirectX::XMFLOAT4X4 M;
	DirectX::XMStoreFloat4x4(&M, X);

	Matrix V;
	V._11 = M._11;	V._12 = M._12;	V._13 = M._13;	V._14 = M._14;
	V._21 = M._21;	V._22 = M._22;	V._23 = M._23;	V._24 = M._24;
	V._31 = M._31;	V._32 = M._32;	V._33 = M._33;	V._34 = M._34;
	V._41 = M._41;	V._42 = M._42;	V._43 = M._43;	V._44 = M._44;

	return V;
}

inline Matrix Adollib::matrix_cross(const Vector3& vec){
	return Matrix(
		0.0f, vec.z, -vec.y, 0,
		-vec.z, 0.0f, vec.x, 0,
		vec.y, -vec.x, 0.0f, 0,
		0, 0, 0, 1
	);
}

inline Matrix Adollib::matrix_scale(const Vector3& vec) {
	return Matrix(
		vec.x, 0, 0, 0,
		0, vec.y, 0, 0,
		0, 0, vec.z, 0,
		0, 0, 0, 1
	);
}


#else

#pragma region operator

matrix matrix::operator+ () const {
	return *this;
}
matrix matrix::operator- () const {
	matrix R;
	__m128 inv_ = _mm_set1_ps(-1);

	for (int i = 0; i < 4; i++) {
		_mm_store_ps(R.m[i], _mm_set_ps(m[i][0], m[i][1], m[i][2], m[i][3]));
	}

	return R;
}

matrix matrix::operator+ (const matrix& M) const {
	matrix R;
	__m128 Msim1[4];
	__m128 Msim2[4];
	for (int i = 0; i < 4; i++) {
		Msim1[i] = _mm_load_ps(m[i]);
		Msim2[i] = _mm_load_ps(M.m[i]);
	}

	for (int i = 0; i < 4; i++) {
		_mm_store_ps(R.m[i], _mm_add_ps(Msim1[i], Msim2[i]));
	}

	return R;
};
matrix matrix::operator- (const matrix& M) const {
	matrix R;
	__m128 Msim1[4];
	__m128 Msim2[4];
	for (int i = 0; i < 4; i++) {
		Msim1[i] = _mm_load_ps(m[i]);
		Msim2[i] = _mm_load_ps(M.m[i]);
	}

	for (int i = 0; i < 4; i++) {
		_mm_store_ps(R.m[i], _mm_div_ps(Msim1[i], Msim2[i]));
	}

	return R;
}
matrix matrix::operator* (const matrix& M) const {
	matrix R;
	__m128 Msim1[4];
	__m128 Msim2[4];

	for (int i = 0; i < 4; i++) {
		Msim1[i] = _mm_set_ps(m[0][i], m[1][i], m[2][i], m[3][i]);
		Msim2[i] = _mm_set_ps(M.m[i][0], M.m[i][1], M.m[i][2], M.m[i][3]);
	}

	__declspec(align(16)) float V[4];

	for (int law = 0; law < 4; law++) {
		for (int col = 0; col < 4; col++) {

			_mm_store_ps(V, _mm_mul_ps(Msim1[col], Msim2[law]));
			R.m[law][col] = V[0] + V[1] + V[2] + V[3];
		}
	}

	return R;
}
matrix matrix::operator* (float S) const {
	matrix R;
	__m128 Msim[4];
	__m128 Ssim = _mm_set1_ps(S);
	__declspec(align(16)) float Value[4][4];
	for (int i = 0; i < 4; i++) {
		Msim[i] = _mm_set_ps(m[i][0], m[i][1], m[i][2], m[i][3]);
	}
	for (int i = 0; i < 4; i++) {
		Msim[i] = _mm_mul_ps(Msim[i], Ssim);
	}
	for (int i = 0; i < 4; i++) {
		_mm_store_ps(Value[i], Msim[i]);
		R.m[i][0] = Value[i][3];
		R.m[i][1] = Value[i][2];
		R.m[i][2] = Value[i][1];
		R.m[i][3] = Value[i][0];
	}

	return R;
}
matrix Adollib::operator*(float S, const matrix& M) {
	return M * S;
}

matrix matrix::operator/ (float S) const {
	matrix R;
	float inv_S = 1.0f / S;
	return (*this) * inv_S;
}

matrix& matrix::operator+= (const matrix& M) {
	(*this) = (*this) + M;
	return *this;
}
matrix& matrix::operator-= (const matrix& M) {
	(*this) = (*this) - M;
	return *this;
}
matrix& matrix::operator*= (const matrix& M) {
	(*this) = (*this) * M;
	return *this;
}
matrix& matrix::operator*= (float S) {
	(*this) = (*this) * S;
	return *this;
}
matrix& matrix::operator/= (float S) {
	(*this) = (*this) / S;
	return *this;
}
bool matrix::operator== (const matrix& M) {

	return (
		_11 == M._11 && _12 == M._12 && _13 == M._13 && _14 == M._14 &&
		_21 == M._21 && _22 == M._22 && _23 == M._23 && _24 == M._24 &&
		_31 == M._31 && _32 == M._32 && _33 == M._33 && _34 == M._34 &&
		_41 == M._41 && _42 == M._42 && _43 == M._43 && _44 == M._44
		);
}
bool matrix::operator!= (const matrix& M) {

	return (!(*this == M));
}

DirectX::XMMATRIX matrix::get_XMMATRIX() {
	DirectX::XMFLOAT4X4 V = get_XMFLOAT4X4();
	return DirectX::XMLoadFloat4x4(&V);
}
DirectX::XMFLOAT4X4 matrix::get_XMFLOAT4X4() {
	DirectX::XMFLOAT4X4 ret;
	ret._11 = _11; ret._12 = _12; ret._13 = _13; ret._14 = _14;
	ret._21 = _21; ret._22 = _22; ret._23 = _23; ret._24 = _24;
	ret._31 = _31; ret._32 = _32; ret._33 = _33; ret._34 = _34;
	ret._41 = _41; ret._42 = _42; ret._43 = _43; ret._44 = _44;
	return ret;
}

#pragma endregion
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

matrix Adollib::matrix_identity() {
	matrix M;
	M._11 = M._22 = M._33 = M._44 = 1;
	return M;
}

matrix Adollib::matrix_trans(const matrix& M) {
	matrix R = M;
	std::swap(R._21, R._12);
	std::swap(R._31, R._13);	std::swap(R._32, R._23);
	std::swap(R._41, R._14);	std::swap(R._42, R._24);	std::swap(R._43, R._34);
	return R;
}

//3x3の行列式
float m3x3_determinant(const matrix& M) {
	__declspec(align(16)) float Value[2][4];

	{
		matrix R;
		__m128 Msim[4];
		for (int i = 0; i < 4; i++) {
			Msim[i] = _mm_set_ps(M.m[i][0], M.m[i][1], M.m[i][2], M.m[i][3]);
		}

		__m128 numsim[2];
		numsim[0] = _mm_set1_ps(1);
		numsim[1] = _mm_set1_ps(1);

		for (int num = 0; num < 2; num++) {
			for (int i = 0; i < 3; i++) {
				for (int o = 0; o < i; o++) {
					Msim[i] = _mm_shuffle_ps(Msim[i], Msim[i], _MM_SHUFFLE(2, 1, 3, 0));
				}

				numsim[num] = _mm_mul_ps(numsim[num], Msim[i]);
			}
		}

		_mm_store_ps(Value[0], numsim[0]);
		_mm_store_ps(Value[1], numsim[1]);
	}

	return Value[0][3] + Value[0][2] + Value[0][1] -
		(Value[1][3] + Value[1][2] + Value[1][1]);

}

float Adollib::matrix_determinant(const matrix& M) {

	float V1 = M._11 * (M._22 * M._33 * M._44 + M._23 * M._34 * M._42 + M._24 * M._32 * M._43
		- M._24 * M._33 * M._42 - M._23 * M._32 * M._44 - M._22 * M._34 * M._43
		);
	float V2 = M._21 * (M._12 * M._33 * M._44 + M._13 * M._34 * M._42 + M._14 * M._32 * M._43
		- M._14 * M._33 * M._42 - M._13 * M._32 * M._44 - M._12 * M._34 * M._43
		);
	float V3 = M._31 * (M._12 * M._23 * M._44 + M._13 * M._24 * M._42 + M._14 * M._22 * M._43
		- M._14 * M._23 * M._42 - M._13 * M._22 * M._44 - M._12 * M._24 * M._43
		);
	float V4 = M._41 * (M._12 * M._23 * M._34 + M._13 * M._24 * M._32 + M._14 * M._22 * M._33
		- M._14 * M._23 * M._32 - M._13 * M._22 * M._34 - M._12 * M._24 * M._33
		);

	return V1 - V2 + V3 - V4;

}


matrix Adollib::matrix_inverse(const matrix& M) {

	matrix R;

	__m128 Msim[4];
	__m128 numsim[2];
	__m128 invsim[4][4];
	__declspec(align(16)) float V[4];

	float Value[3][3];
	int V_num[2];
	float det;

	for (int col = 0; col < 4; col++) {
		for (int law = 0; law < 4; law++) {
			numsim[0] = _mm_set1_ps(1);
			numsim[1] = _mm_set1_ps(1);
			V_num[0] = V_num[1] = 0;

			//3x3の行列を準備
			for (int col_ = 0; col_ < 4; col_++) {
				if (col_ == col)continue;

				V_num[1] = 0;
				for (int law_ = 0; law_ < 4; law_++) {
					if (law_ == law)continue;

					Value[V_num[0]][V_num[1]] = M.m[col_][law_];

					V_num[1] ++;
				}

				V_num[0] ++;
			}

			//3x3の行列をsimdに変換
			for (int i = 0; i < 3; i++) {
				Msim[i] = _mm_load_ps(Value[i]);
			}

			//3x3の行列式の計算
			for (int num = 0; num < 2; num++) {
				for (int i = 0; i < 3; i++) {
					for (int o = 0; o < i; o++) {
						Msim[i] = _mm_shuffle_ps(Msim[i], Msim[i], _MM_SHUFFLE(3, 0, 2, 1));
					}

					numsim[num] = _mm_mul_ps(numsim[num], Msim[i]);
				}
			}

			invsim[col][law] = _mm_sub_ps(numsim[0], numsim[1]);

			if (col == 0) {
				for (int law = 0; law < 4; law++) {
					_mm_store_ps(V, invsim[col][law]);
					R.m[col][law] = V[0] + V[1] + V[2] + V[3];
					if ((col + law) % 2 != 0)R.m[col][law] *= -1;
				}

				det = R._11 * M._11 + R._21 * M._21 + R._31 * M._31 + R._41 * M._41;
				if (det == 0) {
					matrix RR;
					return RR;
				}
			}
		}


		for (int col = 1; col < 4; col++) {
			for (int law = 0; law < 4; law++) {

				_mm_store_ps(V, invsim[col][law]);
				R.m[col][law] = V[0] + V[1] + V[2] + V[3];
				if ((col + law) % 2 != 0)R.m[col][law] *= -1;
			}
		}

	}
	//det = R._11 * M._11 + R._21 * M._21 + R._31 * M._31 + R._41 * M._41;

	//if (det == 0) {
	//	matrix RR;
	//	return RR;
	//}

	R /= det;
	R = matrix_trans(R);



	return R;
}

vector3 Adollib::matrix_to_euler(const matrix& M) {
	vector3 R;
	if (M._32 >= 1.0f) {
		R.x = DirectX::XM_PIDIV2;
		R.y = 0;
		R.z = atan2f(M._21, M._11);
	}
	else if (M._32 <= -1.0f) {
		R.x = -DirectX::XM_PIDIV2;
		R.y = 0;
		R.z = atan2f(M._21, M._11);
	}
	else {
		R.x = asinf(-M._32);
		R.y = atan2f(M._31, M._33);
		R.z = atan2f(M._12, M._22);
	}
	R.x = ToAngle(R.x);
	R.y = ToAngle(R.y);
	R.z = ToAngle(R.z);

	return R;
}
vector3 Adollib::matrix_to_euler_radian(const matrix& M) {
	vector3 R;
	if (M._32 >= 1.0f) {
		R.x = DirectX::XM_PIDIV2;
		R.y = 0;
		R.z = atan2f(M._21, M._11);
	}
	else if (M._32 <= -1.0f) {
		R.x = -DirectX::XM_PIDIV2;
		R.y = 0;
		R.z = atan2f(M._21, M._11);
	}
	else {
		R.x = asinf(-M._32);
		R.y = atan2f(M._31, M._33);
		R.z = atan2f(M._12, M._22);
	}
	return R;
}

vector3 Adollib::vector3_trans(const vector3& V, const matrix& M) {
	vector3 R;
	R.x = M._11 * V.x + M._21 * V.y + M._31 * V.z + M._41 * 1;
	R.y = M._12 * V.x + M._22 * V.y + M._32 * V.z + M._42 * 1;
	R.z = M._13 * V.x + M._23 * V.y + M._33 * V.z + M._43 * 1;

	if (isinf(R.x)) R.x = R.x > 0 ? R.x = FLT_MAX : -FLT_MAX;
	if (isinf(R.y)) R.y = R.y > 0 ? R.y = FLT_MAX : -FLT_MAX;
	if (isinf(R.z)) R.z = R.z > 0 ? R.z = FLT_MAX : -FLT_MAX;

	return R;
}

matrix Adollib::matrix_world(const vector3& scale, const matrix& rotate, const vector3& trans) {
	matrix ret = rotate;
	ret._11 *= scale.x; ret._12 *= scale.x; ret._13 *= scale.x;
	ret._21 *= scale.y; ret._22 *= scale.y; ret._23 *= scale.y;
	ret._31 *= scale.z; ret._32 *= scale.z; ret._33 *= scale.z;
	ret._41 += trans.x;
	ret._42 += trans.y;
	ret._43 += trans.z;
	ret._44 = 1;

	return ret;
}
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

DirectX::XMMATRIX Adollib::matrix_to_XMMATRIX(const matrix& M) {
	DirectX::XMFLOAT4X4 V;
	V._11 = M._11;	V._12 = M._12;	V._13 = M._13;	V._14 = M._14;
	V._21 = M._21;	V._22 = M._22;	V._23 = M._23;	V._24 = M._24;
	V._31 = M._31;	V._32 = M._32;	V._33 = M._33;	V._34 = M._34;
	V._41 = M._41;	V._42 = M._42;	V._43 = M._43;	V._44 = M._44;

	return DirectX::XMLoadFloat4x4(&V);
}

matrix Adollib::XMMATRIX_to_matrix(const DirectX::XMMATRIX& X) {
	DirectX::XMFLOAT4X4 M;
	DirectX::XMStoreFloat4x4(&M, X);

	matrix V;
	V._11 = M._11;	V._12 = M._12;	V._13 = M._13;	V._14 = M._14;
	V._21 = M._21;	V._22 = M._22;	V._23 = M._23;	V._24 = M._24;
	V._31 = M._31;	V._32 = M._32;	V._33 = M._33;	V._34 = M._34;
	V._41 = M._41;	V._42 = M._42;	V._43 = M._43;	V._44 = M._44;

	return V;
}
#endif