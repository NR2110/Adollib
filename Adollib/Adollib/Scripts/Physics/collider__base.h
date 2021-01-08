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

#include "../Imgui/imgui_all.h"

namespace Adollib {

	namespace Physics_function {
		//::: 計算などに使う情報 :::
		struct Collider_data {
			Vector3 local_position = Vector3(0,0,0);
			Quaternion local_orientation = quaternion_identity();
			Vector3 local_scale = Vector3(1, 1, 1);

			Vector3 half_size;

			Physics_function::ALP_Collider_shape shape = Physics_function::ALP_Collider_shape::None; //形情報

			Meshcoll_data meshcoll_data; //meshcollider用data
		};
		struct Physics_data {
			float inertial_mass = 0; //質量
			float drag = 0; //空気抵抗
			float anglar_drag = 0; //空気抵抗
			float dynamic_friction = 0; //動摩擦
			float static_friction = 0; //静摩擦
			float restitution = 0;	 //反発係数

			bool is_fallable = 0; //落ちない
			bool is_kinematic = 0;//影響うけない(fallはする)
			bool is_kinmatic_anglar = 0; //ほかの物体からの影響で回転速度が変化しない
			bool is_kinmatic_linear = 0; //ほかの物体からの影響で並進速度が変化しない
			bool is_moveable = 0; //動かない
			bool is_hitable = 0;  //衝突しない
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
		Physics_function::Physics_data physics_data;

	protected:
		std::list<Physics_function::ALP_Collider>::iterator ALPcollider_itr;
		std::list<Physics_function::ALP_Physics>::iterator ALPphysics_itr;

		std::list<Collider*>::iterator coll_itr;//自身へのitr

	private:
		bool removed = false; //二重にremoveをするのを防ぐ

	public:
		//::: 後で変更する :::
		const Vector3 linear_velocity() const { return ALPphysics_itr->linear_velocity; }
		const Vector3 linear_velocity(Vector3 v) {
			ALPphysics_itr->linear_velocity = v;
			return ALPphysics_itr->linear_velocity;
		}
		const Vector3 linear_anglar_velocity() const { return ALPphysics_itr->anglar_velocity; }
		const Vector3 linear_anglar_velocity(Vector3 v) {
			ALPphysics_itr->anglar_velocity = v;
			return ALPphysics_itr->anglar_velocity;
		}

	public:
		//on collision enter
		const bool concoll_enter(const unsigned int tag_name) const;

		//並進移動に力を加える
		void add_force(const Vector3& force);

		//角回転に力を加える
		void add_torque(const Vector3& force);

		//計算用のstruct
		virtual Physics_function::Collider_data get_Colliderdata() const = 0;
		virtual Physics_function::Meshcoll_data get_Meshdata() { return Physics_function::Meshcoll_data(); };

		const Physics_function::Physics_data get_Physicsdata() const {
			return physics_data;
		};

		//このcolliderをmanagerから削除
		void remove_collider();

	public:
		void awake() override;

		void finalize() override;

	};
}