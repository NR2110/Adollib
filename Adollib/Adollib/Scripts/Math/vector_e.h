#pragma once

#include "quaternion.h"
#include <math.h>

namespace Adollib {

	//::::::::::::::::::::::::::::::::::::::::::::::::
#pragma region vector3
//::::::::::::::::::::::::::::::::::::::::::::::::

//operator
	inline Vector3 Vector3::operator+ () const {
		return *this;
	}
	inline Vector3 Vector3::operator- () const {
		Vector3 R = *this;
		R.x *= -1;
		R.y *= -1;
		R.z *= -1;
		return R;
	}

	inline Vector3 Vector3::operator+ (const Vector3& M) const {
		Vector3 R;
		R.x = x + M.x;
		R.y = y + M.y;
		R.z = z + M.z;
		return R;
	}
	inline Vector3 Vector3::operator- (const Vector3& M) const {
		Vector3 R;
		R.x = x - M.x;
		R.y = y - M.y;
		R.z = z - M.z;
		return R;
	}
	inline Vector3 Vector3::operator* (const Vector3& M) const {
		Vector3 R;
		R.x = x * M.x;
		R.y = y * M.y;
		R.z = z * M.z;
		return R;
	}
	inline Vector3 Vector3::operator/ (const Vector3& M) const {
		Vector3 R;
		R.x = x / M.x;
		R.y = y / M.y;
		R.z = z / M.z;
		return R;
	}
	inline Vector3 Vector3::operator* (float S) const {
		Vector3 R;
		R.x = x * S;
		R.y = y * S;
		R.z = z * S;
		return R;
	}
	inline Vector3 Adollib::operator*(float S, const Vector3& M) {
		Vector3 R;
		R.x = M.x * S;
		R.y = M.y * S;
		R.z = M.z * S;
		return R;
	}

	inline Vector3 Vector3::operator/ (float S) const {
		Vector3 R;
		float inv_S = 1.0f / S;
		R.x = x * inv_S;
		R.y = y * inv_S;
		R.z = z * inv_S;
		return R;
	}
	inline float& Vector3::operator[] (const int i) {
		switch (i)
		{
		case 0: return x;
			break;
		case 1: return y;
			break;
		case 2: return z;
			break;
		}
		return x;
	}
	inline float Vector3::operator[] (const int i) const {
		switch (i)
		{
		case 0: return x;
			break;
		case 1: return y;
			break;
		case 2: return z;
			break;
		}
		return 0;
	}


	inline Vector3& Vector3::operator+= (const Vector3& M) {
		x = x + M.x;
		y = y + M.y;
		z = z + M.z;
		return *this;
	}
	inline Vector3& Vector3::operator-= (const Vector3& M) {
		x = x - M.x;
		y = y - M.y;
		z = z - M.z;
		return *this;
	}
	inline Vector3& Vector3::operator*= (const Vector3& M) {
		x = x * M.x;
		y = y * M.y;
		z = z * M.z;
		return *this;
	}
	inline Vector3& Vector3::operator/= (const Vector3& M) {
		x = x / M.x;
		y = y / M.y;
		z = z / M.z;
		return *this;
	}
	inline Vector3& Vector3::operator*= (float S) {
		x = x * S;
		y = y * S;
		z = z * S;
		return *this;
	}
	inline Vector3& Vector3::operator/= (float S) {
		x = x / S;
		y = y / S;
		z = z / S;
		return *this;
	}

	inline bool Vector3::operator== (const Vector3& M) const {
		if (x == M.x && y == M.y && z == M.z)return true;
		return false;
	}
	inline bool Vector3::operator!= (const Vector3& M) const {
		if (x == M.x && y == M.y && z == M.z)return false;
		return true;
	}

	inline DirectX::XMFLOAT3 Vector3::get_XM3() const {
		DirectX::XMFLOAT3 R;
		R.x = x;
		R.y = y;
		R.z = z;
		return R;
	}
	//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

