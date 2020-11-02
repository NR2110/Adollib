#pragma once
#include "../Math/math.h"

namespace Adollib {
	namespace physics_function {
		enum class ALP_Collider_shape {
			shape_box,
			shape_sphere,
			shape_plane,
			shape_mesh,

			shape_size
		};

		struct ALP_Solverbody {
			Vector3 delta_LinearVelocity; // ���i���x����
			Vector3 delta_AngulaVelocity; // ��]���x����
			Quaternion orientation; // �p��
			Matrix inv_inertia; // �����e���\���̋t�s��
			float  inv_mass; // ���ʂ̋t��
			int num;
		};

		class ALP_Shape {
		public:
			ALP_Collider_shape shape; 		//�`���
			DOP::DOP_14	DOP_14;			//14DOP �f�[�^
			Solverbody* solve; //�Փˉ����p

		public:
			//�I�u�W�F�N�g���������тɌĂяo���@�̂������悢���K���ɖ��t���[���Ă�
			//DOP_14�f�[�^�̍X�V ALP_Shape::dop14,ALP_Colider::scale,GO���K�v�Ȃ��߂����ɋL�q
			virtual void update_dop14() = 0;

		};
	}
}
