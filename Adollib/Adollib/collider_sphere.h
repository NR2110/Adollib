#pragma once
#include "collider__base.h"

namespace Adollib {

	//���̗p�N���X
	class Sphere : public Collider {
	public:
		float r = 1; //���a

		Sphere(float r = 1, float density = 1, Vector3 pos = Vector3(0, 0, 0)) : r(r) {
			//shape�̐ݒ�
			shape = Collider_shape::shape_sphere;

			//���x�̕ۑ�
			this->density = density;

			//���W
			local_position = pos;

			local_orientation = quaternion_identity();

			local_scale = Vector3(1);

			//���ʂ̌v�Z
			inertial_mass = 4.0f / 3.0f * r * r * r * DirectX::XM_PI * density;

			//�������[�����g�̌v�Z
			inertial_tensor = matrix_identity();
			inertial_tensor._11 = 0.4f * inertial_mass * r * r;
			inertial_tensor._22 = 0.4f * inertial_mass * r * r;
			inertial_tensor._33 = 0.4f * inertial_mass * r * r;


		}

		//�T�C�Y�̏����֐��̃I�[�o�[���C�h
		Quaternion get_dimension() const {
			return Quaternion(r, r, r);
		}
		//world�ϊ��֐��̃I�[�o�[���C�h
		void update_world_trans();
		//size�▧�x���ύX�����Ǝ��ʂ⊮�����[�����g�̕ύX���K�v�ɂȂ邩�炻�̂��߂ɗp��(���̔��a = size.x)
		void update_inertial(const Vector3& size, float density = 1) {
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