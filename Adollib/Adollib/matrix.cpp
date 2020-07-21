
#include "matrix.h"
#include "math.h"
#include <algorithm>

using namespace Adollib;

#define MATRIX_IS_TRANSFORMATION (0); //matrix‚ð•ÏŠ·s—ñ‚Æ‚µ‚Ä‚Ì‚Ýˆµ‚¤‚©‚Ç‚¤‚©

#pragma region operator

matrix matrix::operator+ () const {
	return *this;
}
matrix matrix::operator- () const {
	matrix M = *this;
	M._11 *= -1; 	M._12 *= -1;	M._13 *= -1;	M._14 *= -1;
	M._21 *= -1; 	M._22 *= -1;	M._23 *= -1;	M._24 *= -1;
	M._31 *= -1; 	M._32 *= -1;	M._33 *= -1;	M._34 *= -1;
	M._41 *= -1; 	M._42 *= -1;	M._43 *= -1;	M._44 *= -1;

	return M;
}

matrix matrix::operator+ (const matrix& M) const {
	matrix R;
	R._11 = _11 + M._11; 	R._12 = _12 + M._12;	R._13 = _13 + M._13;	R._14 = _14 + M._14;
	R._21 = _21 + M._21; 	R._22 = _22 + M._22;	R._23 = _23 + M._23;	R._24 = _24 + M._24;
	R._31 = _31 + M._31; 	R._32 = _32 + M._32;	R._33 = _33 + M._33;	R._34 = _34 + M._34;
	R._41 = _41 + M._41; 	R._42 = _42 + M._42;	R._43 = _43 + M._43;	R._44 = _44 + M._44;
	return R;
};
matrix matrix::operator- (const matrix& M) const {
	matrix R;
	R._11 = _11 + M._11; 	R._12 = _12 + M._12;	R._13 = _13 + M._13;	R._14 = _14 + M._14;
	R._21 = _21 + M._21; 	R._22 = _22 + M._22;	R._23 = _23 + M._23;	R._24 = _24 + M._24;
	R._31 = _31 + M._31; 	R._32 = _32 + M._32;	R._33 = _33 + M._33;	R._34 = _34 + M._34;
	R._41 = _41 + M._41; 	R._42 = _42 + M._42;	R._43 = _43 + M._43;	R._44 = _44 + M._44;
	return -R;
}
matrix matrix::operator* (const matrix& M) const {
	matrix R;

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
matrix matrix::operator* (float S) const {
	matrix R;

	R._11 = _11 * S; 	R._12 = _12 * S;	R._13 = _13 * S;	R._14 = _14 * S;
	R._21 = _21 * S; 	R._22 = _22 * S;	R._23 = _23 * S;	R._24 = _24 * S;
	R._31 = _31 * S; 	R._32 = _32 * S;	R._33 = _33 * S;	R._34 = _34 * S;
	R._41 = _41 * S; 	R._42 = _42 * S;	R._43 = _43 * S;	R._44 = _44 * S;

	return R;
}
matrix Adollib::operator*(float S, const matrix& M){
	return M * S;
}

matrix matrix::operator/ (float S) const {
	matrix R;
	R._11 = _11 / S; 	R._12 = _12 / S;	R._13 = _13 / S;	R._14 = _14 / S;
	R._21 = _21 / S; 	R._22 = _22 / S;	R._23 = _23 / S;	R._24 = _24 / S;
	R._31 = _31 / S; 	R._32 = _32 / S;	R._33 = _33 / S;	R._34 = _34 / S;
	R._41 = _41 / S; 	R._42 = _42 / S;	R._43 = _43 / S;	R._44 = _44 / S;
	return R;
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

DirectX::XMMATRIX matrix::get_XMMATRIX(){
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

matrix Adollib::matrix_trans(const matrix& M){
	matrix R = M;
	std::swap(R._21, R._12); 
	std::swap(R._31, R._13);	std::swap(R._32, R._23);
	std::swap(R._41, R._14);	std::swap(R._42, R._24);	std::swap(R._43, R._34);
	return R;
}

//3x3‚Ìs—ñŽ®
float m3x3_determinant(const matrix& M) {
	return
		M._11 * M._22 * M._33 + M._12 * M._23 * M._31 + M._13 * M._21 * M._32 
		- M._11 * M._23 * M._32 - M._12 * M._21 * M._33 - M._13 * M._22 * M._31;
}

float Adollib::matrix_determinant(const matrix& M){

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
	matrix M_11;
	M_11._11 = M._22;	M_11._12 = M._23;	M_11._13 = M._24;
	M_11._21 = M._32;	M_11._22 = M._33;	M_11._23 = M._34;
	M_11._31 = M._42;	M_11._32 = M._43;	M_11._33 = M._44;
	R._11 = m3x3_determinant(M_11);
	matrix M_21;
	M_21._11 = M._12;	M_21._12 = M._13;	M_21._13 = M._14;
	M_21._21 = M._32;	M_21._22 = M._33;	M_21._23 = M._34;
	M_21._31 = M._42;	M_21._32 = M._43;	M_21._33 = M._44;
	R._21 = m3x3_determinant(M_21);
	matrix M_31;
	M_31._11 = M._12;	M_31._12 = M._13;	M_31._13 = M._14;
	M_31._21 = M._22;	M_31._22 = M._23;	M_31._23 = M._24;
	M_31._31 = M._42;	M_31._32 = M._43;	M_31._33 = M._44;
	R._31 = m3x3_determinant(M_31);
	matrix M_41;
	M_41._11 = M._12;	M_41._12 = M._13;	M_41._13 = M._14;
	M_41._21 = M._22;	M_41._22 = M._23;	M_41._23 = M._24;
	M_41._31 = M._32;	M_41._32 = M._33;	M_41._33 = M._34;
	R._41 = m3x3_determinant(M_41);

	float det = R._11 * M._11 - R._21 * M._21 + R._31 * M._31 - R._41 * M._41;
	if (det == 0) {
		matrix RR;
		return RR;
	}
	R._21 *= -1;	R._41 *= -1;

	matrix M_12;
	M_12._11 = M._21;	M_12._12 = M._23;	M_12._13 = M._24;
	M_12._21 = M._31;	M_12._22 = M._33;	M_12._23 = M._34;
	M_12._31 = M._41;	M_12._32 = M._43;	M_12._33 = M._44;
	R._12 = m3x3_determinant(M_12) * -1;
	matrix M_22;
	M_22._11 = M._11;	M_22._12 = M._13;	M_22._13 = M._14;
	M_22._21 = M._31;	M_22._22 = M._33;	M_22._23 = M._34;
	M_22._31 = M._41;	M_22._32 = M._43;	M_22._33 = M._44;
	R._22 = m3x3_determinant(M_22);
	matrix M_32;
	M_32._11 = M._11;	M_32._12 = M._13;	M_32._13 = M._14;
	M_32._21 = M._21;	M_32._22 = M._23;	M_32._23 = M._24;
	M_32._31 = M._41;	M_32._32 = M._43;	M_32._33 = M._44;
	R._32 = m3x3_determinant(M_32) * -1;
	matrix M_42;
	M_42._11 = M._11;	M_42._12 = M._13;	M_42._13 = M._14;
	M_42._21 = M._21;	M_42._22 = M._23;	M_42._23 = M._24;
	M_42._31 = M._31;	M_42._32 = M._33;	M_42._33 = M._34;
	R._42 = m3x3_determinant(M_42);

	matrix M_13;
	M_13._11 = M._21;	M_13._12 = M._22;	M_13._13 = M._24;
	M_13._21 = M._31;	M_13._22 = M._32;	M_13._23 = M._34;
	M_13._31 = M._41;	M_13._32 = M._42;	M_13._33 = M._44;
	R._13 = m3x3_determinant(M_13);
	matrix M_23;
	M_23._11 = M._11;	M_23._12 = M._12;	M_23._13 = M._14;
	M_23._21 = M._31;	M_23._22 = M._32;	M_23._23 = M._34;
	M_23._31 = M._41;	M_23._32 = M._42;	M_23._33 = M._44;
	R._23 = m3x3_determinant(M_23) * -1;
	matrix M_33;
	M_33._11 = M._11;	M_33._12 = M._12;	M_33._13 = M._14;
	M_33._21 = M._21;	M_33._22 = M._22;	M_33._23 = M._24;
	M_33._31 = M._41;	M_33._32 = M._42;	M_33._33 = M._44;
	R._33 = m3x3_determinant(M_33);
	matrix M_43;
	M_43._11 = M._11;	M_43._12 = M._12;	M_43._13 = M._14;
	M_43._21 = M._21;	M_43._22 = M._22;	M_43._23 = M._24;
	M_43._31 = M._31;	M_43._32 = M._32;	M_43._33 = M._34;
	R._43 = m3x3_determinant(M_43) * -1;

	matrix M_14;
	M_14._11 = M._21;	M_14._12 = M._22;	M_14._13 = M._23;
	M_14._21 = M._31;	M_14._22 = M._32;	M_14._23 = M._33;
	M_14._31 = M._41;	M_14._32 = M._42;	M_14._33 = M._43;
	R._14 = m3x3_determinant(M_14) * -1;
	matrix M_24;
	M_24._11 = M._11;	M_24._12 = M._12;	M_24._13 = M._13;
	M_24._21 = M._31;	M_24._22 = M._32;	M_24._23 = M._33;
	M_24._31 = M._41;	M_24._32 = M._42;	M_24._33 = M._43;
	R._24 = m3x3_determinant(M_24);
	matrix M_34;
	M_34._11 = M._11;	M_34._12 = M._12;	M_34._13 = M._13;
	M_34._21 = M._21;	M_34._22 = M._22;	M_34._23 = M._23;
	M_34._31 = M._41;	M_34._32 = M._42;	M_34._33 = M._43;
	R._34 = m3x3_determinant(M_34) * -1;
	matrix M_44;
	M_44._11 = M._11;	M_44._12 = M._12;	M_44._13 = M._13;
	M_44._21 = M._21;	M_44._22 = M._22;	M_44._23 = M._23;
	M_44._31 = M._31;	M_44._32 = M._32;	M_44._33 = M._33;
	R._44 = m3x3_determinant(M_44);

	R /= det;
	R = matrix_trans(R);
#endif
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