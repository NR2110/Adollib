#pragma once

#include <DirectXMath.h>

namespace Adollib {

	class Vector3;

	class Matrix {
	public:

		union
		{
			struct {
				float
					_11, _12, _13, _14,
					_21, _22, _23, _24,
					_31, _32, _33, _34,
					_41, _42, _43, _44;
			};

			float m[4][4];

		};

		Matrix() :
			_11(0), _12(0), _13(0), _14(0),
			_21(0), _22(0), _23(0), _24(0),
			_31(0), _32(0), _33(0), _34(0),
			_41(0), _42(0), _43(0), _44(0)
		{
		}

		Matrix(
			float _11, float _12, float _13,
			float _21, float _22, float _23,
			float _31, float _32, float _33
		) :
			_11(_11), _12(_12), _13(_13), _14(0),
			_21(_21), _22(_22), _23(_23), _24(0),
			_31(_31), _32(_32), _33(_33), _34(0),
			_41(0), _42(0), _43(0), _44(1)
		{
		}

		Matrix(
			float _11, float _12, float _13, float _14,
			float _21, float _22, float _23, float _24,
			float _31, float _32, float _33, float _34,
			float _41, float _42, float _43, float _44
		) :
			_11(_11), _12(_12), _13(_13), _14(_14),
			_21(_21), _22(_22), _23(_23), _24(_24),
			_31(_31), _32(_32), _33(_33), _34(_34),
			_41(_41), _42(_42), _43(_43), _44(_44)
		{
		}

		Matrix operator+ () const;
		Matrix operator- () const;

		Matrix operator+ (const Matrix& M) const;
		Matrix operator- (const Matrix& M) const;
		Matrix operator* (const Matrix& M) const;
		Matrix operator* (float S) const;
		Matrix operator/ (float S) const;

		Matrix& operator+= (const Matrix& M);
		Matrix& operator-= (const Matrix& M);
		Matrix& operator*= (const Matrix& M);
		Matrix& operator*= (float S);
		Matrix& operator/= (float S);
		bool operator== (const Matrix& M);
		bool operator!= (const Matrix& M);

		friend Matrix operator*(float S, const Matrix& M);

		DirectX::XMMATRIX get_XMMATRIX();
		DirectX::XMFLOAT4X4 get_XMFLOAT4X4();

	private:

	};

	//単位行列を返す
	Matrix matrix_identity();

	//転置行列を返す
	Matrix matrix_trans(const Matrix& M);

	//行列式を返す
	float matrix_determinant(const Matrix& M);

	//行列の逆元を返す
	Matrix matrix_inverse(const Matrix& M);

	//回転行列をオイラーに(z,x,y)
	Vector3 matrix_to_euler(const Matrix& M);
	//回転行列をオイラー(radian)に(z,x,y)
	Vector3 matrix_to_euler_radian(const Matrix& M);

	//vector3に回転行列をかける
	Vector3 vector3_trans(const Vector3& V, const Matrix& M);

	//world行列の生成
	Matrix matrix_world(const Vector3& scale, const Matrix& rotate, const Vector3& trans);

	//matrixからXMMATRIXへの変換
	DirectX::XMMATRIX matrix_to_XMMATRIX(const Matrix& M);

	//XMMATRIXからmatrixへの変換
	Matrix XMMATRIX_to_matrix(const DirectX::XMMATRIX& M);

	Matrix matrix_cross(const Vector3& vec);

	Matrix matrix_scale(const Vector3& vec);



}

#include "matrix_e.h"