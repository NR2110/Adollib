#pragma once
#include "../Math/math.h"

namespace Adollib {
	namespace physics_function {
		enum class Collider_shape {
			shape_box,
			shape_sphere,
			shape_plane,
			shape_mesh,

			shape_size
		};

		struct Solverbody {
			Vector3 delta_LinearVelocity; // ���i���x����
			Vector3 delta_AngulaVelocity; // ��]���x����
			Quaternion orientation; // �p��
			Matrix inv_inertia; // �����e���\���̋t�s��
			float  inv_mass; // ���ʂ̋t��
			int num;
		};

		class Shape {
			Collider_shape shape; 		//�`���
			DOP::DOP_14	DOP_14;			//14DOP �f�[�^
			Solverbody* solve; //�Փˉ����p

		};
	}
}
