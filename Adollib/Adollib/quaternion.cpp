
#include "quaternion.h"
#include "math.h"

#include <math.h>

using namespace Adollib;


#pragma region constract

quaternion::quaternion() : x(0), y(0), z(0), w(0) {
}
quaternion::quaternion(float x, float y, float z) : x(x), y(y), z(z), w(0) {
}
quaternion::quaternion(DirectX::XMFLOAT3 Q) : x(Q.x), y(Q.y), z(Q.z), w(0) {
}
quaternion::quaternion(vector3 Q) : x(Q.x), y(Q.y), z(Q.z), w(0) {
}
//quaternion::quaternion(VECTOR4 Q) : x(Q.x), y(Q.y), z(Q.z), w(Q.w) {
//}
quaternion::quaternion(float w, float x, float y, float z) : x(x), y(y), z(z), w(w) {
}
quaternion::quaternion(const quaternion& Q) : x(Q.x), y(Q.y), z(Q.z), w(Q.w) {
}
#pragma endregion

#pragma region operator

quaternion quaternion::operator+() const
{
	return quaternion(w, x, y, z);
}

quaternion quaternion::operator-() const
{
	return quaternion(-w, -x, -y, -z);
}

quaternion quaternion::operator+(const quaternion& Q) const
{
	return quaternion(w + Q.w, x + Q.x, y + Q.y, z + Q.z);
}

quaternion quaternion::operator-(const quaternion& Q) const
{
	return quaternion(w - Q.w, x - Q.x, y - Q.y, z - Q.z);
}

