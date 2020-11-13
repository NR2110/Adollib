#pragma once
#include "collider__base.h"

namespace Adollib {

	//���̗p�N���X
	class Sphere : public Collider {
	public:
		Vector3 center; //���S���W
		float r = 1; //���a

		Sphere() : center(Vector3(0)), r(1) {};

		Physics_function::Collider_data get_Colliderdata() const override {
			Physics_function::Collider_data ret;

			ret.local_orientation = center;
			ret.local_orientation = quaternion_identity();
			ret.local_scale = Vector3(r);

			ret.half_size = Vector3(1, 1, 1);

			ret.shape = Physics_function::ALP_Collider_shape::Sphere;

			return ret;
		};

	};

}