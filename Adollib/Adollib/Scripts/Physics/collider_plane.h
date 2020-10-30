#pragma once
#include "collider__base.h"

namespace Adollib {
	//���ʗp�N���X
	class Plane : public Collider {
	public:
		//�s���I�u�W�F�N�g�Ƃ��Đ���
		Plane(Vector3 n = Vector3(0, 1, 0), float d = 0) {
			//shape�̐ݒ�
			shape = Collider_shape::shape_plane;

			//���ʂ̌v�Z
			inertial_mass = FLT_MAX;

			//�������[�����g�̌v�Z
			inertial_tensor = matrix_identity();
			inertial_tensor._11 = FLT_MAX;
			inertial_tensor._22 = FLT_MAX;
			inertial_tensor._33 = FLT_MAX;

			//�����Ȃ�
			move = false;

			n = n.unit_vect();
			local_position = n * d;

			Vector3 Y(0, 1, 0);
			float angle = acosf(vector3_dot(Y, n));
			Vector3 axis;
			axis = vector3_cross(Y, n);
			axis = axis.unit_vect();
			local_orientation = quaternion_angle_axis(angle, axis);
			local_orientation = local_orientation.unit_vect();
		}

		//�T�C�Y�̏����֐��̃I�[�o�[���C�h
		Quaternion get_dimension() const {
			return Quaternion(1, 0, 1);
		}
		//world�ϊ��֐��̃I�[�o�[���C�h
		void update_world_trans();
		//size�▧�x���ύX�����Ǝ��ʂ⊮�����[�����g�̕ύX���K�v�ɂȂ邩�炻�̂��߂ɗp��(plane�͏����Ȃ�)
		void update_inertial(const Vector3& size, float density = 1) {
			//float r = size.x;
			////���ʂ̌v�Z
			//inertial_mass = 4.0f / 3.0f * r * r * r * DirectX::XM_PI * density;

			////�������[�����g�̌v�Z
			//inertial_tensor = matrix_identity();
			//inertial_tensor._11 = 0.4f * inertial_mass * r * r;
			//inertial_tensor._22 = 0.4f * inertial_mass * r * r;
			//inertial_tensor._33 = 0.4f * inertial_mass * r * r;
		}
		//dop6�̍X�V
		void update_dop14();

	};

}