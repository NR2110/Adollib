#pragma once

#include <DirectXMath.h>
#include "vector.h"

namespace Adollib {
	class matrix {
	public:

		float
			_11, _12, _13, _14,
			_21, _22, _23, _24,
			_31, _32, _33, _34,
			_41, _42, _43, _44
			;

		matrix() :
			_11(0), _12(0), _13(0), _14(0),
			_21(0), _22(0), _23(0), _24(0),
			_31(0), _32(0), _33(0), _34(0),
			_41(0), _42(0), _43(0), _44(0)
		{
		}

		matrix(
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

		matrix(
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

		matrix operator+ () const;
		matrix operator- () const;

		matrix operator+ (const matrix& M) const;
		matrix operator- (const matrix& M) const;
		matrix operator* (const matrix& M) const;
		matrix operator* (float S) const;
		matrix operator/ (float S) const;

		matrix& operator+= (const matrix& M);
		matrix& operator-= (const matrix& M);
		matrix& operator*= (const matrix& M);
		matrix& operator*= (float S);
		matrix& operator/= (float S);	

		friend matrix operator*(float S, const matrix& M);

		DirectX::XMMATRIX get_XMMATRIX();
		DirectX::XMFLOAT4X4 get_XMFLOAT4X4();

	private:
		/*
		//float* matrix_to_array(const matrix& M) const {
		//	float R[4][4] = { 0 };
		//	R[0][0] = M._11;	R[0][1] = M._12;	R[0][2] = M._13;	 R[0][3] = M._14;
		//	R[1][0] = M._21;	R[1][1] = M._22;	R[1][2] = M._23;	 R[1][3] = M._24;
		//	R[2][0] = M._31;	R[2][1] = M._32;	R[2][2] = M._33;	 R[2][3] = M._34;
		//	R[3][0] = M._41;	R[3][1] = M._42;	R[3][2] = M._43;	 R[3][3] = M._44;

		//	return &R[0][0];
		//};

		//matrix array_to_matrix(const float S[4][4]) const {
		//	matrix R;
		//	R._11 = S[0][0];	R._12 = S[0][1];	R._13 = S[0][2];	R._14 = S[0][3];
		//	R._21 = S[1][0];	R._22 = S[1][1];	R._23 = S[1][2];	R._24 = S[1][3];
		//	R._31 = S[2][0];	R._32 = S[2][1];	R._33 = S[2][2];	R._34 = S[2][3];
		//	R._41 = S[3][0];	R._42 = S[3][1];	R._43 = S[3][2];	R._44 = S[3][3];

		//	return R;
		//};
		//*/

	};

	//単位行列を返す
	matrix matrix_identity();

	//転置行列を返す
	matrix matrix_trans(const matrix& M);

	//行列式を返す
	float matrix_determinant(const matrix& M);

	//行列の逆元を返す
	matrix matrix_inverse(const matrix& M);

	//回転行列をオイラーに(z,x,y)
	vector3 matrix_to_euler(const matrix& M);
	//回転行列をオイラー(radian)に(z,x,y)
	vector3 matrix_to_euler_radian(const matrix& M);

	//vector3に回転行列をかける
	vector3 vector3_trans(const vector3& V, const matrix& M);

	//world行列の生成
	matrix matrix_world(const vector3& scale, const matrix& rotate, const vector3& trans);

	//matrixからXMMATRIXへの変換
	DirectX::XMMATRIX matrix_to_XMMATRIX(const matrix& M);

	//XMMATRIXからmatrixへの変換
	matrix XMMATRIX_to_matrix(const DirectX::XMMATRIX& M);



}

//typedef Adollib::matrix matrix;