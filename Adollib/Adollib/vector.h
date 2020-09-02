#pragma once

#include <DirectXMath.h>
#include <immintrin.h>
#include <math.h>
#include "math_e.h"

namespace Adollib {

	class quaternion;

	//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
	#pragma region vector2
	//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
	class vector2 {
	private:
		// ��const�̌v�Z������ύX������܂�sqrt���g��Ȃ��čς�

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
	// 2�_�Ԃ̋���
	float vector2_distance_sqr(const vector2&, const vector2&);

	// 2�_�Ԃ̋���^2
	float vector2_distance(const vector2&, const vector2&);

	// 2�x�N�g���̓���
	float vector2_dot(const vector2&, const vector2&);

	// 2�x�N�g���̊Ԃ̊p
	float vector2_angle(const vector2&, const vector2&);

	// V1����V2�ւ̃x�N�g��
	vector2 vector2_look_at(const vector2& V1, const vector2& V2);
#pragma endregion
	//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

	//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
    #pragma region vector3
	//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
	class vector3 {

	public:
		union {
			struct {
				float x;
				float y;
				float z;
			};

			struct {
				DirectX::XMFLOAT3 xyz;
			};

			struct {
				vector2 xy;
				float z;
			};

			struct {
				float x;
				vector2 yz;
			};


		};
	private:
	public:
		vector3() :x(0), y(0), z(0) {};
		vector3(float x, float y, float z) :x(x), y(y), z(z) {};
		vector3(vector2 xy, float z) :x(xy.x), y(xy.y), z(z) {};
		explicit vector3(float v) :x(v), y(v), z(v) {};
		vector3(DirectX::XMFLOAT3 xm) :x(xm.x), y(xm.y), z(xm.z) {};
		//vector3(n_vector2 V) :x(V.x), y(V.y), z(0) {};

		vector3 operator+ () const;
		vector3 operator- () const;

		vector3 operator+ (const vector3& M) const;
		vector3 operator- (const vector3& M) const;
		vector3 operator* (const vector3& M) const;
		vector3 operator* (float S) const;
		vector3 operator/ (float S) const;
		float& operator[] (const int i);
		float operator[] (const int i) const;

		vector3& operator+= (const vector3& M);
		vector3& operator-= (const vector3& M);
		vector3& operator*= (float S);
		vector3& operator/= (float S);

		bool operator== (const vector3& M);
		bool operator!= (const vector3& M);

		friend vector3 operator*(float S, const vector3& M);

		//XMFLOAT3�̕ϊ�
		DirectX::XMFLOAT3 get_XM3();


		// norm^2
		float norm_sqr() const;

		// norm
		float norm() const;

		// unit_vect
		vector3 unit_vect() const;
		vector3 unit_vect();

	};

	// 2�_�Ԃ̋���
	float vector3_distance_sqr(const vector3&, const vector3&);

	// 2�_�Ԃ̋���^2
	float vector3_distance(const vector3&, const vector3&);

	// 2�x�N�g���̓���
	float vector3_dot(const vector3&, const vector3&);

	// 2�x�N�g���̊O��
	vector3 vector3_cross(const vector3&, const vector3&);

	// 2�x�N�g���̊Ԃ̊p
	float vector3_angle(const vector3&, const vector3&);
	// 2�x�N�g���̊Ԃ̊p
	float vector3_radian(const vector3&, const vector3&);

	// V1����V2�ւ̃x�N�g��
	vector3 vector3_look_at(const vector3& V1, const vector3& V2);

	//Q1����Q2�ւ̋��ʕ��
	//R = 0 = V1
	//R = 1 = V2
	vector3 vector3_slerp(const vector3& V1, const vector3& V2, float R);

#pragma endregion
	//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

	//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
	#pragma region vector4
	//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
	class vector4 {

	public:
		union {
			struct {
				float x;
				float y;
				float z;
				float w;
			};
			struct {
				vector3 xyz;
				float w;
			};
		};
	private:

	public:
		vector4() :x(0), y(0), z(0), w(0) {};
		vector4(vector3 xyz, float w) :x(xyz.x), y(xyz.y), z(xyz.z), w(w) {};
		vector4(float x, float y, float z, float w) :x(x), y(y), z(z), w(w) {};
		//vector3(n_vector2 V) :x(V.x), y(V.y), z(0) {};

		vector4 operator+ () const;
		vector4 operator- () const;

		vector4 operator+ (const vector4& M) const;
		vector4 operator- (const vector4& M) const;
		vector4 operator* (const vector4& M) const;
		vector4 operator* (float S) const;
		vector4 operator/ (float S) const;
		float& operator[] (const int i);
		float operator[] (const int i) const;

		vector4& operator+= (const vector4& M);
		vector4& operator-= (const vector4& M);
		vector4& operator*= (float S);
		vector4& operator/= (float S);

		bool operator== (const vector4& M);
		bool operator!= (const vector4& M);

		friend vector4 operator*(float S, const vector4& M);

		//XMFLOAT3�̕ϊ�
		DirectX::XMFLOAT4 get_XM4();


		// norm^2
		float norm_sqr() const;

		// norm
		float norm() const;

		// unit_vect
		vector4 unit_vect() const;
		vector4 unit_vect();

	};

#pragma endregion
	//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

}



#include "vector_e.h"