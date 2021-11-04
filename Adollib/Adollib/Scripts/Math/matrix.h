#pragma once

#include <DirectXMath.h>
#include "math_e.h"

namespace Adollib {

	class Vector3;

	class Matrix33;
	class Matrix44;

	class Matrix44 :public DirectX::XMFLOAT4X4 {
	public:

		Matrix44() noexcept :
			XMFLOAT4X4(
				0, 0, 0, 0,
				0, 0, 0, 0,
				0, 0, 0, 0,
				0, 0, 0, 0
			) {}

		Matrix44(
			float _11, float _12, float _13,
			float _21, float _22, float _23,
			float _31, float _32, float _33
		)  noexcept :
			XMFLOAT4X4(
				_11, _12, _13, 0,
				_21, _22, _23, 0,
				_31, _32, _33, 0,
				0, 0, 0, 1
			) {}

		Matrix44(
			float _11, float _12, float _13, float _14,
			float _21, float _22, float _23, float _24,
			float _31, float _32, float _33, float _34,
			float _41, float _42, float _43, float _44
		)  noexcept :
			XMFLOAT4X4(
				_11, _12, _13, _14,
				_21, _22, _23, _24,
				_31, _32, _33, _34,
				_41, _42, _43, _44
			){}

		Matrix44(
			XMFLOAT4X4 mat
		)  noexcept :
			XMFLOAT4X4(
				mat._11, mat._12, mat._13, mat._14,
				mat._21, mat._22, mat._23, mat._24,
				mat._31, mat._32, mat._33, mat._34,
				mat._41, mat._42, mat._43, mat._44
			) {}

		Matrix44 operator+ () const;
		Matrix44 operator- () const;

		Matrix44 operator+ (const Matrix44& M) const;
		Matrix44 operator- (const Matrix44& M) const;
		Matrix44 operator* (const Matrix44& M) const;
		Matrix44 operator* (float S) const;
		Matrix44 operator/ (float S) const;

		Matrix44& operator= (const Matrix33& M);
		Matrix44& operator+= (const Matrix44& M);
		Matrix44& operator-= (const Matrix44& M);
		Matrix44& operator*= (const Matrix44& M);
		Matrix44& operator*= (float S);
		Matrix44& operator/= (float S);
		bool operator== (const Matrix44& M) const;
		bool operator!= (const Matrix44& M) const;

		friend Matrix44 operator*(float S, const Matrix44& M);

		DirectX::XMMATRIX get_XMMATRIX4X4();

	private:

	};

	class Matrix33 :public DirectX::XMFLOAT3X3 {
	public:

		Matrix33() noexcept :
			XMFLOAT3X3(
				0, 0, 0,
				0, 0, 0,
				0, 0, 0
			)
		{
		}

		Matrix33(
			float _11, float _12, float _13,
			float _21, float _22, float _23,
			float _31, float _32, float _33
		)  noexcept :
			XMFLOAT3X3(
				_11, _12, _13,
				_21, _22, _23,
				_31, _32, _33
			)
		{
		}

		Matrix33 operator+ () const;
		Matrix33 operator- () const;

		Matrix33 operator+ (const Matrix33& M) const;
		Matrix33 operator- (const Matrix33& M) const;
		Matrix33 operator* (const Matrix33& M) const;
		Matrix33 operator* (float S) const;
		Matrix33 operator/ (float S) const;

		Matrix33& operator+= (const Matrix33& M);
		Matrix33& operator-= (const Matrix33& M);
		Matrix33& operator*= (const Matrix33& M);
		Matrix33& operator*= (float S);
		Matrix33& operator/= (float S);
		bool operator== (const Matrix33& M) const;
		bool operator!= (const Matrix33& M) const;

		friend Matrix33 operator*(float S, const Matrix33& M);

		DirectX::XMMATRIX get_XMMATRIX();

	private:

	};

	//単位行列を返す
	Matrix44 matrix44_identity();
	//単位行列を返す
	Matrix33 matrix33_identity();

	//全部zeroの行列を返す
	Matrix44 matrix44_zero();
	//全部zeroの行列を返す
	Matrix33 matrix33_zero();

	//転置行列を返す
	Matrix44 matrix_trans(const Matrix44& M);
	//転置行列を返す
	Matrix33 matrix_trans(const Matrix33& M);

	//ベクトルを外積行列に治す
	Matrix44 matrix44_cross(const Vector3& vec);
	//ベクトルを外積行列に治す
	Matrix33 matrix33_cross(const Vector3& vec);

	//Scaleの行列に治す
	Matrix33 matrix33_scale(const Vector3& vec);
	//Scaleの行列に治す
	Matrix44 matrix44_scale(const Vector3& vec);

	//XMMATRIXからmatrixへの変換
	Matrix44 XMMATRIX_to_matrix44(const DirectX::XMMATRIX& M);
	//XMMATRIXからmatrixへの変換
	Matrix33 XMMATRIX_to_matrix33(const DirectX::XMMATRIX& M);


	//行列式を返す
	float matrix_determinant(const Matrix44& M);
	//行列式を返す
	float matrix_determinant(const Matrix33& M);

	//行列の逆元を返す
	Matrix44 matrix_inverse(const Matrix44& M);
	//行列の逆元を返す
	Matrix33 matrix_inverse(const Matrix33& M);

	//回転行列をオイラーに(z,x,y)
	Vector3 matrix_to_euler(const Matrix44& M);
	//回転行列をオイラー(radian)に(z,x,y)
	Vector3 matrix_to_euler_radian(const Matrix44& M);
	//回転行列をオイラーに(z,x,y)
	Vector3 matrix_to_euler(const Matrix33& M);
	//回転行列をオイラー(radian)に(z,x,y)
	Vector3 matrix_to_euler_radian(const Matrix33& M);

	//vector3に回転行列をかける
	Vector3 vector3_trans(const Vector3& V, const Matrix44& M);
	//vector3に回転行列をかける
	Vector3 vector3_trans(const Vector3& V, const Matrix33& M);

	//world行列の生成
	Matrix44 matrix_world(const Vector3& scale, const Matrix44& rotate, const Vector3& trans);
	//world行列の生成
	Matrix44 matrix_world(const Vector3& scale, const Matrix33& rotate, const Vector3& trans);

	//matrixからXMMATRIXへの変換
	DirectX::XMMATRIX matrix_to_XMMATRIX(const Matrix44& M);
	//matrixからXMMATRIXへの変換
	DirectX::XMMATRIX matrix_to_XMMATRIX(const Matrix33& M);

	//Boxの慣性テンソルを返す
	Matrix33 make_box_tensor(Vector3 size, float mass);

}

#if _DEBUG || !USE_SIMD
#include "matrix4x4.inl"
#include "matrix3x3.inl"
#else
#include "matrixSIM_e.h"
#endif
