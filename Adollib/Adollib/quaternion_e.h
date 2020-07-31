#pragma once

#include "vector.h"
#include "matrix.h"
using namespace Adollib;

#pragma region constract

inline quaternion::quaternion() : x(0), y(0), z(0), w(0) {
}
inline quaternion::quaternion(float x, float y, float z) : x(x), y(y), z(z), w(0) {
}
inline quaternion::quaternion(DirectX::XMFLOAT3 Q) : x(Q.x), y(Q.y), z(Q.z), w(0) {
}
inline quaternion::quaternion(vector3 Q) : x(Q.x), y(Q.y), z(Q.z), w(0) {
}
//quaternion::quaternion(VECTOR4 Q) : x(Q.x), y(Q.y), z(Q.z), w(Q.w) {
//}
inline quaternion::quaternion(float w, float x, float y, float z) : x(x), y(y), z(z), w(w) {
}
inline quaternion::quaternion(const quaternion& Q) : x(Q.x), y(Q.y), z(Q.z), w(Q.w) {
}
#pragma endregion

#if 1
#pragma region operator

inline quaternion quaternion::operator+() const
{
	return quaternion(w, x, y, z);
}

inline quaternion quaternion::operator-() const
{
	return quaternion(-w, -x, -y, -z);
}

inline quaternion quaternion::operator+(const quaternion& Q) const
{
	return quaternion(w + Q.w, x + Q.x, y + Q.y, z + Q.z);
}

inline quaternion quaternion::operator-(const quaternion& Q) const
{
	return quaternion(w - Q.w, x - Q.x, y - Q.y, z - Q.z);
}

inline quaternion quaternion::operator*(const quaternion& Q) const {

	quaternion R;
	//R.w = w * Q.w - x * Q.x - y * Q.y - z * Q.z;
	//R.x = x * Q.w + w * Q.x - z * Q.y + y * Q.z;
	//R.y = y * Q.w + z * Q.x + w * Q.y - x * Q.z;
	//R.z = z * Q.w - y * Q.x + x * Q.y + w * Q.z;

	R.w = Q.w * w - Q.x * x - Q.y * y - Q.z * z;
	R.x = Q.x * w + Q.w * x - Q.z * y + Q.y * z;
	R.y = Q.y * w + Q.z * x + Q.w * y - Q.x * z;
	R.z = Q.z * w - Q.y * x + Q.x * y + Q.w * z;

	return R;
}
inline quaternion quaternion::operator*(float S) const {
	quaternion R;
	R.w = w * S;
	R.x = x * S;
	R.y = y * S;
	R.z = z * S;

	return R;
}
inline quaternion Adollib::operator*(float S, const quaternion& Q) {
	quaternion R = Q;
	R.w *= S;
	R.x *= S;
	R.y *= S;
	R.z *= S;

	return R;
}
inline quaternion quaternion::operator/(float S) const {
	quaternion R;
	R.w = w / S;
	R.x = x / S;
	R.y = y / S;
	R.z = z / S;

	return R;
}


inline quaternion& quaternion::operator+=(const quaternion& Q)
{
	(*this) = (*this) + Q;
	return *this;
}

inline quaternion& quaternion::operator-=(const quaternion& Q)
{
	(*this) = (*this) - Q;
	return *this;
}
inline quaternion& quaternion::operator*=(const quaternion& Q) {
	(*this) = (*this) * Q;
	return *this;
}
inline quaternion& quaternion::operator*=(float S) {
	(*this) = (*this) * S;
	return *this;
}
inline quaternion& quaternion::operator/=(float S) {
	(*this) = (*this) / S;
	return *this;
}
inline bool quaternion::operator==(quaternion& Q) {
	return (
		x == Q.x &&
		y == Q.y &&
		z == Q.z
		);
}
inline bool quaternion::operator!=(quaternion& Q) {
	return (
		x != Q.x ||
		y != Q.y ||
		z != Q.z
		);
}

#pragma endregion

