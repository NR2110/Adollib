#pragma once
#include <unordered_map>
#include <string>
#include "../Math/math.h"
#include "../Scene/scene_list.h"

#include "ALP__tags.h"
#include "collider_shape.h"
#include "ALP_world_transform.h"

#include <mutex>

namespace Adollib {

	class Collider;
	class Gameobject;

	namespace Physics_function {

		class ALP_Physics;
		class Meshcoll_part;
		class ALP_Joint;

		namespace Contacts {
			struct Contact_pair;
		}

		//形情報
		class ALP_Collider {
		public:
			//コンストラクタ
			ALP_Collider(
				Gameobject* l_go,
				Collider* l_collitr,
				std::list<ALP_Collider*>::iterator l_itr,
				ALP_Physics* l_ALPphysics,
				const Scenelist l_scene,
				const u_int l_index
			) :
				this_itr(l_itr), index(l_index), scene(l_scene), coll_ptr(l_collitr), ALPphysics(l_ALPphysics), gameobject(l_go) {};

		private:
			//::: ComponentがアタッチされたColliderへのイテレータ :::
			Collider* coll_ptr = nullptr;

			//::: Physicsへのポインタ :::
			ALP_Physics* ALPphysics = nullptr;

			//::: アタッチされたGOへのポインタ :::
			Gameobject* gameobject = nullptr;

			//::: 自身へのイテレータ(remove用) :::
			std::list<ALP_Collider*>::iterator this_itr{};

			//::: このColliderが属しているjointへのポインタ配列 :::
			std::list<ALP_Joint*> joints{};

			u_int index = 0; //このcolliderのuniqueなID
			Scenelist scene = Scenelist::scene_null; //このcolliderが存在するscene

			//::: アタッチされたshapeの配列 :::
			std::list<Collider_shape*> shapes{};
			std::list<Collider_shape*> added_shapes{}; // マルチスレッド用 処理の途中で追加された要素

			//
			std::mutex mtx;

		public:
			// privateにしたかった........
			//::: oncoll_enter :::::::
			Collider_tagbit oncoll_check_bits = 0; //on collision enterを行うtagの情報(互いに衝突しないけどoncollenterが必要な場合)
			Collider_tagbit oncoll_bits = 0; //oncollision enterで使用するbit情報

			//::: tag ::::::::
			Collider_tagbit tag = 0; //自身のtag(bit)
			Collider_tagbit ignore_tags = 0; //衝突しないtags(bit)

		public:
			Collider* get_collptr() const { return coll_ptr; };
			ALP_Physics* get_ALPphysics() const { return ALPphysics; };
			//Gameobject* get_gameobject() const { return gameobject; };

			// ALPphysics_ptrのset
			void set_ALPphysics_ptr(ALP_Physics* ptr) { ALPphysics = ptr; };

			// マルチスレッド化するにあたり、add_colliderした時点ではメインのlistに入れずbufferのlistに入れるため 自身のitrが生成時に決まらないため set関数を準備
			void set_this_itr(std::list<ALP_Collider*>::iterator itr) { this_itr = itr; };

		public:
			//[[nodiscard]]

			// このcolliderのuniqueなID
			u_int get_index() const { return index; };

			// このcolliderが存在するscene
			Scenelist get_scene() const { return scene; };

			// on collision enterを行うtagの情報
			Collider_tagbit get_oncoll_check_bits() const { return oncoll_check_bits; };

			std::list<ALP_Joint*> get_joints() const { return joints; };

			// アタッチされたshapeの配列
			std::list<Collider_shape*> get_shapes() const { return shapes; };
			std::list<Collider_shape*> get_added_shapes() const { return added_shapes; };

			// 衝突したcolliderのtagを保存
			void add_oncoll_bits(Collider_tagbit bit) { oncoll_bits |= bit; };

		public:
			//::: マルチスレッドにするためtransformのworld情報を保存する :::
			world_trans transform_start; //初めの値
			world_trans transform; //計算している値 (初めの値との差を移動量としてgameobject.transformに入れる)
			world_trans transform_for_GO; //gameobjectへ渡すためのtransform

			//::: アタッチされたgameobjectが削除されたとき trueにする 別スレッドなので削除するタイミングが異なるから
			bool is_deleted = false;

			//::: addedの配列からメインの配列にadaptされたらtrueにする (itrがaddedを指すのかmainの配列を刺すのかわからないため)
			bool is_added = false;

		public:
			// added_dataをmainのdata配列に引っ越す
			void adapt_added_data() {
				if (added_shapes.size() == 0)return;

				std::lock_guard <std::mutex> lock(mtx);

				shapes.splice(shapes.end(), std::move(added_shapes));

				added_shapes.clear();
			};


		public:

			//::: 毎フレーム呼ぶもの ::::::
			//oncollision enter そのcolliderと接触していたらtrue
			const bool concoll_enter(const unsigned int tag_name);

			// gameobjectのtransformからcolliderのworld空間での情報を更新
			void update_world_trans();

			//座標,姿勢によるworld情報の更新
			void integrate(float duration, const Vector3& linear_velocity, const Vector3& angula_velocity, const Vector3& old_linear_velocity,const Vector3& old_angula_velocity);

			// main_threadが更新したフレームに呼ばれる
			void reset_data_per_frame();

			// 毎フレーム行う collider::componentのtagなどのデータをコピーする
			void adapt_collider_component_data();

			//::: collider::componentのtagを獲得 :::::::::::::
			Collider_tagbit get_tag() const; //自身のtag(bit)
			Collider_tagbit get_ignore_tags() const; //衝突しないtags

			//:::::::::::::::::::::::::::
			// 規定のshapeをアタッチする
			template<typename T>
			T* add_shape() {
				std::lock_guard <std::mutex> lock(mtx);

				static_assert(std::is_base_of<Collider_shape, T>::value == true, "template T must inherit ALP_shape");

				T* shape = newD T(this);

				added_shapes.emplace_back(shape);
				//adapt_added_data();
				return shape;
			};
			Meshcoll_part* add_mesh_shape(const char* filepath, Physics_function::Meshcollider_data* mesh_data);

			// gameobjectのtransformからcolliderのworld空間での情報を更新
			void update_world_trans_contain_added();

			// このcolliderが属するjointを追加
			void add_joint(ALP_Joint* joint);

			// jointから外された
			void remove_joint(ALP_Joint* joint);

			// 衝突情報を保存
			void add_contacted_collider(const Contacts::Contact_pair* pair, u_int num) const;


			//::: mainthreadから呼ばれる :::
			// gameobjectのtransformへ自身の保持するtransformを入れる
			void adapt_to_gameobject_transform() const;

			void adapt_transform_for_GO();

			// ヒエラルキー描画用
			void Update_hierarchy();

			// managerから自身のremove,shapeの削除,jointの削除
			void destroy();


		private:

		};
	}
}
