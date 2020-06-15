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


		// ���K��
		quaternion unit_vect() const;

		// ����
		quaternion conjugate() const;

		// �t��
		quaternion inverse() const;

		// �I�C���[�p�ŕԂ�
		vector3 euler() const;

		// �I�C���[�p(radian)�ŕԂ�
		vector3 euler_radian() const;

	};

	quaternion quaternion_identity();
	vector3 vector3_be_rotated_by_quaternion(const vector3& V, const quaternion& Q);

	//axis������R�x��]������quaternion�̍쐬
	quaternion quaternion_angle_axis(float R, const vector3& axis);
	//axis������R�x��]������quaternion�̍쐬
	quaternion quaternion_radian_axis(float R, const  vector3& axis);

	//Q1����Q2�ւ̋��ʕ��
	//R = 0 = Q1
	//R = 1 = Q2
	//���얢�m�F
	quaternion quaternion_slerp(const quaternion& Q1, const quaternion& Q2, float R);

	//z��,x��,y���̏��ŉ�]�N�H�[�^�j�I���̐���
	quaternion quaternion_from_euler(float x, float y, float z);
	//z��,x��,y���̏��ŉ�]�N�H�[�^�j�I���̐���
	quaternion quaternion_from_euler(vector3 V);

	//��]�s�񂩂�N�H�[�^�j�I���̐���
	quaternion quaternion_by_rotate_matrix(matrix& M);

	quaternion quaternion_look_at(vector3& me, vector3& you);

	//2��quaternion�̓��ς�Ԃ�
	float quaternion_dot(const quaternion&, const quaternion&);

	//Q1����Q2�ւ̉�]quaternion��Ԃ�
	quaternion quaternion_from_to_rotate(const vector3& V1, const vector3& V2);

	//2��quaternion�̊Ԃ̊p�x��Ԃ�
	float quaternion_angle(const quaternion& Q, const quaternion& T);
	//2��quaternion�̊Ԃ̊p�x��Ԃ�
	float quaternion_radian(const quaternion& Q, const quaternion& T);
};

//typedef Adollib::quaternion quaternion;