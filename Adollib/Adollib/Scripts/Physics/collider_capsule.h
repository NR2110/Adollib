#pragma once
#include "collider__base.h"

namespace Adollib {
	//Boxクラス
	class Capsule : public Collider {
	public:
		Vector3	center;	//中心座標
		Vector3	rotate;	//回転
		float r;		//半径
		float length;	//長さ

		Capsule() : center(Vector3(0)), rotate(Vector3(0)), r(1), length(1) { name = std::string("Capsule"); };

		Physics_function::Collider_data get_Colliderdata() const override {
			Physics_function::Collider_data ret;

			ret.local_position = center;
			ret.local_orientation = quaternion_from_euler(rotate);
			ret.local_scale = Vector3(r, length, r);

			ret.half_size = Vector3(1, 1, 1);

			ret.shape = Physics_function::ALP_Collider_shape::Capsule;

			return ret;
		};

	};

}