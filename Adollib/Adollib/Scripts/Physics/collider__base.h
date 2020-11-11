#pragma once

#include <vector>
#include <assert.h>
#include <string>
#include <unordered_map>
#include "../Math/math.h"
#include "../Object/component.h"

#include "ALP__meshcoll_data.h"
#include "ALP_collider.h"
#include "ALP_physics.h"

namespace Adollib {

	namespace physics_function {
		//::: 計算などに使う情報 :::
		struct Collider_data {
			Vector3 local_position = Vector3(0,0,0);
			Quaternion local_orientation = quaternion_identity();
			Vector3 local_scale = Vector3(1, 1, 1);

			Vector3 half_size;

			physics_function::ALP_Collider_shape shape = physics_function::ALP_Collider_shape::None; //形情報

			Meshcoll_data meshcoll_data; //meshcollider用data
		};
		struct Physics_data {
			float inertial_mass; //質量
			float drag; //空気抵抗
			float anglar_drag; //空気抵抗
			float dynamic_friction;//動摩擦
			float static_friction; //静摩擦
			float restitution;	 //反発係数

			bool is_fallable; //落ちない
			bool is_kinematic; //影響うけない(fallはする)
			bool is_moveable;//動かない
			bool is_hitable;  //衝突しない
		};
		//:::::::::::::::::::::::::

		struct ColliderPhysics_itrs {
			std::list<ALP_Collider>::iterator ALPcollider_itr;
			std::list<ALP_Physics>::iterator ALPphysics_itr;

			std::list<Collider*>::iterator coll_itr;

		};

	}

	class Collider : public Component {

	public:
		//::: tag関係 ::::::::
		u_int tag = 0; //自身のtag(bit)
		u_int not_hitable_tags = 0; //衝突しないtags

		//::: unityのphysics部分 分ける必要なんてないやろ ::::
		float inertial_mass = 0; //質量
		float drag = 0; //空気抵抗
		float anglar_drag = 0; //空気抵抗
		float dynamic_friction = 0; //動摩擦
		float static_friction = 0; //静摩擦
		float restitution = 0;	 //反発係数

		bool is_fallable = 0; //落ちない
		bool is_kinematic = 0;//影響うけない(fallはする)
		bool is_moveable = 0; //動かない
		bool is_hitable = 0;  //衝突しない

	protected:
		std::list<physics_function::ALP_Collider>::iterator ALPcollider_itr;
		std::list<physics_function::ALP_Physics>::iterator ALPphysics_itr;

		std::list<Collider*>::iterator coll_itr;//自身へのitr

	private:
		bool removed = false; //二重にremoveをするのを防ぐ

	public:
		//on collision enter
		const bool concoll_enter(const unsigned int tag_name) const;

		//並進移動に力を加える
		void add_force(const Vector3& force);

		//角回転に力を加える
		void add_torque(const Vector3& force);

		//計算用のstruct
		virtual physics_function::Collider_data get_Colliderdata() const = 0;
		virtual physics_function::Meshcoll_data get_Meshdata() { return physics_function::Meshcoll_data(); };

		const physics_function::Physics_data get_Physicsdata() const {
			physics_function::Physics_data data;
			data.inertial_mass = inertial_mass;
			data.drag = drag;
			data.anglar_drag = anglar_drag;
			data.dynamic_friction = dynamic_friction;
			data.static_friction = static_friction;
			data.restitution = restitution;

			data.is_fallable = is_fallable;
			data.is_kinematic = is_kinematic;
			data.is_moveable = is_moveable;
			data.is_hitable = is_hitable;
			return data;
		};

		//このcolliderをmanagerから削除
		void remove_collider();

	public:
		void awake() override;

		void finalize() override;

	};
}