inline float quaternion::norm_sqr() const {
	float V = norm();
	if (V == 1.0f)	return 1.0f;
	else return sqrtf(V);
}
inline float quaternion::norm() const {
	return w * w + x * x + y * y + z * z;
}
inline quaternion quaternion::unit_vect() const {
	float L = norm_sqr();
	if (L == 0)return quaternion(1, 0, 0, 0);
	else return quaternion((*this) / L);
}
inline quaternion quaternion::conjugate() const {
	return quaternion(w, -x, -y, -z);
}
inline quaternion quaternion::inverse() const {
	return conjugate() / norm_sqr();
}

inline vector3 quaternion::euler() const {
	return matrix_to_euler(get_rotate_matrix());
}

inline vector3 quaternion::euler_radian() const {
	return matrix_to_euler_radian(get_rotate_matrix());
}


inline DirectX::XMFLOAT3 quaternion::get_XM3() const {
	return DirectX::XMFLOAT3(x, y, z);
}
inline DirectX::XMFLOAT4 quaternion::get_XM4() const {
	return DirectX::XMFLOAT4(x, y, z, w);
}
inline vector3 quaternion::get_NV3() const {
	return vector3(x, y, z);
}
inline matrix quaternion::get_rotate_matrix() const {
	matrix R;
	R._11 = 1 - 2 * y * y - 2 * z * z; 	R._12 = 2 * x * y + 2 * z * w;     R._13 = 2 * z * x - 2 * w * y;
	R._21 = 2 * x * y - 2 * z * w;		R._22 = 1 - 2 * z * z - 2 * x * x; R._23 = 2 * y * z + 2 * x * w;
	R._31 = 2 * z * x + 2 * w * y;      R._32 = 2 * y * z - 2 * w * x;     R._33 = 1 - 2 * x * x - 2 * y * y;
	R._44 = 1;
	return R;
}

inline quaternion Adollib::quaternion_identity() {
	return quaternion(1, 0, 0, 0);
}

inline vector3 Adollib::vector3_be_rotated_by_quaternion(const vector3& V, const quaternion& Q) {
#if 0
	vector3 R;
	R.x = 2.0f * (Q.x * V.x + Q.y * V.y + Q.z * V.z) * Q.x + (Q.w * Q.w - (Q.x * Q.x + Q.y * Q.y + Q.z * Q.z)) * V.x + 2.0f * Q.w * (Q.y * V.z - Q.z * V.y);
	R.y = 2.0f * (Q.x * V.x + Q.y * V.y + Q.z * V.z) * Q.y + (Q.w * Q.w - (Q.x * Q.x + Q.y * Q.y + Q.z * Q.z)) * V.y + 2.0f * Q.w * (Q.z * V.x - Q.x * V.z);
	R.z = 2.0f * (Q.x * V.x + Q.y * V.y + Q.z * V.z) * Q.z + (Q.w * Q.w - (Q.x * Q.x + Q.y * Q.y + Q.z * Q.z)) * V.z + 2.0f * Q.w * (Q.x * V.y - Q.y * V.x);
	return R;
#else
	quaternion R(V);
	return (Q.conjugate() * R * Q).get_NV3();
#endif
}

inline quaternion Adollib::quaternion_angle_axis(float S, const vector3& axis) {
	return quaternion_radian_axis(ToRadian(S), axis);
}
inline quaternion Adollib::quaternion_radian_axis(float S, const vector3& axis) {
	if (S == 0)return quaternion(1, 0, 0, 0);
	if (axis.norm() == 0)return quaternion(1, 0, 0, 0);
	vector3 A = axis.unit_vect();

	quaternion R;
	R.w = cosf(S / 2.0f);
	R.x = A.x * sinf(S / 2.0f);
	R.y = A.y * sinf(S / 2.0f);
	R.z = A.z * sinf(S / 2.0f);
	return R;
}

