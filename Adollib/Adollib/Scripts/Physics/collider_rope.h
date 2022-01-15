#pragma once

#include <vector>
#include <assert.h>
#include <string>
#include <unordered_map>
#include <map>
#include "../Math/math.h"
#include "../Object/component.h"

#include "ALP__tags.h"
//#include "ALP_collider.h"
#include "ALP_physics.h"

#include "collider.h"

#include "../Imgui/imgui_all.h"

namespace Adollib {

	namespace Physics_function {
		class ALP_Joint;
		class ALP_Collider;
		class ALP_Physics;
		struct Meshcollider_data;
		namespace Contacts {
			struct Contact_pair;
		}
	}

	class Collider_Rope : public Component {

	public:
		//::: tag ::::::::
		Collider_tagbit tag = 0; //自身のtag(bit)
		Collider_tagbit ignore_tags = 0; //衝突しないtags(bit)

		//::: アタッチするcolliderのphysicsの初期値 ::::
		Physics_data default_physics_data;

		//::: ropeのsphereの個数 ::::
		int sphere_num_size = 5;

		//::: ropeのsphereの半径 ::::
		float sphere_size_r = 0.01f;

		//::: ropeのsphreeの距離 ::::
		float sphree_offset_size = 0.5f;

		//::: ropeの最初の方向 :::
		Vector3 start_rope_dir = Vector3(0, 0, 1);


		//::: static同士で衝突判定を行わない :::
		bool is_static = false;

		//::: 自身の関わるcontact_pairの情報をメンバに保存するかどうか :::
		bool is_save_contacted_colls = false;

	public:
		// 適当なstruct
		enum class Rope_constraint_type {
			sructural_spring, //構成バネ
			bending_spring //曲げバネ
		};

	private:
		std::vector<Collider*> colliders; //頂点毎にアタッチしたcolliderの配列

		std::unordered_map<Rope_constraint_type, std::vector<Joint_base*>> joints; //配置したcolliderの配列

		std::shared_ptr<std::vector<std::pair<Vector3, Vector3>>> vertex_offset; //model頂点からどれくらいずれているか rendererがposition,normalのstd::pairなので合わせる

		std::vector<int> ignore_coll_num;

	public:
		// アタッチされたjointの数
		const int get_joint_count(Rope_constraint_type type);
		// jointのptrを返す
		Joint_base* get_joint_ptr(Rope_constraint_type type, int joint_num);

		// 指定の頂点のphysics_dataを変更する
		void set_vertex_data(const int& vertex_num, const Physics_data& physics_data);
		// 指定の頂点のphysics_dataを得る
		const Physics_data& get_vertex_data(const int& vertex_num) const;

		int get_collider_size() { return colliders.size(); };
		Collider* get_collider(const int& vertex_num) { return colliders[vertex_num]; };

		std::shared_ptr<std::vector<std::pair<Vector3, Vector3>>> get_vertex_offset() { return vertex_offset; };

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
		void reset_force() {for (auto& ptr : colliders)  ptr->reset_force(); };

		// 速度制限を行う
		void set_max_linear_velocity(const float& max_scalar) {for (auto& ptr : colliders)  ptr->set_max_linear_velocity(max_scalar); };
		void set_max_angula_velocity(const float& max_scalar) {for (auto& ptr : colliders)  ptr->set_max_angula_velocity(max_scalar); };

		//void clear_offset_0_joint();
	public:
		void create_rope();

		void Update_hierarchy() override;

		void awake() override;

		void update() override;

		// userが呼ばないように!!
		void finalize() override;

	};





}