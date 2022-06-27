#pragma once

#include <DirectXMath.h>
#include "math_e.h"

namespace Adollib {

	class Vector3;
	class Matrix44;
	class Matrix33;

	class Quaternion : public DirectX::XMFLOAT4 {
		//public:

		//	float x;
		//	float y;
		//	float z;
		//	float w;


	private:

	public:

		Quaternion()noexcept;
		Quaternion(DirectX::XMFLOAT3)noexcept;
		Quaternion(DirectX::XMFLOAT4)noexcept;
		Quaternion(Vector3)noexcept;

		//::::::::::
		// x, y, z
		//::::::::::
		Quaternion(float x, float y, float z)noexcept;
		//::::::::::
		// w, x, y, z
		//::::::::::
		Quaternion(float w, float x, float y, float z)noexcept;

		Quaternion(const Quaternion& Q)noexcept;
		~Quaternion() {}

		Quaternion operator+(const Quaternion&) const;
		Quaternion operator-(const Quaternion&) const;
		Quaternion operator*(float) const;
		Quaternion operator*(const Quaternion&) const;
		Quaternion operator/(float) const;

		Quaternion& operator+=(const Quaternion&);
		Quaternion& operator-=(const Quaternion&);
		Quaternion& operator*=(float);
		Quaternion& operator*=(const Quaternion&);
		Quaternion& operator/=(float);
		bool operator==(Quaternion& Q);
		bool operator!=(Quaternion& Q);

		Quaternion operator+() const;
		Quaternion operator-() const;

		friend Quaternion operator* (float S, const Quaternion& Q);

	public:
		DirectX::XMFLOAT3 get_XM3() const;
		Vector3 get_NV3() const;
		DirectX::XMFLOAT4 get_XM4() const;
		Matrix33 get_rotate_matrix() const;

		// length
		float norm_sqr() const;

		// length^2
		float norm() const;


		// 正規化
		Quaternion unit_vect() const;

		// 共役
		Quaternion inverse() const;

		// 逆元
		Quaternion conjugate() const;

		// オイラー角で返す
		Vector3 euler() const;

		// オイラー角(radian)で返す
		Vector3 euler_radian() const;

		// 回転の軸を返す
		Vector3 axis() const;

		// 回転量を返す
		float radian() const;

		// 回転量を返す
		float angle() const;

	};

	Quaternion quaternion_identity();
	Vector3 vector3_quatrotate(const Vector3& V, const Quaternion& Q);

	//axisを軸にR度回転させるquaternionの作成
	Quaternion quaternion_axis_angle(const Vector3& axis, float R);
	//axisを軸にR度回転させるquaternionの作成
	Quaternion quaternion_axis_radian(const Vector3& axis, float R);

	//Q1からQ2への球面補間
	//R = 0 = Q1
	//R = 1 = Q2
	//動作未確認
	Quaternion quaternion_slerp(const Quaternion& Q1, const Quaternion& Q2, float R);

	//z軸,x軸,y軸の順で回転クォータニオンの生成
	Quaternion quaternion_from_euler(float x, float y, float z);
	//z軸,x軸,y軸の順で回転クォータニオンの生成
	Quaternion quaternion_from_euler(Vector3 V);

	//回転行列からクォータニオンの生成
	Quaternion quaternion_by_rotate_matrix(Matrix44& M);

	Quaternion quaternion_look_at(Vector3& me, Vector3& you);

	//2つのquaternionの内積を返す
	float quaternion_dot(const Quaternion&, const Quaternion&);

	//Q1からQ2への回転quaternionを返す
	Quaternion quaternion_from_to_rotate(const Vector3& V1, const Vector3& V2);

	//2つのquaternionの間の角度を返す
	float quaternion_angle(const Quaternion& Q, const Quaternion& T);
	//2つのquaternionの間の角度を返す
	float quaternion_radian(const Quaternion& Q, const Quaternion& T);
};

#if _DEBUG || !USE_SIMD
#include "quaternion.inl"
#else
#include "quaternionSIM_e.h"
#endif