inline quaternion Adollib::quaternion_slerp(const quaternion& Q1, const quaternion& Q2, float R) {
	vector3 g = vector3_cross(Q1.get_NV3(), Q2.get_NV3());
	float radian = quaternion_radian(Q1, Q2) / 2.0f;

	quaternion F;
	F.w = cosf(radian * R);
	F.x = g.x * sinf(radian * R);
	F.y = g.y * sinf(radian * R);
	F.z = g.z * sinf(radian * R);

	return quaternion(vector3_be_rotated_by_quaternion(Q1.get_NV3(), F));
}
inline quaternion Adollib::quaternion_from_euler(float x, float y, float z) {
	quaternion Rx = quaternion_angle_axis(x, vector3(1, 0, 0));
	quaternion Ry = quaternion_angle_axis(y, vector3(0, 1, 0));
	quaternion Rz = quaternion_angle_axis(z, vector3(0, 0, 1));

	quaternion A = Rz * Rx * Ry;
	return  A.unit_vect();
}
inline quaternion Adollib::quaternion_from_euler(vector3 V) {
	quaternion Rx = quaternion_angle_axis(V.x, vector3(1, 0, 0));
	quaternion Ry = quaternion_angle_axis(V.y, vector3(0, 1, 0));
	quaternion Rz = quaternion_angle_axis(V.z, vector3(0, 0, 1));

	return (Rz * Rx * Ry).unit_vect();
}
//quaternion Adollib::quaternion_from_euler_by_local_axis(vector3 Q, float x, float y, float z) {
//	quaternion Rx = quaternion_angle_axis(x, Q);
//	quaternion Ry = quaternion_angle_axis(y, Q);
//	quaternion Rz = quaternion_angle_axis(z, Q);
//
//	return Rz * Rx * Ry;
//}
//quaternion Adollib::quaternion_from_euler_by_local_axis(vector3 Q, vector3 V) {
//	quaternion Rx = quaternion_angle_axis(V.x, vector3(1, 0, 0));
//	quaternion Ry = quaternion_angle_axis(V.y, vector3(0, 1, 0));
//	quaternion Rz = quaternion_angle_axis(V.z, vector3(0, 0, 1));
//
//	return Rz * Rx * Ry;
//}
inline quaternion Adollib::quaternion_by_rotate_matrix(matrix& M) {
	vector3 V = matrix_to_euler(M);
	quaternion Rx = quaternion_angle_axis(V.x, vector3(1, 0, 0));
	quaternion Ry = quaternion_angle_axis(V.y, vector3(0, 1, 0));
	quaternion Rz = quaternion_angle_axis(V.z, vector3(0, 0, 1));

	return Rz * Rx * Ry;
}
inline quaternion Adollib::quaternion_look_at(vector3& me, vector3& you) {
	vector3 Q = you - me;
	return quaternion_from_to_rotate(vector3(0, 0, 1), Q);
}


inline float Adollib::quaternion_dot(const quaternion& Q1, const quaternion& Q2) {
	return Q1.x * Q2.x + Q1.y * Q2.y + Q1.z * Q2.z;
}

inline quaternion Adollib::quaternion_from_to_rotate(const vector3& V1, const vector3& V2) {
	vector3 g = vector3_cross(V1.unit_vect(), V2.unit_vect()).unit_vect();
	float radian = quaternion_radian(V1.unit_vect(), V2.unit_vect());

	quaternion F;
	F.w = cosf(radian / 2);
	F.x = g.x * sinf(radian / 2);
	F.y = g.y * sinf(radian / 2);
	F.z = g.z * sinf(radian / 2);

	return F;
}

inline float Adollib::quaternion_angle(const quaternion& Q1, const quaternion& Q2) {
	return ToAngle(acosf(quaternion_dot(Q1.unit_vect(), Q2.unit_vect())));
}
inline float Adollib::quaternion_radian(const quaternion& Q1, const quaternion& Q2) {
	return acosf(quaternion_dot(Q1.unit_vect(), Q2.unit_vect()));
}

#else 
#pragma region operator

inline quaternion quaternion::operator+() const
{
	return quaternion(w, x, y, z);
}

inline quaternion quaternion::operator-() const
{
	return quaternion(-w, -x, -y, -z);
}

inline quaternion quaternion::operator+(const quaternion& Q) const
{
	return quaternion(w + Q.w, x + Q.x, y + Q.y, z + Q.z);
}

