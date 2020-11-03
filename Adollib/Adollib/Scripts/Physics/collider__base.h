#pragma once

#include <vector>
#include <assert.h>
#include <string>
#include <unordered_map>
#include "../Math/math.h"
#include "../Object/component.h"

#include "ALP_collider.h"
#include "ALP_physics.h"
#include "ALP_shape.h"

namespace Adollib {

	namespace physics_function {
		//::: 計算に使う情報 :::
		struct Collider_data {
			Vector3 local_position;
			Quaternion local_orientation;
			Vector3 local_scale;

			physics_function::ALP_Collider_shape shape;
		};

		struct ColliderPhysicsShape_itrs {
			std::list<Collider*>::iterator coll_itr;

			std::list<ALP_Collider>::iterator ALPcollider_itr;
			std::list<ALP_Physics>::iterator ALPphysics_itr;
			std::list<ALP_Shape>::iterator ALPshape_itr;
		};

	}

	class Collider : public Component {

	public:
		//::: unityのphysics部分 分ける必要なんてないやろ ::::
		float inertial_mass; //質量
		float drag; //空気抵抗
		float anglar_drag; //空気抵抗
		float dynamic_friction; //動摩擦
		float static_friction; //静摩擦
		float restitution;	 //反発係数

		bool is_fallable; //落ちない
		bool is_kinematic; //影響うけない(fallはする)
		bool is_moveable; //動かない

	private:
		std::list<physics_function::ALP_Collider>::iterator ALPcollider_itr;
		std::list<physics_function::ALP_Physics>::iterator ALPphysics_itr;
		std::list<physics_function::ALP_Shape>::iterator ALPshape_itr;

		std::list<Collider*>::iterator coll_itr;//自身へのitr
		
	public:
		//自身へのitrを返す
		const physics_function::ColliderPhysicsShape_itrs get_itrs() const {
			physics_function::ColliderPhysicsShape_itrs ret; 

			ret.ALPcollider_itr = ALPcollider_itr;
			ret.ALPphysics_itr = ALPphysics_itr;
			ret.ALPshape_itr = ALPshape_itr;

			ret.coll_itr = coll_itr;

			return ret;
		};

	public:
		//on collision enter
		bool concoll_enter(ALP_tags tag_name);

		//並進移動に力を加える
		void add_force(const Vector3& force);

		//角回転に力を加える
		void add_torque(const Vector3& force);

		virtual physics_function::Collider_data get_data() = 0;

	public:
		void awake() override;

		void finalize() override;

	};
}