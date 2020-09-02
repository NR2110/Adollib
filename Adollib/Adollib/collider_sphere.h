#pragma once
#include "collider__base.h"

namespace Adollib {

	//���̗p�N���X
	class Sphere : public Collider {
	public:
		float r = 1; //���a

		Sphere(float r, float density, vector3 pos = vector3(0, 0, 0)) : r(r) {
			//shape�̐ݒ�
			shape = Collider_shape::shape_sphere;

			//���x�̕ۑ�
			this->density = density;

			//���W
			local_position = pos;

			local_orientation = quaternion_identity();

			//���ʂ̌v�Z
			inertial_mass = 4.0f / 3.0f * r * r * r * DirectX::XM_PI * density;

			//�������[�����g�̌v�Z
			inertial_tensor = matrix_identity();
			inertial_tensor._11 = 0.4f * inertial_mass * r * r;
			inertial_tensor._22 = 0.4f * inertial_mass * r * r;
			inertial_tensor._33 = 0.4f * inertial_mass * r * r;


		}

		//�T�C�Y�̏����֐��̃I�[�o�[���C�h
		quaternion get_dimension() const {
			return quaternion(r, r, r);
		}
		//world�ϊ��֐��̃I�[�o�[���C�h
		void update_world_trans();
		//size�▧�x���ύX�����Ǝ��ʂ⊮�����[�����g�̕ύX���K�v�ɂȂ邩�炻�̂��߂ɗp��(���̔��a = size.x)
		void update_inertial(const vector3& size, float density = 1) {
			this->density = density;

			float r = size.x;
			//���ʂ̌v�Z
			inertial_mass = 4.0f / 3.0f * r * r * r * DirectX::XM_PI * density;

			//�������[�����g�̌v�Z
			inertial_tensor = matrix_identity();
			inertial_tensor._11 = 0.4f * inertial_mass * r * r;
			inertial_tensor._22 = 0.4f * inertial_mass * r * r;
			inertial_tensor._33 = 0.4f * inertial_mass * r * r;
		}
		//dop6�̍X�V
		void update_dop14();

	};

}