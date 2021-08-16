#pragma once

#include <vector>
#include <assert.h>
#include <string>
#include <unordered_map>
#include "../Math/math.h"
#include "../Object/component.h"

#include "ALP__tags.h"
#include "ALP_collider.h"
#include "ALP_physics.h"

#include "../Imgui/imgui_all.h"

namespace Adollib {
	class Collider;



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

	struct Contacted_data {
		Collider* coll; //相手のcollider
		float penetrate = 0; //貫通量
		Vector3 normal; //衝突法線 world座標系
		Vector3 contacted_pointA; //自身のGO座標系の衝突点
		Vector3 contacted_pointB; //相手のGO座標系の衝突点
	};
	//:::::::::::::::::::::::::



	namespace Physics_function {
		class ALP_Joint;
		class ALP_Collider;
		class ALP_Physics;
		namespace Contacts {
			struct Contact_pair;
		}
	}


	class Collider : public Component {

	public:
		//::: tag ::::::::
		Collider_tagbit tag = 0; //自身のtag(bit)
		Collider_tagbit ignore_tags = 0; //衝突しないtags(bit)

		//::: unityのphysics部分 分ける必要なんてないやろ ::::
		Physics_data physics_data;

		//::: static同士で衝突判定を行わない :::
		bool is_static = false;

		//::: 自身の関わるcontact_pairの情報をメンバに保存するかどうか :::
		bool is_save_contacted_colls = false;

		//::: is_save_contacted_collsがtrueの時 自身の関わるcontact_pairの情報を保存する :::
		std::vector<Contacted_data> contacted_colliders;

	private:
		Physics_function::ALP_Collider* ALPcollider_ptr = nullptr;
		Physics_function::ALP_Physics* ALPphysics_ptr = nullptr;

	public:
		//::: 後で変更する :::
		const Vector3 linear_velocity() const { return ALPphysics_ptr->linear_velocity; }
		const Vector3 linear_velocity(Vector3 v) {
			ALPphysics_ptr->linear_velocity = v;
			return ALPphysics_ptr->linear_velocity;
		}
		const Vector3 angula_velocity() const { return ALPphysics_ptr->angula_velocity; }
		const Vector3 angula_velocity(Vector3 v) {
			ALPphysics_ptr->angula_velocity = v;
			return ALPphysics_ptr->angula_velocity;
		}

	public:
		//jointに自身の保持するALPColliderの情報を入れる
		void set_ptr_to_joint(Physics_function::ALP_Joint*& joint_base);

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
		void add_shape(const char* filepass, bool is_right_rtiangle = true, bool is_permit_edge_have_many_facet = false);

		//慣性モーメントをユーザー定義で設定する
		void set_tensor(const Matrix33& tensor) { ALPphysics_ptr->set_tensor(tensor); };

		void set_barycenter(const Vector3& cent) { ALPphysics_ptr->set_barycenter(cent); };

		//重心のlocal座標を返す
		const Vector3 get_barycenter() const { return ALPphysics_ptr->get_barycenter(); };

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