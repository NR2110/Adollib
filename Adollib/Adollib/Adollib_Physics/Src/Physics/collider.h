#pragma once

#include <vector>
#include <assert.h>
#include <string>
#include <unordered_map>

#include "ALP__tags.h"
#include "ALP_Collider.h"
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
}

namespace Adollib {
	class Joint_base;

	namespace Physics_function {
		class ALP_Joint;
		class ALP_Collider;
		class ALP_Physics;
		namespace Contacts {
			struct Contact_pair;
		}
	}


	class Collider {

	public:
		//::: tag ::::::::
		Collider_tagbit tag = 0; //自身のtag(bit)
		Collider_tagbit ignore_tags = 0; //衝突しないtags(bit)

		//::: unityのphysics部分 分ける必要なんてないやろ ::::
		Physics_data physics_data;

		//::: 自身の関わるcontact_pairの情報をメンバに保存するかどうか :::
		bool is_save_contacted_colls = false;

		//::: transform(落下方向はworld方向なので このColliderの親情報のinverseを使う)
		DirectX::XMFLOAT3 Wposition;
		DirectX::XMFLOAT4 Worientation;
		DirectX::XMFLOAT3 Wscale;
		DirectX::XMFLOAT4 pearent_Worientation_inverse;

	private:
		Physics_function::ALP_Physics* ALPphysics_ptr = nullptr;
		Physics_function::ALP_Collider* ALPcollider_ptr = nullptr;

	public:
		//::: 後で変更する :::
		const DirectX::XMFLOAT3 linear_velocity() const;
		const DirectX::XMFLOAT3 angula_velocity() const;
		const void linear_velocity(DirectX::XMFLOAT3 v);
		const void angula_velocity(DirectX::XMFLOAT3 v);
		// 指定した一点での速度
		const DirectX::XMFLOAT3 get_point_velocity(const DirectX::XMFLOAT3& pos, bool is_local = false);

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
		void add_force(const DirectX::XMFLOAT3& force, const float& delta_time, const bool& is_force_local = false);

		// 並進移動に力を加える
		void add_force(const DirectX::XMFLOAT3& force, const DirectX::XMFLOAT3& position, const float& delta_time, const bool& is_position_local = false, const bool& is_force_local = false);

		// 角回転に力を加える
		void add_torque(const DirectX::XMFLOAT3& force, const float& delta_time, const bool& is_local = false);

		// 並進加速に値を加える
		void add_linear_acc(const DirectX::XMFLOAT3& force, const float& delta_time);

		// 角加速に値を加える
		void add_angula_acc(const DirectX::XMFLOAT3& force, const float& delta_time);

		// 現在かかっている速度、加速度、力を0にする
		void reset_force();

		// 速度制限を行う
		void set_max_linear_velocity(const float& max_scalar);
		void set_max_angula_velocity(const float& max_scalar);

		// shapeのアタッチ
		template<typename T>
		T* add_shape() { return ALPcollider_ptr->add_shape<T>(); };

		// meshcolliderのアタッチ
		void add_mesh_shape(const char* filepass, bool is_right_rtiangle = true, bool is_permit_edge_have_many_facet = false);

		// 慣性モーメントをユーザー定義で設定する
		void set_tensor(const Physics_function::Matrix33& tensor);

		// 重心をユーザー定義で設定する
		void set_barycenter(const DirectX::XMFLOAT3& cent);

		// 現在の慣性モーメントの値
		const Physics_function::Matrix33 get_tensor();

		// 重心のlocal座標を返す
		const DirectX::XMFLOAT3 get_barycenter() const;

		// 衝突したcolliderの配列を返す is_save_contacted_collsがtrueになっていないと衝突したcolliderの情報は保存されない
		std::vector<Contacted_data> get_Contacted_data() const;

	public:

		//:::::::::
		// Goptr : ユニークな値ならなんでもOK 同じ値を持つものとは衝突しない
		// Wpos, Worient, Wscale : world transform の初期値
		// pearent_Worient_inv : 落下方向はWorld座標系での落下方向なので 変化量の算出時に親の回転のinvをかける
		//:::::::::
		void awake(const void* Goptr, const DirectX::XMFLOAT3& Wpos, const DirectX::XMFLOAT4& Worient, const DirectX::XMFLOAT3& Wscale, const DirectX::XMFLOAT4& pearent_Worient_inv);

		void update();

		void update_Wtransform(const DirectX::XMFLOAT3& Wpos, const DirectX::XMFLOAT4& Worient, const DirectX::XMFLOAT3& Wscale, const DirectX::XMFLOAT4& pearent_Worient_inv);

		//void Update_hierarchy();

		void finalize();

	};
}

#include "shape_box.h"
#include "shape_plane.h"
#include "shape_sphere.h"
#include "shape_capsule.h"