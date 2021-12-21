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

namespace Adollib {

	//表示用のphysics_data ユーザーが簡単に変更できるように
	struct Physics_data {
		float inertial_mass = 0; //質量
		float drag = 0; //空気抵抗
		float anglar_drag = 0; //空気抵抗
		float dynamic_friction = 0; //動摩擦
		float static_friction = 0; //静摩擦
		float restitution = 0;	 //反発係数

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

	enum class croth_constraint_type {
		sructural_spring, //構成バネ
		shear_spring, //せん断バネ
		bending_spring //曲げバネ
	};
	struct Croth_constraint {

		int vertId0 = 0; //頂点
		int vertId1 = 0; //頂点

		float natural_length = 0; //ばねの自然長
		croth_constraint_type constraint_type = croth_constraint_type::bending_spring;
	};

	class Collider_Croth : public Component {

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
		std::vector<Collider*> colliders; //頂点毎にアタッチしたcolliderの配列

		std::shared_ptr<std::vector<Vector3>> vertex_offset; //model頂点からどれくらいずれているか

		std::vector<Croth_constraint> croth_constraints; //構成ばね

	public:
		// アタッチされたjointの数
		const int get_joint_count();
		// 指定した番号にアタッチされているjointの情報を得る
		Joint_base* get_joint(const int num);

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
		void reset_force() { for(auto ptr : colliders) ptr->reset_force(); };

		// 速度制限を行う
		void set_max_linear_velocity(const float& max_scalar) { for (auto ptr : colliders) ptr->set_max_linear_velocity(max_scalar); };
		void set_max_angula_velocity(const float& max_scalar) { for (auto ptr : colliders) ptr->set_max_angula_velocity(max_scalar); };

		// shapeのアタッチ
		template<typename T>
		T* add_shape() { return ALPcollider_ptr->add_shape<T>(); };

		// 慣性モーメントをユーザー定義で設定する
		void set_tensor(const Matrix33& tensor) { for (auto ptr : colliders) ptr->set_tensor(tensor); };

		// 重心をユーザー定義で設定する
		void set_barycenter(const Vector3& cent) { for (auto ptr : colliders) ptr->set_barycenter(cent); };

		// 重心のlocal座標を返す
		const Vector3 get_barycenter() const;

	public:
		void load_file(const std::string& filename,bool is_right_rtiangle,bool is_permit_edge_have_many_facet);

		void Update_hierarchy() override;

		void update() override;

		// userが呼ばないように!!
		void finalize() override;

	};
}
