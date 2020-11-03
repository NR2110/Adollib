#pragma once
#include "collider__base.h"

namespace Adollib {
	//Boxクラス
	class Box : public Collider {
	public:
		Vector3	center;
		Vector3	rotate;
		Vector3	size;

		Box();

		////sizeや密度が変更されると質量や完成モーメントの変更が必要になるからそのために用意
		//void update_inertial(const Vector3& half_size, float density = 1) {

		//	//慣性モーメントの計算
		//	inertial_tensor = matrix_identity();
		//	inertial_tensor._11 = 0.3333333f * inertial_mass * ((half_size.y * half_size.y) + (half_size.z * half_size.z));
		//	inertial_tensor._22 = 0.3333333f * inertial_mass * ((half_size.z * half_size.z) + (half_size.x * half_size.x));
		//	inertial_tensor._33 = 0.3333333f * inertial_mass * ((half_size.x * half_size.x) + (half_size.y * half_size.y));
		//}
		////dop6の更新
		//void update_dop14();

		physics_function::Collider_data get_data() {
			physics_function::Collider_data ret;
			ret.local_position = center;
			ret.local_orientation = quaternion_from_euler(rotate);
			ret.local_scale = size;

			ret.shape = physics_function::ALP_Collider_shape::shape_box;
			return ret;
		};
	};
}