inline quaternion quaternion::operator-(const quaternion& Q) const
{
	return quaternion(w - Q.w, x - Q.x, y - Q.y, z - Q.z);
}

inline quaternion quaternion::operator*(const quaternion& Q) const {

	quaternion R;
	__m128 Qsim1 = _mm_set_ps(x, y, z, w);
	__m128 Qsim2 = _mm_set_ps(Q.x, Q.y, Q.z, Q.w);
	__declspec(align(16)) float Value[4][4];

	for (int i = 0; i < 4; i += 2) {
		_mm_store_ps(Value[i], _mm_mul_ps(Qsim1, Qsim2));

		Qsim2 = _mm_shuffle_ps(Qsim2, Qsim2, _MM_SHUFFLE(0, 1, 2, 3));

		_mm_store_ps(Value[i + 1], _mm_mul_ps(Qsim1, Qsim2));

		Qsim2 = _mm_shuffle_ps(Qsim2, Qsim2, _MM_SHUFFLE(2, 3, 0, 1));
	}

	R.w = Value[0][0] - Value[0][3] - Value[0][2] - Value[0][1];
	R.x = Value[1][0] + Value[1][3] - Value[1][2] + Value[1][1];
	R.y = Value[2][0] + Value[2][3] + Value[2][2] - Value[2][1];
	R.z = Value[3][0] - Value[3][3] + Value[3][2] + Value[3][1];

	return R;
}
inline quaternion quaternion::operator*(float S) const {
	quaternion R;
	R.w = w * S;
	R.x = x * S;
	R.y = y * S;
	R.z = z * S;

	return R;
}
inline quaternion Adollib::operator*(float S, const quaternion& Q) {
	quaternion R = Q;
	R.w *= S;
	R.x *= S;
	R.y *= S;
	R.z *= S;

	return R;
}
inline quaternion quaternion::operator/(float S) const {
	quaternion R;
	float inv_S = 1.0f / S;
	R.w = w * inv_S;
	R.x = x * inv_S;
	R.y = y * inv_S;
	R.z = z * inv_S;

	return R;
}


inline quaternion& quaternion::operator+=(const quaternion& Q) {
	(*this) = (*this) + Q;
	return *this;
}

inline quaternion& quaternion::operator-=(const quaternion& Q) {
	(*this) = (*this) - Q;
	return *this;
}
inline quaternion& quaternion::operator*=(const quaternion& Q) {
	(*this) = (*this) * Q;
	return *this;
}
inline quaternion& quaternion::operator*=(float S) {
	(*this) = (*this) * S;
	return *this;
}
inline quaternion& quaternion::operator/=(float S) {
	(*this) = (*this) / S;
	return *this;
}
inline bool quaternion::operator==(quaternion& Q) {
	return (
		x == Q.x &&
		y == Q.y &&
		z == Q.z
		);
}
inline bool quaternion::operator!=(quaternion& Q) {
	return (
		x != Q.x ||
		y != Q.y ||
		z != Q.z
		);
}

#pragma endregion

inline float quaternion::norm_sqr() const {
	float V = norm();
	if (V == 1.0f)	return 1.0f;
	else return sqrtf(V);
}
inline float quaternion::norm() const {
	return w * w + x * x + y * y + z * z;
}
inline quaternion quaternion::unit_vect() const {
	float L = norm_sqr();
	if (L == 0)return quaternion(1, 0, 0, 0);
	else return quaternion((*this) / L);
}
inline quaternion quaternion::conjugate() const {
	return quaternion(w, -x, -y, -z);
}
inline quaternion quaternion::inverse() const {
	return conjugate() / norm_sqr();
}

inline vector3 quaternion::euler() const {
	return matrix_to_euler(get_rotate_matrix());
}

inline vector3 quaternion::euler_radian() const {
	return matrix_to_euler_radian(get_rotate_matrix());
}