	inline float Vector3::norm_sqr() const {
		float S = norm();
		if (S == 1.0f) return 1.0f;
		else return sqrtf(S);
	}

	inline float Vector3::norm() const {
		return x * x + y * y + z * z;
	}

	inline Vector3 Vector3::unit_vect() const {
		float V = norm_sqr();
		if (V == 1.0f)return *this;
		if (V == 0)return Vector3(0, 0, 0);
		Vector3 R = *this;
		return R / V;
	}

	inline float Adollib::vector3_distance_sqr(const Vector3& P, const Vector3& Q) {
		return
			sqrtf((P.x - Q.x) * (P.x - Q.x) + (P.y - Q.y) * (P.y - Q.y) + (P.z - Q.z) * (P.z - Q.z));
	}

	inline float Adollib::vector3_distance(const Vector3& P, const Vector3& Q) {
		return (P.x - Q.x) * (P.x - Q.x) + (P.y - Q.y) * (P.y - Q.y) + (P.z - Q.z) * (P.z - Q.z);
	}


	inline float Adollib::vector3_dot(const Vector3& P, const Vector3& Q) {
		return P.x * Q.x + P.y * Q.y + P.z * Q.z;
	}


	inline Vector3 Adollib::vector3_cross(const Vector3& P, const Vector3& Q) {
		Vector3 R;
		R.x = P.y * Q.z - P.z * Q.y;
		R.y = P.z * Q.x - P.x * Q.z;
		R.z = P.x * Q.y - P.y * Q.x;

		if (isnan(R.x)) {
			float A = P.y * Q.z;
			float B = P.z * Q.y;
			if (isinf(A)) (A = A > 0 ? FLT_MAX : -FLT_MAX);
			if (isinf(B)) (B = B > 0 ? FLT_MAX : -FLT_MAX);

			R.x = A - B;
		}
		if (isnan(R.y)) {
			float A = P.z * Q.x;
			float B = P.x * Q.z;
			if (isinf(A)) (A = A > 0 ? FLT_MAX : -FLT_MAX);
			if (isinf(B)) (B = B > 0 ? FLT_MAX : -FLT_MAX);

			R.y = A - B;
		}
		if (isnan(R.z)) {
			float A = P.x * Q.y;
			float B = P.y * Q.x;
			if (isinf(A)) (A = A > 0 ? FLT_MAX : -FLT_MAX);
			if (isinf(B)) (B = B > 0 ? FLT_MAX : -FLT_MAX);

			R.z = A - B;
		}
		if (isinf(R.x)) R.x = R.x > 0 ? R.x = FLT_MAX : -FLT_MAX;
		if (isinf(R.y)) R.y = R.y > 0 ? R.y = FLT_MAX : -FLT_MAX;
		if (isinf(R.z)) R.z = R.z > 0 ? R.z = FLT_MAX : -FLT_MAX;

		return R;
	}


	inline float Adollib::vector3_angle(const Vector3& P, const Vector3& Q) {

		return ToAngle(acosf(Adollib::vector3_dot(P.unit_vect(), Q.unit_vect())));
	}
	inline float Adollib::vector3_radian(const Vector3& P, const Vector3& Q) {

		float buf = ALClamp(Adollib::vector3_dot(P.unit_vect(), Q.unit_vect()), -1, 1);
		return acosf(buf);
	}

	inline Vector3 Adollib::vector3_look_at(const Vector3& P, const Vector3& Q) {
		return Q - P;
	}

	inline Vector3 Adollib::vector3_slerp(const Vector3& V1, const Vector3& V2, float R) {
		Vector3 g = vector3_cross(V1, V2);
		float radian = vector3_radian(V1, V2) * 0.5f;

		Quaternion F;
		F.w = cosf(radian * R);
		F.x = g.x * sinf(radian * R);
		F.y = g.y * sinf(radian * R);
		F.z = g.z * sinf(radian * R);

		return vector3_quatrotate(V1, F);
	}

