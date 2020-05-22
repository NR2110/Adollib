#pragma once
#include <DirectXMath.h>


namespace Adollib {

	//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
    #pragma region vector3
	//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
	class vector3 {

	public:
		float x;
		float y;
		float z;

	private:
		// 非constの計算したら変更があるまでsqrtを使わなくて済む
		float normed;
	public:
		vector3() :x(0), y(0), z(0) {};
		vector3(float x, float y, float z) :x(x), y(y), z(z) {};
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

		//XMFLOAT3の変換
		DirectX::XMFLOAT3 get_XM3();


		// norm^2
		float norm_sqr() const;
		float norm_sqr();

		// norm
		float norm() const;

		// unit_vect
		vector3 unit_vect() const;
		vector3 unit_vect();

	};

	// 2点間の距離
	float vector3_distance_sqr(const vector3&, const vector3&);

	// 2点間の距離^2
	float vector3_distance(const vector3&, const vector3&);

	// 2ベクトルの内積
	float vector3_dot(const vector3&, const vector3&);

	// 2ベクトルの外積
	vector3 vector3_cross(const vector3&, const vector3&);

	// 2ベクトルの間の角
	float vector3_angle(const vector3&, const vector3&);
	// 2ベクトルの間の角
	float vector3_radian(const vector3&, const vector3&);

	// V1からV2へのベクトル
	vector3 vector3_look_at(const vector3& V1, const vector3& V2);

	//Q1からQ2への球面補間
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
		float x;
		float y;
		float z;
		float w;

	private:
		// 非constの計算したら変更があるまでsqrtを使わなくて済む
		float normed;
	public:
		vector4() :x(0), y(0), z(0), w(0) {};
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

		//XMFLOAT3の変換
		DirectX::XMFLOAT4 get_XM4();


		// norm^2
		float norm_sqr() const;
		float norm_sqr();

		// norm
		float norm() const;

		// unit_vect
		vector4 unit_vect() const;
		vector4 unit_vect();

	};

#pragma endregion
	//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

	//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
	#pragma region vector2
	//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
	class n_vector2 {
	private:
		// 非constの計算したら変更があるまでsqrtを使わなくて済む
		float normed;

	public:
		float x;
		float y;

		n_vector2 operator+ () const;
		n_vector2 operator- () const;

		n_vector2 operator+ (const n_vector2& M) const;
		n_vector2 operator- (const n_vector2& M) const;
		n_vector2 operator* (const n_vector2& M) const;
		n_vector2 operator* (float S) const;
		n_vector2 operator/ (float S) const;

		n_vector2& operator+= (const n_vector2& M);
		n_vector2& operator-= (const n_vector2& M);
		n_vector2& operator*= (float S);
		n_vector2& operator/= (float S);

		friend n_vector2 operator*(float S, const n_vector2& M);


		// norm^2
		float norm_sqr() const;
		float norm_sqr();

		// norm
		float norm() const;

		// unit_vect
		n_vector2 unit_vect() const;
		n_vector2 unit_vect();


	};
	// 2点間の距離
	float vector2_distance_sqr(const n_vector2&, const n_vector2&);

	// 2点間の距離^2
	float vector2_distance(const n_vector2&, const n_vector2&);

	// 2ベクトルの内積
	float vector2_dot(const n_vector2&, const n_vector2&);

	// 2ベクトルの間の角
	float vector2_angle(const n_vector2&, const n_vector2&);

	// V1からV2へのベクトル
	n_vector2 vector2_look_at(const n_vector2& V1, const n_vector2& V2);
#pragma endregion
	//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

	float ToRadian(float angle);
	float ToAngle(float radian);
}