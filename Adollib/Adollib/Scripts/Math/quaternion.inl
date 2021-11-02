#pragma once

#include "vector.h"
#include "matrix.h"
namespace Adollib {

#pragma region constract

	inline Quaternion::Quaternion() noexcept :XMFLOAT4(0, 0, 0, 1) {
	}
	inline Quaternion::Quaternion(float x, float y, float z) noexcept :XMFLOAT4(x, y, z, 0) {
	}
	inline Quaternion::Quaternion(DirectX::XMFLOAT3 Q)  noexcept :XMFLOAT4(Q.x, Q.y, Q.z, 0) {
	}
	inline Quaternion::Quaternion(Vector3 Q)  noexcept :XMFLOAT4(Q.x, Q.y, Q.z, 0) {
	}
	//quaternion::quaternion(VECTOR4 Q) : x(Q.x), y(Q.y), z(Q.z), w(Q.w) {
	//}
	inline Quaternion::Quaternion(float w, float x, float y, float z)noexcept :XMFLOAT4(x, y, z, w) {
	}
	inline Quaternion::Quaternion(const Quaternion& Q) noexcept :XMFLOAT4(Q.x, Q.y, Q.z, Q.w) {
	}
#pragma endregion


#pragma region operator

	inline Quaternion Quaternion::operator+() const
	{
		return Quaternion(w, x, y, z);
	}

	inline Quaternion Quaternion::operator-() const
	{
		return Quaternion(-w, -x, -y, -z);
	}

	inline Quaternion Quaternion::operator+(const Quaternion& Q) const
	{
		return Quaternion(w + Q.w, x + Q.x, y + Q.y, z + Q.z);
	}

	inline Quaternion Quaternion::operator-(const Quaternion& Q) const
	{
		return Quaternion(w - Q.w, x - Q.x, y - Q.y, z - Q.z);
	}

