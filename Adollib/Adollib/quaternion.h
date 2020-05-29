#pragma once

#include <DirectXMath.h>
#include "vector.h"
#include "matrix.h"

namespace Adollib {

	class quaternion {
	public:

		float x;
		float y;
		float z;
		float w;


	private:

	public:

		quaternion();
		quaternion(DirectX::XMFLOAT3);
		quaternion(vector3);
		//::::::::::
		// x, y, z
		//::::::::::
		quaternion(float x, float y, float z);
		//::::::::::
		// w, x, y, z
		//::::::::::
		quaternion(float w, float x, float y, float z);

		quaternion(const quaternion& Q);
		~quaternion() {}

		quaternion operator+(const quaternion&) const;
		quaternion operator-(const quaternion&) const;
		quaternion operator*(float) const;
		quaternion operator*(const quaternion&) const;
		quaternion operator/(float) const;

		quaternion& operator+=(const quaternion&);
		quaternion& operator-=(const quaternion&);
		quaternion& operator*=(float);
		quaternion& operator*=(const quaternion&);
		quaternion& operator/=(float);

		quaternion operator+() const;
		quaternion operator-() const;

		friend quaternion operator* (float S, const quaternion& Q);

	public:
		DirectX::XMFLOAT3 get_XM3() const;
		vector3 get_NV3() const;
		DirectX::XMFLOAT4 get_XM4() const;
		matrix get_rotate_matrix() const;

		// length
		float norm_sqr() const;

		// length^2
		float norm() const;


		// 正規化
		quaternion unit_vect() const;

		// 共役
		quaternion conjugate() const;

		// 逆元
		quaternion inverse() const;

		// オイラー角で返す
		vector3 euler() const;

		// オイラー角(radian)で返す
		vector3 euler_radian() const;

	};

	quaternion quaternion_identity();
	vector3 vector3_be_rotated_by_quaternion(const vector3& V, const quaternion& Q);

	//axisを軸にR度回転させるquaternionの作成
	quaternion quaternion_angle_axis(float R, const vector3& axis);
	//axisを軸にR度回転させるquaternionの作成
	quaternion quaternion_radian_axis(float R, const  vector3& axis);

	//Q1からQ2への球面補間
	//R = 0 = Q1
	//R = 1 = Q2
	quaternion quaternion_slerp(const quaternion& Q1, const quaternion& Q2, float R);

	//z軸,x軸,y軸の順で回転クォータニオンの生成
	quaternion quaternion_from_euler(float x, float y, float z);
	//z軸,x軸,y軸の順で回転クォータニオンの生成
	quaternion quaternion_from_euler(vector3 V);
	////任意軸で回転クォータニオンの生成
	//quaternion quaternion_from_euler_by_local_axis(vector3 axis, float x, float y, float z);
	////任意軸で回転クォータニオンの生成
	//quaternion quaternion_from_euler_by_local_axis(vector3 axis, vector3 V);

	//回転行列からクォータニオンの生成
	quaternion quaternion_by_rotate_matrix(matrix& M);

	quaternion quaternion_look_at(vector3& me, vector3& you);

	//2つのquaternionの内積を返す
	float quaternion_dot(const quaternion&, const quaternion&);

	//Q1からQ2への回転quaternionを返す
	quaternion quaternion_from_to_rotate(const vector3& V1, const vector3& V2);

	//2つのquaternionの間の角度を返す
	float quaternion_angle(const quaternion& Q, const quaternion& T);
	//2つのquaternionの間の角度を返す
	float quaternion_radian(const quaternion& Q, const quaternion& T);
};

//typedef Adollib::quaternion quaternion;