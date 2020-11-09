#pragma once
#include "collider__base.h"

namespace Adollib {
	//平面用クラス
	class Plane : public Collider {
	public:
		Vector3 normal; //平面の向き
		float distance; //平面の距離

		//不動オブジェクトとして生成
		Plane():normal(Vector3(0,1,0)),distance(0)
		{
			is_moveable = false;
			inertial_mass = FLT_MAX;
		}

		physics_function::Collider_data get_Colliderdata() const override {
			physics_function::Collider_data ret;

			ret.local_orientation = quaternion_from_to_rotate(Vector3(0, 1, 0), normal).unit_vect();
			ret.local_position = normal * distance;
			ret.local_scale = Vector3(FLT_MAX, 0, FLT_MAX);

			ret.half_size = Vector3(1, 1, 1);

			ret.shape = physics_function::ALP_Collider_shape::Plane;

			return ret;
		}

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