	inline Quaternion Quaternion::operator*(const Quaternion& Q) const {

		Quaternion R;
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
	inline Quaternion Quaternion::operator*(float S) const {
		Quaternion R;
		R.w = w * S;
		R.x = x * S;
		R.y = y * S;
		R.z = z * S;

		return R;
	}
	inline Quaternion Adollib::operator*(float S, const Quaternion& Q) {
		Quaternion R = Q;
		R.w *= S;
		R.x *= S;
		R.y *= S;
		R.z *= S;

		return R;
	}
	inline Quaternion Quaternion::operator/(float S) const {
		Quaternion R;
		R.w = w / S;
		R.x = x / S;
		R.y = y / S;
		R.z = z / S;

		return R;
	}


	inline Quaternion& Quaternion::operator+=(const Quaternion& Q)
	{
		(*this) = (*this) + Q;
		return *this;
	}

	inline Quaternion& Quaternion::operator-=(const Quaternion& Q)
	{
		(*this) = (*this) - Q;
		return *this;
	}
	inline Quaternion& Quaternion::operator*=(const Quaternion& Q) {
		(*this) = (*this) * Q;
		return *this;
	}
	inline Quaternion& Quaternion::operator*=(float S) {
		(*this) = (*this) * S;
		return *this;
	}
	inline Quaternion& Quaternion::operator/=(float S) {
		(*this) = (*this) / S;
		return *this;
	}
	inline bool Quaternion::operator==(Quaternion& Q) {
		return (
			x == Q.x &&
			y == Q.y &&
			z == Q.z
			);
	}
	inline bool Quaternion::operator!=(Quaternion& Q) {
		return (
			x != Q.x ||
			y != Q.y ||
			z != Q.z
			);
	}

#pragma endregion

	inline float Quaternion::norm_sqr() const {
		float V = norm();
		if (V == 1.0f)	return 1.0f;
		else return sqrtf(V);
	}
	inline float Quaternion::norm() const {
		//return w * w + x * x + y * y + z * z;
		return ((*this) * conjugate()).w;
	}
	inline Quaternion Quaternion::unit_vect() const {
		float L = norm_sqr();
		if (L == 0)return Quaternion(1, 0, 0, 0);
		else return Quaternion((*this) / L);
	}
	inline Quaternion Quaternion::inverse() const {
		return conjugate() / norm();
	}
	inline Quaternion Quaternion::conjugate() const {
		return Quaternion(w, -x, -y, -z);
	}

	inline Vector3 Quaternion::euler() const {
		return matrix_to_euler(get_rotate_matrix());
	}

	inline Vector3 Quaternion::euler_radian() const {
		return matrix_to_euler_radian(get_rotate_matrix());
	}

	inline Vector3 Quaternion::axis() const {
		return Vector3(x, y, z).unit_vect();
	}

	inline float Quaternion::radian() const {
		if (w >= 1) return 0;
		else if (w <= -1) return DirectX::XM_2PI;
		return acosf(w) * 2;
	}

	inline float Quaternion::angle() const {
		return ToAngle(radian());
	}


	inline DirectX::XMFLOAT3 Quaternion::get_XM3() const {
		return DirectX::XMFLOAT3(x, y, z);
	}
	inline DirectX::XMFLOAT4 Quaternion::get_XM4() const {
		return DirectX::XMFLOAT4(x, y, z, w);
	}
	inline Vector3 Quaternion::get_NV3() const {
		return Vector3(x, y, z);
	}
	inline Matrix33 Quaternion::get_rotate_matrix() const {
		Matrix33 R;
		R._11 = 1 - 2 * y * y - 2 * z * z; 	R._12 = 2 * x * y + 2 * z * w;     R._13 = 2 * z * x - 2 * w * y;
		R._21 = 2 * x * y - 2 * z * w;		R._22 = 1 - 2 * z * z - 2 * x * x; R._23 = 2 * y * z + 2 * x * w;
		R._31 = 2 * z * x + 2 * w * y;      R._32 = 2 * y * z - 2 * w * x;     R._33 = 1 - 2 * x * x - 2 * y * y;
		return R;
	}

	inline Quaternion Adollib::quaternion_identity() {
		return Quaternion(1, 0, 0, 0);
	}

	inline Vector3 Adollib::vector3_quatrotate(const Vector3& V, const Quaternion& Q) {
#if 0
		vector3 R;
		R.x = 2.0f * (Q.x * V.x + Q.y * V.y + Q.z * V.z) * Q.x + (Q.w * Q.w - (Q.x * Q.x + Q.y * Q.y + Q.z * Q.z)) * V.x + 2.0f * Q.w * (Q.y * V.z - Q.z * V.y);
		R.y = 2.0f * (Q.x * V.x + Q.y * V.y + Q.z * V.z) * Q.y + (Q.w * Q.w - (Q.x * Q.x + Q.y * Q.y + Q.z * Q.z)) * V.y + 2.0f * Q.w * (Q.z * V.x - Q.x * V.z);
		R.z = 2.0f * (Q.x * V.x + Q.y * V.y + Q.z * V.z) * Q.z + (Q.w * Q.w - (Q.x * Q.x + Q.y * Q.y + Q.z * Q.z)) * V.z + 2.0f * Q.w * (Q.x * V.y - Q.y * V.x);
		return R;
#else
		Quaternion R(V);
		return (Q.inverse() * R * Q).get_NV3();
#endif
	}

	inline Quaternion Adollib::quaternion_axis_angle(const Vector3& axis, float S) {
		return quaternion_axis_radian(axis, ToRadian(S));
	}
	inline Quaternion Adollib::quaternion_axis_radian(const Vector3& axis, float S) {
		if (S == 0)return Quaternion(1, 0, 0, 0);
		if (axis.norm() == 0)return Quaternion(1, 0, 0, 0);
		Vector3 A = axis.unit_vect();

		Quaternion R;
		R.w = cosf(S * 0.5f);
		R.x = A.x * sinf(S * 0.5f);
		R.y = A.y * sinf(S * 0.5f);
		R.z = A.z * sinf(S * 0.5f);
		return R;
	}

	inline Quaternion Adollib::quaternion_slerp(const Quaternion& Q1, const Quaternion& Q2, float R) {
		float dotproduct = Q1.x * Q2.x + Q1.y * Q2.y + Q1.z * Q2.z + Q1.w * Q2.w;
		float theta, st, sut, sout, coeff1, coeff2;

		// algorithm adapted from Shoemake's paper
		R = R * 0.5f;
		dotproduct = ALmax(1, ALmin(-1, dotproduct));
		theta = (float)acos(dotproduct);
		if (theta == 0)theta = FLT_EPSILON;

		st = (float)sinf(theta);
		sut = (float)sinf(R * theta);
		sout = (float)sinf((1 - R) * theta);
		coeff1 = sout / st;
		coeff2 = sut / st;

		Quaternion ret;
		ret.x = coeff1 * Q1.x + coeff2 * Q2.x;
		ret.y = coeff1 * Q1.y + coeff2 * Q2.y;
		ret.z = coeff1 * Q1.z + coeff2 * Q2.z;
		ret.w = coeff1 * Q1.w + coeff2 * Q2.w;

		return ret.unit_vect();
	}
	inline Quaternion Adollib::quaternion_from_euler(float x, float y, float z) {
		Quaternion Rx = quaternion_axis_angle(Vector3(1, 0, 0), x);
		Quaternion Ry = quaternion_axis_angle(Vector3(0, 1, 0), y);
		Quaternion Rz = quaternion_axis_angle(Vector3(0, 0, 1), z);

		Quaternion A = Rz * Rx * Ry;
		return  A.unit_vect();
	}
	inline Quaternion Adollib::quaternion_from_euler(Vector3 V) {
		Quaternion Rx = quaternion_axis_angle(Vector3(1, 0, 0), V.x);
		Quaternion Ry = quaternion_axis_angle(Vector3(0, 1, 0), V.y);
		Quaternion Rz = quaternion_axis_angle(Vector3(0, 0, 1), V.z);

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
	inline Quaternion Adollib::quaternion_by_rotate_matrix(Matrix44& M) {
		Vector3 V = matrix_to_euler(M);
		Quaternion Rx = quaternion_axis_angle(Vector3(1, 0, 0), V.x);
		Quaternion Ry = quaternion_axis_angle(Vector3(0, 1, 0), V.y);
		Quaternion Rz = quaternion_axis_angle(Vector3(0, 0, 1), V.z);

		return Rz * Rx * Ry;
	}
	inline Quaternion Adollib::quaternion_look_at(Vector3& me, Vector3& you) {
		Vector3 Q = you - me;
		return quaternion_from_to_rotate(Vector3(0, 0, 1), Q);
	}


	inline float Adollib::quaternion_dot(const Quaternion& Q1, const Quaternion& Q2) {
		return Q1.x * Q2.x + Q1.y * Q2.y + Q1.z * Q2.z;
	}

	inline Quaternion Adollib::quaternion_from_to_rotate(const Vector3& V1, const Vector3& V2) {
		if (V1 == +V2)return +quaternion_identity();
		if (V1 == -V2) {
			Vector3 axis = Vector3(0, 1, 0);
			if (fabsf(V1.y) == 1)axis = Vector3(1, 0, 0);

			const Quaternion buff = quaternion_axis_angle(axis, 90);
			return buff * quaternion_from_to_rotate(vector3_quatrotate(V1, buff), V2);
		}

		Vector3 g = vector3_cross(V1.unit_vect(), V2.unit_vect()).unit_vect();
		float radian = vector3_radian(V1.unit_vect(), V2.unit_vect());

		Quaternion F;
		F.w = cosf(radian / 2);
		F.x = g.x * sinf(radian / 2);
		F.y = g.y * sinf(radian / 2);
		F.z = g.z * sinf(radian / 2);

		return F;
	}

	inline float Adollib::quaternion_angle(const Quaternion& Q1, const Quaternion& Q2) {
		return ToAngle(acosf(quaternion_dot(Q1.unit_vect(), Q2.unit_vect())));
	}
	inline float Adollib::quaternion_radian(const Quaternion& Q1, const Quaternion& Q2) {
		return acosf(quaternion_dot(Q1.unit_vect(), Q2.unit_vect()));
	}

}