#pragma once

#pragma once

#include <vector>
#include <assert.h>
#include <string>
#include <unordered_map>
#include "../Math/math.h"
#include "../Object/component.h"

//#include "ALP__meshcoll_data.h"
#include "ALP_collider.h"
#include "ALP_physics.h"

#include "../Imgui/imgui_all.h"

namespace Adollib {

	namespace Physics_function {

		//表示用のphysics_data ユーザーが簡単に変更できるように
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

	}

	class Collider : public Component {

	public:
		//::: tag ::::::::
		Collider_tagbit tag = 0; //自身のtag(bit)
		Collider_tagbit ignore_tags = 0; //衝突しないtags(bit)

		//::: unityのphysics部分 分ける必要なんてないやろ ::::
		Physics_function::Physics_data physics_data;

		//static同士で衝突判定を行わない
		bool is_static = false;

	private:
		Physics_function::ALP_Collider* ALPcollider_ptr = nullptr;
		Physics_function::ALP_Physics*  ALPphysics_ptr = nullptr;

		std::list<Collider*>::iterator coll_itr;//自身へのitr

	public:
		//::: 後で変更する :::
		const Vector3 linear_velocity() const { return ALPphysics_ptr->linear_velocity; }
		const Vector3 linear_velocity(Vector3 v) {
			ALPphysics_ptr->linear_velocity = v;
			return ALPphysics_ptr->linear_velocity;
		}
		const Vector3 linear_anglar_velocity() const { return ALPphysics_ptr->anglar_velocity; }
		const Vector3 linear_anglar_velocity(Vector3 v) {
			ALPphysics_ptr->anglar_velocity = v;
			return ALPphysics_ptr->anglar_velocity;
		}

	public:

		//交差していたらtrueを返す
		const bool concoll_enter(const unsigned int tag_name) const;

		//並進移動に力を加える
		void add_force(const Vector3& force);

		//角回転に力を加える
		void add_torque(const Vector3& force);

		//shapeのアタッチ
		template<typename T>
		T* add_shape() { return ALPcollider_ptr->add_shape<T>(); };

		//meshcolliderのアタッチ
		void add_shape(const char* filepass, bool is_right_rtiangle = true);

		//慣性モーメントをユーザー定義で設定する
		void set_tensor(const Matrix& tensor) { ALPphysics_ptr->set_tensor(tensor); };


	public:
		void Update_hierarchy();

		void awake() override;

		void finalize() override;

	};
}

#include "collider_box.h"
#include "collider_plane.h"
#include "collider_sphere.h"
#include "collider_capsule.h"
#include "collider_meshcoll.h"