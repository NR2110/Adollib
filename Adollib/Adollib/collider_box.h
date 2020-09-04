#pragma once
#include "collider__base.h"

namespace Adollib {
	//Box�N���X
	class Box : public Collider {
	public:
		vector3 half_size = vector3();

		//�s���I�u�W�F�N�g�Ƃ��Đ���
				//	vector3 pos = vector3(0, 0, 0),
		//	vector3 scale = vector3(1, 1, 1), 
		//	quaternion orient = quaternion_identity(),
		//	float density = 1,
		//	std::string tag = std::string("Box"),

		//	std::vector<std::string> no_hit_tag = std::vector<std::string>()
		Box(vector3 half_size = vector3(1), float density = 1, vector3 pos = vector3(0, 0, 0)) : half_size(half_size) {
			//shape�̐ݒ�
			shape = Collider_shape::shape_box;

			//���x�̕ۑ�
			this->density = density;

			//���W
			local_position = pos;

			local_scale = vector3(1);

			local_orientation = quaternion(1, 0, 0, 0);

			//���ʂ̌v�Z
			inertial_mass = (half_size.x * half_size.y * half_size.z) * 8.0f * density;

			//�������[�����g�̌v�Z
			inertial_tensor = matrix_identity();
			inertial_tensor._11 = 0.3333333f * inertial_mass * ((half_size.y * half_size.y) + (half_size.z * half_size.z));
			inertial_tensor._22 = 0.3333333f * inertial_mass * ((half_size.z * half_size.z) + (half_size.x * half_size.x));
			inertial_tensor._33 = 0.3333333f * inertial_mass * ((half_size.x * half_size.x) + (half_size.y * half_size.y));
		}

		//�T�C�Y�̏����֐��̃I�[�o�[���C�h
		quaternion get_dimension() const {
			return half_size;
		}
		//world�ϊ��֐��̃I�[�o�[���C�h
		void update_world_trans();

		//size�▧�x���ύX�����Ǝ��ʂ⊮�����[�����g�̕ύX���K�v�ɂȂ邩�炻�̂��߂ɗp��
		void update_inertial(const vector3& half_size, float density = 1) {
			this->density = density;

			//���ʂ̌v�Z
			inertial_mass = (half_size.x * half_size.y * half_size.z) * 8.0f * density;

			//�������[�����g�̌v�Z
			inertial_tensor = matrix_identity();
			inertial_tensor._11 = 0.3333333f * inertial_mass * ((half_size.y * half_size.y) + (half_size.z * half_size.z));
			inertial_tensor._22 = 0.3333333f * inertial_mass * ((half_size.z * half_size.z) + (half_size.x * half_size.x));
			inertial_tensor._33 = 0.3333333f * inertial_mass * ((half_size.x * half_size.x) + (half_size.y * half_size.y));
		}
		//dop6�̍X�V
		void update_dop14();

	};
}