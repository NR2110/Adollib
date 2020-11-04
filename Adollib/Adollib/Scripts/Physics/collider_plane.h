#pragma once
#include "collider__base.h"

namespace Adollib {
	//���ʗp�N���X
	class Plane : public Collider {
	public:
		Vector3 normal; //���ʂ̌���
		float distance; //���ʂ̋���

		//�s���I�u�W�F�N�g�Ƃ��Đ���
		Plane():normal(Vector3(0,1,0)),distance(0)
		{
			is_moveable = false;
			inertial_mass = FLT_MAX;
		}

		physics_function::Collider_data get_data() override {
			physics_function::Collider_data ret;

			ret.local_orientation = quaternion_from_to_rotate(Vector3(0, 1, 0), normal).unit_vect();
			ret.local_position = normal * distance;
			ret.local_scale = Vector3(FLT_MAX, 0, FLT_MAX);

			ret.shape = physics_function::ALP_Collider_shape::Plane;
		}

	};

}