	inline Vector3 vector3_zero() {
		return Vector3(0);
	};
#pragma endregion
	//::::::::::::::::::::::::::::::::::::::::::::::::

	//::::::::::::::::::::::::::::::::::::::::::::::::
#pragma region vector4
//::::::::::::::::::::::::::::::::::::::::::::::::

//operator
	inline Vector4 Vector4::operator+ () const {
		return *this;
	}
	inline Vector4 Vector4::operator- () const {
		Vector4 R = *this;
		R.x *= -1;
		R.y *= -1;
		R.z *= -1;
		R.w *= -1;
		return R;
	}

	inline Vector4 Vector4::operator+ (const Vector4& M) const {
		Vector4 R;
		R.x = x + M.x;
		R.y = y + M.y;
		R.z = z + M.z;
		R.w = w + M.w;
		return R;
	}
	inline Vector4 Vector4::operator- (const Vector4& M) const {
		Vector4 R;
		R.x = x - M.x;
		R.y = y - M.y;
		R.z = z - M.z;
		R.w = w - M.z;
		return R;
	}
	inline Vector4 Vector4::operator* (const Vector4& M) const {
		Vector4 R;
		R.x = x * M.x;
		R.y = y * M.y;
		R.z = z * M.z;
		R.w = w * M.w;
		return R;
	}
	inline Vector4 Vector4::operator* (float S) const {
		Vector4 R;
		R.x = x * S;
		R.y = y * S;
		R.z = z * S;
		R.w = w * S;
		return R;
	}
	inline Vector4 Adollib::operator*(float S, const Vector4& M) {
		Vector4 R;
		R.x = M.x * S;
		R.y = M.y * S;
		R.z = M.z * S;
		R.w = M.w * S;
		return R;
	}

	inline Vector4 Vector4::operator/ (float S) const {
		Vector4 R;
		R.x = x / S;
		R.y = y / S;
		R.z = z / S;
		R.w = w / S;
		return R;
	}
	inline float& Vector4::operator[] (const int i) {
		switch (i)
		{
		case 0: return x;
			break;
		case 1: return y;
			break;
		case 2: return z;
			break;
		case 3: return w;
			break;
		}
		float R = 0;
		return R;
	}
	inline float Vector4::operator[] (const int i) const {
		switch (i)
		{
		case 0: return x;
			break;
		case 1: return y;
			break;
		case 2: return z;
			break;
		case 3: return w;
			break;
		}
		return 0;
	}


	inline Vector4& Vector4::operator+= (const Vector4& M) {
		x = x + M.x;
		y = y + M.y;
		z = z + M.z;
		w = w + M.w;
		return *this;
	}
	inline Vector4& Vector4::operator-= (const Vector4& M) {
		x = x - M.x;
		y = y - M.y;
		z = z - M.z;
		w = w - M.w;
		return *this;
	}
	inline Vector4& Vector4::operator*= (float S) {
		x = x * S;
		y = y * S;
		z = z * S;
		w = w * S;
		return *this;
	}
	inline Vector4& Vector4::operator/= (float S) {
		x = x / S;
		y = y / S;
		z = z / S;
		w = w / S;
		return *this;
	}

	inline bool Vector4::operator== (const Vector4& M) {
		if (x == M.x && y == M.y && z == M.z && w == M.w)return true;
		return false;
	}
	inline bool Vector4::operator!= (const Vector4& M) {
		if (x == M.x && y == M.y && z == M.z && w == M.w)return false;
		return true;
	}

	inline DirectX::XMFLOAT4 Vector4::get_XM4() {
		DirectX::XMFLOAT4 R;
		R.x = x;
		R.y = y;
		R.z = z;
		R.w = w;
		return R;
	}
	//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::


	inline float Vector4::norm_sqr() const {
		float S = norm();
		if (S == 1.0f) return 1.0f;
		else return sqrtf(S);
	}

