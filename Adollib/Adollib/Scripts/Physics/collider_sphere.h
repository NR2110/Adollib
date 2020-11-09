#pragma once
#include "collider__base.h"

namespace Adollib {

	//球体用クラス
	class Sphere : public Collider {
	public:
		Vector3 center; //中心座標
		float r = 1; //半径

		Sphere() : center(Vector3(0)), r(1) {};

		physics_function::Collider_data get_Colliderdata() const override {
			physics_function::Collider_data ret;

			ret.local_orientation = center;
			ret.local_orientation = quaternion_identity();
			ret.local_scale = Vector3(r);

			ret.half_size = Vector3(1, 1, 1);

			ret.shape = physics_function::ALP_Collider_shape::Sphere;

			return ret;
		};

		void awake() override {
			physics_function::ColliderPhysics_itrs data;

			data = Phyisics_manager::add_collider(this);

			ALPcollider_itr = data.ALPcollider_itr;
			ALPphysics_itr = data.ALPphysics_itr;
			coll_itr = data.coll_itr;

			data.ALPcollider_itr->coll_itr = coll_itr;

		}

	};

}