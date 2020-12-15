#pragma once

#include <DirectXMath.h>
#include <immintrin.h>
#include <math.h>
#include "math_e.h"

namespace Adollib {

	class Quaternion;

	//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
#pragma region vector2
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
	class vector2 {
	private:
		// 非constの計算したら変更があるまでsqrtを使わなくて済む

	public:
		float x;
		float y;

		vector2 operator+ () const;
		vector2 operator- () const;

		vector2 operator+ (const vector2& M) const;
		vector2 operator- (const vector2& M) const;
		vector2 operator* (const vector2& M) const;
		vector2 operator* (float S) const;
		vector2 operator/ (float S) const;

		vector2& operator+= (const vector2& M);
		vector2& operator-= (const vector2& M);
		vector2& operator*= (float S);
		vector2& operator/= (float S);

		friend vector2 operator*(float S, const vector2& M);


		// norm^2
		float norm_sqr() const;

		// norm
		float norm() const;

		// unit_vect
		vector2 unit_vect() const;


	};
	// 2点間の距離
	float vector2_distance_sqr(const vector2&, const vector2&);

	// 2点間の距離^2
	float vector2_distance(const vector2&, const vector2&);

	// 2ベクトルの内積
	float vector2_dot(const vector2&, const vector2&);

	// 2ベクトルの間の角
	float vector2_angle(const vector2&, const vector2&);

	// V1からV2へのベクトル
	vector2 vector2_look_at(const vector2& V1, const vector2& V2);
#pragma endregion
	//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

	//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
#pragma region vector3
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
	class Vector3 : public DirectX::XMFLOAT3 {

	private:
	public:
		//Vector3() :x(0), y(0), z(0) {};
		//Vector3(float x, float y, float z) :x(x), y(y), z(z) {};
		//Vector3(vector2 xy, float z) :x(xy.x), y(xy.y), z(z) {};
		//explicit Vector3(float v) :x(v), y(v), z(v) {};
		//Vector3(DirectX::XMFLOAT3 xm) :x(xm.x), y(xm.y), z(xm.z) {};
		constexpr Vector3() noexcept :XMFLOAT3(0, 0, 0) {};
		constexpr Vector3(float x, float y, float z) noexcept :XMFLOAT3(x, y, z) {};
		constexpr Vector3(vector2 xy, float z) noexcept :XMFLOAT3(xy.x, xy.y, z) {};
		constexpr explicit Vector3(float v) noexcept :XMFLOAT3(v, v, v) {};
		constexpr Vector3(DirectX::XMFLOAT3 xm) noexcept :XMFLOAT3(xm.x, xm.y, xm.z) {};

		//vector3(n_vector2 V) :x(V.x), y(V.y), z(0) {};

		Vector3 operator+ () const;
		Vector3 operator- () const;

		Vector3 operator+ (const Vector3& M) const;
		Vector3 operator- (const Vector3& M) const;
		Vector3 operator* (const Vector3& M) const;
		Vector3 operator/ (const Vector3& M) const;
		Vector3 operator* (float S) const;
		Vector3 operator/ (float S) const;
		float& operator[] (const int i);
		float operator[] (const int i) const;

		Vector3& operator+= (const Vector3& M);
		Vector3& operator-= (const Vector3& M);
		Vector3& operator*= (const Vector3& M);
		Vector3& operator/= (const Vector3& M);
		Vector3& operator*= (float S);
		Vector3& operator/= (float S);

		bool operator== (const Vector3& M) const;
		bool operator!= (const Vector3& M) const;

		friend Vector3 operator*(float S, const Vector3& M);

		//XMFLOAT3の変換
		DirectX::XMFLOAT3 get_XM3() const;


		// norm^2
		float norm_sqr() const;

		// norm
		float norm() const;

		// unit_vect
		Vector3 unit_vect() const;

	};

	// 2点間の距離
	float vector3_distance_sqr(const Vector3&, const Vector3&);

	// 2点間の距離^2
	float vector3_distance(const Vector3&, const Vector3&);

	// 2ベクトルの内積
	float vector3_dot(const Vector3&, const Vector3&);

	// 2ベクトルの外積
	Vector3 vector3_cross(const Vector3&, const Vector3&);

	// 2ベクトルの間の角
	float vector3_angle(const Vector3&, const Vector3&);
	// 2ベクトルの間の角
	float vector3_radian(const Vector3&, const Vector3&);

	// V1からV2へのベクトル
	Vector3 vector3_look_at(const Vector3& V1, const Vector3& V2);

	//Q1からQ2への球面補間
	//R = 0 = V1
	//R = 1 = V2
	Vector3 vector3_slerp(const Vector3& V1, const Vector3& V2, float R);

#pragma endregion
	//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

	//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
#pragma region vector4
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
	class Vector4 : public DirectX::XMFLOAT4 {

	private:

	public:
		Vector4() noexcept :XMFLOAT4(0, 0, 0, 0) {};
		Vector4(Vector3 xyz, float w)noexcept :XMFLOAT4(xyz.x, xyz.y, xyz.z, w) {};
		Vector4(float x, float y, float z, float w)noexcept :XMFLOAT4(x, y, z, w) {};
		//vector3(n_vector2 V) :x(V.x), y(V.y), z(0) {};

		Vector4 operator+ () const;
		Vector4 operator- () const;

		Vector4 operator+ (const Vector4& M) const;
		Vector4 operator- (const Vector4& M) const;
		Vector4 operator* (const Vector4& M) const;
		Vector4 operator* (float S) const;
		Vector4 operator/ (float S) const;
		float& operator[] (const int i);
		float operator[] (const int i) const;

		Vector4& operator+= (const Vector4& M);
		Vector4& operator-= (const Vector4& M);
		Vector4& operator*= (float S);
		Vector4& operator/= (float S);

		bool operator== (const Vector4& M);
		bool operator!= (const Vector4& M);

		friend Vector4 operator*(float S, const Vector4& M);

		//XMFLOAT3の変換
		DirectX::XMFLOAT4 get_XM4();


		// norm^2
		float norm_sqr() const;

		// norm
		float norm() const;

		// unit_vect
		Vector4 unit_vect() const;
		Vector4 unit_vect();

	};

#pragma endregion
	//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::


}


#if _DEBUG || !USE_SIMD
#include "vector_e.h"
#else
#include "vectorSIM_e.h"
#endif