#pragma once
#include "collider__base.h"
#include "meshcoll_resorce_manager.h"
namespace Adollib {
	//Mesh collider�N���X
	class Meshcoll : public Collider {
	public:
		vector3 half_size = vector3();
		vector3 offset = vector3();
		DOP_14 dopbase;

		std::vector<int> index;
		std::vector<vector3> vertex;

		//�s���I�u�W�F�N�g�Ƃ��Đ���
		Meshcoll(const char* filename, float density, vector3 pos = vector3(0, 0, 0)) {
			//shape�̐ݒ�
			shape = Collider_shape::shape_mesh;

			Collider_ResourceManager::CreateMCFromFBX(filename, &index, &vertex);

			vector3 half_max = vector3(-FLT_MAX, -FLT_MAX, -FLT_MAX);
			vector3 half_min = vector3(FLT_MAX, FLT_MAX, FLT_MAX);

			memset(dopbase.max, -FLT_MAX, DOP_size);
			memset(dopbase.min,  FLT_MAX, DOP_size);

			//������Ԃ�DOP�̕ۑ� ��]�Ȃǂ��l��������DOP�͂���DOP����ɂ���
			for (int i = 0; i < DOP_size; i++) {
				for (vector3& v : vertex) {

					float dis = fabsf(vector3_dot(DOP_14_axis[i], v));
					if (dopbase.max[i] < dis) dop14.max[i] = +dis * 1.00000001f;//�m���ɂ��邽�߂�����Ƒ傫�߂ɂƂ�
					if (dopbase.min[i] > dis) dop14.min[i] = +dis * 1.00000001f;
				}
			}

			half_size = (half_max + half_min) * 0.5f;
			offset = half_max - half_min;

			//���x�̕ۑ�
			this->density = density;

			//���W
			local_position = pos;

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