	inline float Vector4::norm() const {
		return x * x + y * y + z * z + w * w;
	}

	inline Vector4 Vector4::unit_vect() {
		float V = norm_sqr();
		if (V == 1.0f)return *this;
		if (V < FLT_EPSILON)return Vector4(0, 0, 0, 0);
		Vector4 R = *this;
		return R / V;
	}
	inline Vector4 Vector4::unit_vect() const {
		float V = norm_sqr();
		Vector4 R = *this;
		return R / V;
	}


#pragma endregion
	//::::::::::::::::::::::::::::::::::::::::::::::::

	//::::::::::::::::::::::::::::::::::::::::::::::::
#pragma region vector2
//::::::::::::::::::::::::::::::::::::::::::::::::
//operator
	inline Vector2 Vector2::operator+ () const {
		return *this;
	}
	inline Vector2 Vector2::operator- () const {
		Vector2 R = *this;
		R.x *= -1;
		R.y *= -1;
		return R;
	}

	inline Vector2 Vector2::operator+ (const Vector2& M) const {
		Vector2 R;
		R.x = x + M.x;
		R.y = y + M.y;
		return R;
	}
	inline Vector2 Vector2::operator- (const Vector2& M) const {
		Vector2 R;
		R.x = x - M.x;
		R.y = y - M.y;
		return R;
	}
	inline Vector2 Vector2::operator* (const Vector2& M) const {
		Vector2 R;
		R.x = x * M.x;
		R.y = y * M.y;
		return R;
	}
	inline Vector2 Vector2::operator* (float S) const {
		Vector2 R;
		R.x = x * S;
		R.y = y * S;
		return R;
	}
	inline Vector2 Adollib::operator*(float S, const Vector2& M) {
		Vector2 R;
		R.x = M.x * S;
		R.y = M.y * S;
		return R;
	}

	inline Vector2 Vector2::operator/ (float S) const {
		Vector2 R;
		R.x = x / S;
		R.y = y / S;
		return R;
	}

	inline Vector2& Vector2::operator+= (const Vector2& M) {
		x = x + M.x;
		y = y + M.y;
		return *this;
	}
	inline Vector2& Vector2::operator-= (const Vector2& M) {
		x = x - M.x;
		y = y - M.y;
		return *this;
	}
	inline Vector2& Vector2::operator*= (float S) {
		x = x * S;
		y = y * S;
		return *this;
	}
	inline Vector2& Vector2::operator/= (float S) {
		x = x / S;
		y = y / S;
		return *this;
	}

	//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

	inline float Vector2::norm_sqr() const {
		float S = norm();
		if (S == 1.0f) return 1.0f;
		else return sqrtf(S);
	}

	inline float Vector2::norm() const {
		return x * x + y * y;
	}

	inline Vector2 Vector2::unit_vect() const {
		float V = norm_sqr();
		Vector2 R = *this;
		return R / V;
	}
	inline float Adollib::vector2_distance_sqr(const Vector2& P, const Vector2& Q) {
		return
			sqrtf((P.x - Q.x) * (P.x - Q.x) + (P.y - Q.y) * (P.y - Q.y));
	}

	inline float Adollib::vector2_distance(const Vector2& P, const Vector2& Q) {
		return (P.x - Q.x) * (P.x - Q.x) + (P.y - Q.y) * (P.y - Q.y);
	}


	inline float Adollib::vector2_dot(const Vector2& P, const Vector2& Q) {
		return P.x * Q.x + P.y * Q.y;
	}

	inline float Adollib::vector2_angle(const Vector2& P, const Vector2& Q) {

		return ToAngle(acosf(Adollib::vector2_dot(P.unit_vect(), Q.unit_vect())));
	}

	inline Vector2 Adollib::vector2_look_at(const Vector2& P, const Vector2& Q) {
		return Q - P;
	}


#pragma endregion
	//::::::::::::::::::::::::::::::::::::::::::::::::

}