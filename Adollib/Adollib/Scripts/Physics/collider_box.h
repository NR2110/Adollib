#pragma once
#include "collider__base.h"

namespace Adollib {
	//Box�N���X
	class Box : public Collider {
	public:
		Vector3	center;//���S���W
		Vector3	rotate;//��]
		Vector3	size;//�傫��

		Box() : center(Vector3(0)), rotate(Vector3(0)), size(Vector3(1)) {};

		physics_function::Collider_data get_Colliderdata() const override {
			physics_function::Collider_data ret;

			ret.local_position = center;
			ret.local_orientation = quaternion_from_euler(rotate);
			ret.local_scale = size;

			ret.half_size = Vector3(1, 1, 1);

			ret.shape = physics_function::ALP_Collider_shape::BOX;

			return ret;
		};
	};
}