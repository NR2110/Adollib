#pragma once

#include <DirectXMath.h>

namespace Adollib {

	class Vector3;
	class Matrix;

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
		Matrix get_rotate_matrix() const;

		// length
		float norm_sqr() const;

		// length^2
		float norm() const;


		// ���K��
		Quaternion unit_vect() const;

		// ����
		Quaternion conjugate() const;

		// �t��
		Quaternion inverse() const;

		// �I�C���[�p�ŕԂ�
		Vector3 euler() const;

		// �I�C���[�p(radian)�ŕԂ�
		Vector3 euler_radian() const;

	};

	Quaternion quaternion_identity();
	Vector3 vector3_quatrotate(const Vector3& V, const Quaternion& Q);

	//axis������R�x��]������quaternion�̍쐬
	Quaternion quaternion_angle_axis(float R, const Vector3& axis);
	//axis������R�x��]������quaternion�̍쐬
	Quaternion quaternion_radian_axis(float R, const  Vector3& axis);

	//Q1����Q2�ւ̋��ʕ��
	//R = 0 = Q1
	//R = 1 = Q2
	//���얢�m�F
	Quaternion quaternion_slerp(const Quaternion& Q1, const Quaternion& Q2, float R);

	//z��,x��,y���̏��ŉ�]�N�H�[�^�j�I���̐���
	Quaternion quaternion_from_euler(float x, float y, float z);
	//z��,x��,y���̏��ŉ�]�N�H�[�^�j�I���̐���
	Quaternion quaternion_from_euler(Vector3 V);

	//��]�s�񂩂�N�H�[�^�j�I���̐���
	Quaternion quaternion_by_rotate_matrix(Matrix& M);

	Quaternion quaternion_look_at(Vector3& me, Vector3& you);

	//2��quaternion�̓��ς�Ԃ�
	float quaternion_dot(const Quaternion&, const Quaternion&);

	//Q1����Q2�ւ̉�]quaternion��Ԃ�
	Quaternion quaternion_from_to_rotate(const Vector3& V1, const Vector3& V2);

	//2��quaternion�̊Ԃ̊p�x��Ԃ�
	float quaternion_angle(const Quaternion& Q, const Quaternion& T);
	//2��quaternion�̊Ԃ̊p�x��Ԃ�
	float quaternion_radian(const Quaternion& Q, const Quaternion& T);
};

#include "quaternion_e.h"