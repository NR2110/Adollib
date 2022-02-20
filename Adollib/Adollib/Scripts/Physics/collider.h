#pragma once

#include <vector>
#include <assert.h>
#include <string>
#include <unordered_map>
#include "../Math/math.h"
#include "../Object/component.h"

#include "ALP__tags.h"
//#include "ALP_collider.h"
#include "ALP_physics.h"

#include "../Imgui/imgui_all.h"

#include "ALP_struct_contacted_data.h"

namespace Adollib {

	//表示用のphysics_data ユーザーが簡単に変更できるように
	struct Physics_data {
		float inertial_mass = 0; //質量
		float drag = 0; //空気抵抗
		float anglar_drag = 0; //空気抵抗
		float dynamic_friction = 0; //動摩擦
		float static_friction = 0; //静摩擦
		float restitution = 0;	 //反発係数
		float linear_sleep_threrhold = 0; //freezeの閾値
		float angula_sleep_threrhold = 0; //freezeの閾値

		bool is_fallable = false; // 落ちない
		bool is_kinematic = false;// 影響うけない(fallはする)
		bool is_kinmatic_anglar = false; // ほかの物体からの影響で回転速度が変化しない
		bool is_kinmatic_linear = false; // ほかの物体からの影響で並進速度が変化しない
		bool is_moveable = false; // 動かない
		bool is_hitable = false;  // 衝突しない
		bool is_static = false;  // static同士はoncoll_enterが使えない けど軽くなる
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

	private:
		Physics_function::ALP_Physics* ALPphysics_ptr = nullptr;
		Physics_function::ALP_Collider* ALPcollider_ptr = nullptr;

	public:
		//::: 後で変更する :::
		const Vector3 linear_velocity() const { return ALPphysics_ptr->linear_velocity(); }
		const Vector3 angula_velocity() const { return ALPphysics_ptr->angula_velocity(); }
		const void linear_velocity(Vector3 v) {
			ALPphysics_ptr->set_linear_velocity(v);
			ALPphysics_ptr->set_old_linear_velocity(v);
		}
		const void angula_velocity(Vector3 v) {
			ALPphysics_ptr->set_angula_velocity(v);
			ALPphysics_ptr->set_old_angula_velocity(v);
		}

		// 指定した一点での速度
		const Vector3 get_point_velocity(const Vector3& pos, bool is_local = false);

		// アタッチされたjointの数
		const int get_joint_count();
		// 指定した番号にアタッチされているjointの情報を得る
		Joint_base* get_joint(const int num);

	public:
		// jointに自身の保持するALPColliderの情報を入れる
		void set_ptr_to_joint(Physics_function::ALP_Joint* joint_base);

	public:
		// 交差していたらtrueを返す
		const bool concoll_enter(const Collider_tagbit tag_name);

		// 並進移動に力を加える
		void add_force(const Vector3& force, const bool& is_force_local = false);

		// 並進移動に力を加える
		void add_force(const Vector3& force, const Vector3& position, const bool& is_position_local = false, const bool& is_force_local = false);

		// 角回転に力を加える
		void add_torque(const Vector3& force, const bool& is_local = false);

		// 並進加速に値を加える
		void add_linear_acc(const Vector3& force);

		// 角加速に値を加える
		void add_angula_acc(const Vector3& force);

		// 現在かかっている速度、加速度、力を0にする
		void reset_force() { ALPphysics_ptr->reset_force(); };

		// 速度制限を行う
		void set_max_linear_velocity(const float& max_scalar) { ALPphysics_ptr->set_max_linear_velocity(max_scalar); };
		void set_max_angula_velocity(const float& max_scalar) { ALPphysics_ptr->set_max_angula_velocity(max_scalar); };

		// shapeのアタッチ
		template<typename T>
		T* add_shape() { return ALPcollider_ptr->add_shape<T>(); };

		// meshcolliderのアタッチ
		void add_mesh_shape(const char* filepass, bool is_right_rtiangle = true, bool is_permit_edge_have_many_facet = false);

		// 慣性モーメントをユーザー定義で設定する
		void set_tensor(const Matrix33& tensor) { ALPphysics_ptr->set_tensor(tensor); };

		// 重心をユーザー定義で設定する
		void set_barycenter(const Vector3& cent) { ALPphysics_ptr->set_barycenter(cent); };

		// 現在の慣性モーメントの値
		const Matrix33 get_tensor();

		// 重心のlocal座標を返す
		const Vector3 get_barycenter() const;

		// 衝突したcolliderの配列を返す is_save_contacted_collsがtrueになっていないと衝突したcolliderの情報は保存されない
		std::vector<Contacted_data> get_Contacted_data() const;

	public:
		void Update_hierarchy();

		void awake() override;

		// userが呼ばないように!!
		void finalize() override;

	};
}

#include "shape_box.h"
#include "shape_plane.h"
#include "shape_sphere.h"
#include "shape_capsule.h"