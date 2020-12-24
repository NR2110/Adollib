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
		__m128 Q1vec = DirectX::XMLoadFloat4(this);
		__m128 Q2vec = DirectX::XMLoadFloat4(&Q);

		Quaternion R;
		DirectX::XMStoreFloat4(&R, DirectX::XMQuaternionMultiply(Q1vec, Q2vec));

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
		return w * w + x * x + y * y + z * z;
	}
	inline Quaternion Quaternion::unit_vect() const {
		float L = norm_sqr();
		if (L == 0)return Quaternion(1, 0, 0, 0);
		else return Quaternion((*this) / L);
	}
	inline Quaternion Quaternion::conjugate() const {
		return Quaternion(w, -x, -y, -z);
	}
	inline Quaternion Quaternion::inverse() const {
		return conjugate() / norm_sqr();
	}

	inline Vector3 Quaternion::euler() const {
		return matrix_to_euler(get_rotate_matrix());
	}

	inline Vector3 Quaternion::euler_radian() const {
		return matrix_to_euler_radian(get_rotate_matrix());
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
	inline Matrix Quaternion::get_rotate_matrix() const {
		__m128 Q = DirectX::XMLoadFloat4(this);

		Matrix R;
		DirectX::XMStoreFloat4x4(&R, DirectX::XMMatrixRotationQuaternion(Q));
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
		return (Q.conjugate() * R * Q).get_NV3();
#endif
	}

	inline Quaternion Adollib::quaternion_angle_axis(float S, const Vector3& axis) {
		return quaternion_radian_axis(ToRadian(S), axis);
	}
	inline Quaternion Adollib::quaternion_radian_axis(float S, const Vector3& axis) {
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
		__m128 Q1vec = DirectX::XMLoadFloat4(&Q1);
		__m128 Q2vec = DirectX::XMLoadFloat4(&Q2);

		Vector4 val;
		DirectX::XMStoreFloat4(&val, _mm_mul_ps(Q1vec, Q2vec));
		float dotproduct = val.x + val.y + val.z + val.w;

		float theta, st, sut, sout, coeff1, coeff2;

		// algorithm adapted from Shoemake's paper
		R = R * 0.5f;
		dotproduct = ALmax(1, ALmin(-1, dotproduct));
		theta = (float)acos(dotproduct);
		if (theta == 0)theta = FLT_EPSILON;

		st = 1 / (float)sinf(theta);
		sut = (float)sinf(R * theta);
		sout = (float)sinf((1 - R) * theta);
		coeff1 = sout * st;
		coeff2 = sut * st;

		Quaternion ret;
		DirectX::XMStoreFloat4(&ret, _mm_add_ps(DirectX::XMVectorScale(Q1vec, coeff1), DirectX::XMVectorScale(Q2vec, coeff2)));

		return ret.unit_vect();
	}
	inline Quaternion Adollib::quaternion_from_euler(float x, float y, float z) {
		Quaternion Rx = quaternion_angle_axis(x, Vector3(1, 0, 0));
		Quaternion Ry = quaternion_angle_axis(y, Vector3(0, 1, 0));
		Quaternion Rz = quaternion_angle_axis(z, Vector3(0, 0, 1));

		Quaternion A = Rz * Rx * Ry;
		return  A.unit_vect();
	}
	inline Quaternion Adollib::quaternion_from_euler(Vector3 V) {
		Quaternion Rx = quaternion_angle_axis(V.x, Vector3(1, 0, 0));
		Quaternion Ry = quaternion_angle_axis(V.y, Vector3(0, 1, 0));
		Quaternion Rz = quaternion_angle_axis(V.z, Vector3(0, 0, 1));

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
	inline Quaternion Adollib::quaternion_by_rotate_matrix(Matrix& M) {
		Vector3 V = matrix_to_euler(M);
		Quaternion Rx = quaternion_angle_axis(V.x, Vector3(1, 0, 0));
		Quaternion Ry = quaternion_angle_axis(V.y, Vector3(0, 1, 0));
		Quaternion Rz = quaternion_angle_axis(V.z, Vector3(0, 0, 1));

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

			const Quaternion buff = quaternion_angle_axis(90, axis);
			return buff * quaternion_from_to_rotate(vector3_quatrotate(V1, buff), V2);
		}

		Vector3 g = vector3_cross(V1.unit_vect(), V2.unit_vect()).unit_vect();
		float radian = quaternion_radian(V1.unit_vect(), V2.unit_vect());

		Quaternion F;
		F.w = cosf(radian * 0.5f);
		F.x = g.x * sinf(radian * 0.5f);
		F.y = g.y * sinf(radian * 0.5f);
		F.z = g.z * sinf(radian * 0.5f);

		return F;
	}

	inline float Adollib::quaternion_angle(const Quaternion& Q1, const Quaternion& Q2) {
		return ToAngle(acosf(quaternion_dot(Q1.unit_vect(), Q2.unit_vect())));
	}
	inline float Adollib::quaternion_radian(const Quaternion& Q1, const Quaternion& Q2) {
		return acosf(quaternion_dot(Q1.unit_vect(), Q2.unit_vect()));
	}

}