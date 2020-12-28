#pragma once

#include <DirectXMath.h>
#include "math_e.h"

namespace Adollib {

	class Vector3;

	class Matrix :public DirectX::XMFLOAT4X4{
	public:

		//union
		//{
		//	struct {
		//		float
		//			_11, _12, _13, _14,
		//			_21, _22, _23, _24,
		//			_31, _32, _33, _34,
		//			_41, _42, _43, _44;
		//	};

		//	float m[4][4];

		//};

		Matrix() noexcept :
			XMFLOAT4X4(
				0,0,0,0,
				0,0,0,0,
				0,0,0,0,
				0,0,0,0
			)
		{
		}

		Matrix(
			float _11, float _12, float _13,
			float _21, float _22, float _23,
			float _31, float _32, float _33
		)  noexcept :
			XMFLOAT4X4(
				_11, _12, _13, 0,
				_21, _22, _23, 0,
				_31, _32, _33, 0,
				0, 0, 0, 1
			)
		{
		}

		Matrix(
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
			)
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

	//�P�ʍs���Ԃ�
	Matrix matrix_identity();

	//�S��zero�̍s���Ԃ�
	Matrix matrix_zero();

	//�]�u�s���Ԃ�
	Matrix matrix_trans(const Matrix& M);

	//�s�񎮂�Ԃ�
	float matrix_determinant(const Matrix& M);

	//�s��̋t����Ԃ�
	Matrix matrix_inverse(const Matrix& M);

	//��]�s����I�C���[��(z,x,y)
	Vector3 matrix_to_euler(const Matrix& M);
	//��]�s����I�C���[(radian)��(z,x,y)
	Vector3 matrix_to_euler_radian(const Matrix& M);

	//vector3�ɉ�]�s���������
	Vector3 vector3_trans(const Vector3& V, const Matrix& M);

	//world�s��̐���
	Matrix matrix_world(const Vector3& scale, const Matrix& rotate, const Vector3& trans);

	//matrix����XMMATRIX�ւ̕ϊ�
	DirectX::XMMATRIX matrix_to_XMMATRIX(const Matrix& M);

	//XMMATRIX����matrix�ւ̕ϊ�
	Matrix XMMATRIX_to_matrix(const DirectX::XMMATRIX& M);

	Matrix matrix_cross(const Vector3& vec);

	Matrix matrix_scale(const Vector3& vec);



}

#if _DEBUG || !USE_SIMD
#include "matrix_e.h"
#else
#include "matrixSIM_e.h"
#endif
