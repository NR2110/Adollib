#pragma once

#include <DirectXMath.h>

namespace Adollib {

	class Vector3;

	class matrix {
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
		bool operator== (const matrix& M);
		bool operator!= (const matrix& M);

		friend matrix operator*(float S, const matrix& M);

		DirectX::XMMATRIX get_XMMATRIX();
		DirectX::XMFLOAT4X4 get_XMFLOAT4X4();

	private:

	};

	//�P�ʍs���Ԃ�
	matrix matrix_identity();

	//�]�u�s���Ԃ�
	matrix matrix_trans(const matrix& M);

	//�s�񎮂�Ԃ�
	float matrix_determinant(const matrix& M);

	//�s��̋t����Ԃ�
	matrix matrix_inverse(const matrix& M);

	//��]�s����I�C���[��(z,x,y)
	Vector3 matrix_to_euler(const matrix& M);
	//��]�s����I�C���[(radian)��(z,x,y)
	Vector3 matrix_to_euler_radian(const matrix& M);

	//vector3�ɉ�]�s���������
	Vector3 vector3_trans(const Vector3& V, const matrix& M);

	//world�s��̐���
	matrix matrix_world(const Vector3& scale, const matrix& rotate, const Vector3& trans);

	//matrix����XMMATRIX�ւ̕ϊ�
	DirectX::XMMATRIX matrix_to_XMMATRIX(const matrix& M);

	//XMMATRIX����matrix�ւ̕ϊ�
	matrix XMMATRIX_to_matrix(const DirectX::XMMATRIX& M);



}

#include "matrix_e.h"