inline DirectX::XMFLOAT3 quaternion::get_XM3() const {
	return DirectX::XMFLOAT3(x, y, z);
}
inline DirectX::XMFLOAT4 quaternion::get_XM4() const {
	return DirectX::XMFLOAT4(x, y, z, w);
}
inline vector3 quaternion::get_NV3() const {
	return vector3(x, y, z);
}
inline matrix quaternion::get_rotate_matrix() const {
	matrix R;
	__m128 Qsim = _mm_set_ps(x, y, z, w);
	__m128 Qsim_ = _mm_set_ps(x, y, z, w);
	__declspec(align(16)) float Value[4][4];

	for (int i = 0; i < 4; i++) {
		_mm_store_ps(Value[i], _mm_mul_ps(Qsim, Qsim_));
		Qsim_ = _mm_shuffle_ps(Qsim_, Qsim_, _MM_SHUFFLE(2, 1, 0, 3));
	}

	R._11 = 1 - 2 * (Value[0][2] + Value[0][1]); R._12 = 2 * (Value[1][3] + Value[1][1]);     R._13 = 2 * (Value[2][1] - Value[2][0]);
	R._21 = 2 * (Value[1][3] - Value[1][1]);	 R._22 = 1 - 2 * (Value[0][1] + Value[0][3]); R._23 = 2 * (Value[1][2] + Value[1][0]);
	R._31 = 2 * (Value[2][1] + Value[2][0]);     R._32 = 2 * (Value[1][2] - Value[1][0]);     R._33 = 1 - 2 * (Value[0][3] + Value[0][2]);
	R._44 = 1;

	return R;
}

inline quaternion Adollib::quaternion_identity() {
	return quaternion(1, 0, 0, 0);
}

inline vector3 Adollib::vector3_be_rotated_by_quaternion(const vector3& V, const quaternion& Q) {
#if 0
	vector3 R;
	R.x = 2.0f * (Q.x * V.x + Q.y * V.y + Q.z * V.z) * Q.x + (Q.w * Q.w - (Q.x * Q.x + Q.y * Q.y + Q.z * Q.z)) * V.x + 2.0f * Q.w * (Q.y * V.z - Q.z * V.y);
	R.y = 2.0f * (Q.x * V.x + Q.y * V.y + Q.z * V.z) * Q.y + (Q.w * Q.w - (Q.x * Q.x + Q.y * Q.y + Q.z * Q.z)) * V.y + 2.0f * Q.w * (Q.z * V.x - Q.x * V.z);
	R.z = 2.0f * (Q.x * V.x + Q.y * V.y + Q.z * V.z) * Q.z + (Q.w * Q.w - (Q.x * Q.x + Q.y * Q.y + Q.z * Q.z)) * V.z + 2.0f * Q.w * (Q.x * V.y - Q.y * V.x);
	return R;
#else
	quaternion R(V);
	return (Q.conjugate() * R * Q).get_NV3();
#endif
}

inline quaternion Adollib::quaternion_angle_axis(float S, const vector3& axis) {
	return quaternion_radian_axis(ToRadian(S), axis);
}
inline quaternion Adollib::quaternion_radian_axis(float S, const vector3& axis) {
	if (S == 0)return quaternion(1, 0, 0, 0);
	if (axis.norm() == 0)return quaternion(1, 0, 0, 0);
	vector3 A = axis.unit_vect();

	__m128 sim = _mm_set_ps(axis.x, axis.y, axis.z, 0);
	float Value[4];
	_mm_store_ps(Value, _mm_mul_ps(sim, _mm_sin_ps(_mm_set1_ps(S * 0.5f))));

	quaternion R;
	R.w = cosf(S * 0.5f);
	R.x = Value[3];
	R.y = Value[2];
	R.z = Value[1];
	return R;
}