quaternion quaternion::operator*(const quaternion& Q) const {

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
quaternion quaternion::operator*(float S) const {
	quaternion R;
	R.w = w * S;
	R.x = x * S;
	R.y = y * S;
	R.z = z * S;

	return R;
}
quaternion Adollib::operator*(float S, const quaternion& Q) {
	quaternion R = Q;
	R.w *= S;
	R.x *= S;
	R.y *= S;
	R.z *= S;

	return R;
}
quaternion quaternion::operator/(float S) const {
	quaternion R;
	R.w = w / S;
	R.x = x / S;
	R.y = y / S;
	R.z = z / S;

	return R;
}


quaternion& quaternion::operator+=(const quaternion& Q)
{
	(*this) = (*this) + Q;
	return *this;
}

quaternion& quaternion::operator-=(const quaternion& Q)
{
	(*this) = (*this) - Q;
	return *this;
}
quaternion& quaternion::operator*=(const quaternion& Q) {
	(*this) = (*this) * Q;
	return *this;
}
quaternion& quaternion::operator*=(float S) {
	(*this) = (*this) * S;
	return *this;
}
quaternion& quaternion::operator/=(float S) {
	(*this) = (*this) / S;
	return *this;
}

#pragma endregion

float quaternion::norm_sqr() const {
	float V = norm();
	if (V == 1.0f)	return 1.0f;
	else return sqrtf(V);
}
float quaternion::norm() const {
	return w * w + x * x + y * y + z * z;
}
quaternion quaternion::unit_vect() const {
	float L = norm_sqr();
	if (L == 0)return quaternion(1, 0, 0, 0);
	else return quaternion((*this) / L);
}
quaternion quaternion::conjugate() const {
	return quaternion(w, -x, -y, -z);
}
quaternion quaternion::inverse() const {
	return conjugate() / norm_sqr();
}

vector3 quaternion::euler() const {
	return matrix_to_euler(get_rotate_matrix());
}

vector3 quaternion::euler_radian() const {
	return matrix_to_euler_radian(get_rotate_matrix());
}


DirectX::XMFLOAT3 quaternion::get_XM3() const {
	return DirectX::XMFLOAT3(x, y, z);
}
DirectX::XMFLOAT4 quaternion::get_XM4() const {
	return DirectX::XMFLOAT4(x, y, z, w);
}
vector3 quaternion::get_NV3() const {
	return vector3(x, y, z);
}
matrix quaternion::get_rotate_matrix() const {
	matrix R;
	R._11 = 1 - 2 * y * y - 2 * z * z; 	R._12 = 2 * x * y + 2 * z * w;     R._13 = 2 * z * x - 2 * w * y;
	R._21 = 2 * x * y - 2 * z * w;		R._22 = 1 - 2 * z * z - 2 * x * x; R._23 = 2 * y * z + 2 * x * w;
	R._31 = 2 * z * x + 2 * w * y;      R._32 = 2 * y * z - 2 * w * x;     R._33 = 1 - 2 * x * x - 2 * y * y;
	R._44 = 1;
	return R;
}

quaternion Adollib::quaternion_identity() {
	return quaternion(1, 0, 0, 0);
}

vector3 Adollib::vector3_be_rotated_by_quaternion(const vector3& V, const quaternion& Q) {
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

quaternion Adollib::quaternion_angle_axis(float S, const vector3& axis) {
	return quaternion_radian_axis(ToRadian(S), axis);
}
quaternion Adollib::quaternion_radian_axis(float S, const vector3& axis) {
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

quaternion Adollib::quaternion_slerp(const quaternion& Q1, const quaternion& Q2, float R) {
	vector3 g = vector3_cross(Q1.get_NV3(), Q2.get_NV3());
	float radian = quaternion_radian(Q1, Q2) / 2.0f;

	quaternion F;
	F.w = cosf(radian * R);
	F.x = g.x * sinf(radian * R);
	F.y = g.y * sinf(radian * R);
	F.z = g.z * sinf(radian * R);

	return quaternion(vector3_be_rotated_by_quaternion(Q1.get_NV3(), F));
}
quaternion Adollib::quaternion_from_euler(float x, float y, float z) {
	quaternion Rx = quaternion_angle_axis(x, vector3(1, 0, 0));
	quaternion Ry = quaternion_angle_axis(y, vector3(0, 1, 0));
	quaternion Rz = quaternion_angle_axis(z, vector3(0, 0, 1));

	quaternion A = Rz * Rx * Ry;
	return  A.unit_vect(); 
}
quaternion Adollib::quaternion_from_euler(vector3 V) {
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
quaternion Adollib::quaternion_by_rotate_matrix(matrix& M) {
	vector3 V = matrix_to_euler(M);
	quaternion Rx = quaternion_angle_axis(V.x, vector3(1, 0, 0));
	quaternion Ry = quaternion_angle_axis(V.y, vector3(0, 1, 0));
	quaternion Rz = quaternion_angle_axis(V.z, vector3(0, 0, 1));

	return Rz * Rx * Ry;
}
quaternion Adollib::quaternion_look_at(vector3& me, vector3& you) {
	vector3 Q = you - me;
	return quaternion_from_to_rotate(vector3(0, 0, 1), Q);
}


float Adollib::quaternion_dot(const quaternion& Q1, const quaternion& Q2) {
	return Q1.x * Q2.x + Q1.y * Q2.y + Q1.z * Q2.z;
}

quaternion Adollib::quaternion_from_to_rotate(const vector3& V1, const vector3& V2) {
	vector3 g = vector3_cross(V1.unit_vect(), V2.unit_vect()).unit_vect();
	float radian = quaternion_radian(V1.unit_vect(), V2.unit_vect());

	quaternion F;
	F.w = cosf(radian / 2);
	F.x = g.x * sinf(radian / 2);
	F.y = g.y * sinf(radian / 2);
	F.z = g.z * sinf(radian / 2);

	return F;
}

float Adollib::quaternion_angle(const quaternion& Q1, const quaternion& Q2) {
	return ToAngle(acosf(quaternion_dot(Q1.unit_vect(), Q2.unit_vect())));
}
float Adollib::quaternion_radian(const quaternion& Q1, const quaternion& Q2) {
	return acosf(quaternion_dot(Q1.unit_vect(), Q2.unit_vect()));
}