inline quaternion Adollib::quaternion_slerp(const quaternion& Q1, const quaternion& Q2, float R) {
	vector3 g = vector3_cross(Q1.get_NV3(), Q2.get_NV3());
	float radian = quaternion_radian(Q1, Q2) * 0.5f;

	__m128 sim = _mm_set_ps(g.x, g.y, g.z, 0);
	__declspec(align(16))float Value[4];
	_mm_store_ps(Value, _mm_mul_ps(sim, _mm_sin_ps(_mm_set1_ps(radian * R))));

	quaternion F;
	F.w = cosf(radian * R);
	F.x = Value[3];
	F.y = Value[2];
	F.z = Value[1];

	return quaternion(vector3_be_rotated_by_quaternion(Q1.get_NV3(), F));
}
inline quaternion Adollib::quaternion_from_euler(float x, float y, float z) {
	quaternion Rx = quaternion_angle_axis(x, vector3(1, 0, 0));
	quaternion Ry = quaternion_angle_axis(y, vector3(0, 1, 0));
	quaternion Rz = quaternion_angle_axis(z, vector3(0, 0, 1));

	quaternion A = Rz * Rx * Ry;
	return  A.unit_vect();
}
inline quaternion Adollib::quaternion_from_euler(vector3 V) {
	quaternion Rx = quaternion_angle_axis(V.x, vector3(1, 0, 0));
	quaternion Ry = quaternion_angle_axis(V.y, vector3(0, 1, 0));
	quaternion Rz = quaternion_angle_axis(V.z, vector3(0, 0, 1));

	return (Rz * Rx * Ry).unit_vect();
}
//quaternion Adollib::quaternion_from_euler_by_local_axis(vector3 Q, float x, float y, float z) {
//	quaternion Rx = quaternion_angle_axis(x, Q);
//	quaternion Ry = quaternion_angle_axis(y, Q);
//	quaternion Rz = quaternion_angle_axis(z, Q);
//
//	return Rz * Rx * Ry;
//}
//quaternion Adollib::quaternion_from_euler_by_local_axis(vector3 Q, vector3 V) {
//	quaternion Rx = quaternion_angle_axis(V.x, vector3(1, 0, 0));
//	quaternion Ry = quaternion_angle_axis(V.y, vector3(0, 1, 0));
//	quaternion Rz = quaternion_angle_axis(V.z, vector3(0, 0, 1));
//
//	return Rz * Rx * Ry;
//}
inline quaternion Adollib::quaternion_by_rotate_matrix(matrix& M) {
	vector3 V = matrix_to_euler(M);
	quaternion Rx = quaternion_angle_axis(V.x, vector3(1, 0, 0));
	quaternion Ry = quaternion_angle_axis(V.y, vector3(0, 1, 0));
	quaternion Rz = quaternion_angle_axis(V.z, vector3(0, 0, 1));

	return Rz * Rx * Ry;
}
inline quaternion Adollib::quaternion_look_at(vector3& me, vector3& you) {
	vector3 Q = you - me;
	return quaternion_from_to_rotate(vector3(0, 0, 1), Q);
}


inline float Adollib::quaternion_dot(const quaternion& Q1, const quaternion& Q2) {
	return Q1.x * Q2.x + Q1.y * Q2.y + Q1.z * Q2.z;
}

inline quaternion Adollib::quaternion_from_to_rotate(const vector3& V1, const vector3& V2) {
	vector3 g = vector3_cross(V1.unit_vect(), V2.unit_vect()).unit_vect();
	float radian = quaternion_radian(V1.unit_vect(), V2.unit_vect());

	__m128 sim = _mm_set_ps(g.x, g.y, g.z, 0);
	__declspec(align(16))float Value[4];
	_mm_store_ps(Value, _mm_mul_ps(sim, _mm_sin_ps(_mm_set1_ps(radian * 0.5f))));

	quaternion F;
	F.w = cosf(radian * 0.5f);
	F.x = Value[3];
	F.y = Value[2];
	F.z = Value[1];

	return F;
}

inline float Adollib::quaternion_angle(const quaternion& Q1, const quaternion& Q2) {
	return ToAngle(acosf(quaternion_dot(Q1.unit_vect(), Q2.unit_vect())));
}
inline float Adollib::quaternion_radian(const quaternion& Q1, const quaternion& Q2) {
	return acosf(quaternion_dot(Q1.unit_vect(), Q2.unit_vect()));
}
#endif