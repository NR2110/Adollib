#pragma once
#include <unordered_map>
#include <string>
#include "../Math/math.h"
#include "../Scene/scene_list.h"

#include "ALP__tags.h"
#include "collider_shape.h"

namespace Adollib {

	class Collider;
	class Gameobject;

	namespace Physics_function {

		class ALP_Physics;
		class Meshcoll_part;
		class ALP_Joint;

		namespace Contacts {
			class Contact_pair;
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
				Scenelist l_scene,
				u_int l_index
			) :
				gameobject(l_go), coll_itr(l_collitr), this_itr(l_itr), ALPphysics(l_ALPphysics), scene(l_scene), index(l_index) {};

		private:
			//::: 自身へのイテレータ(remove用) :::
			std::list<ALP_Collider*>::iterator this_itr;

			//::: このColliderが属しているjointへのポインタ配列 :::
			std::list<ALP_Joint*> joints;

			u_int index = 0; //このcolliderのuniqueなID
			Scenelist scene = Scenelist::scene_null; //このcolldierが存在するscene

			//::: アタッチされたshapeの配列 :::
			std::vector<Collider_shape*> shapes;

			//::: 自身の関わるcontact_pairの情報をメンバに保存するかどうか :::
			bool is_save_pair_ = false;

			//::: is_save_pairがtrueの時 自身の関わるcontact_pairの情報を保存する
			std::vector<std::pair<Contacts::Contact_pair*, u_int>> contacted_colliders;
		public:
			// ここをすべてprivateにしたかった........
			//::: oncoll_enter :::::::
			Collider_tagbit oncoll_check_bits = 0; //on collision enterを行うtagの情報(互いに衝突しないけどoncollenterが必要な場合)
			Collider_tagbit oncoll_bits = 0; //oncollision enterで使用するbit情報


		public:
			// このcolliderのuniqueなID
			const u_int get_index() const { return index; };

			// このcolldierが存在するscene
			const Scenelist get_scene() const { return scene; };

			// on collision enterを行うtagの情報
			const Collider_tagbit get_oncoll_check_bits() const { return oncoll_check_bits; };

			// アタッチされたshapeの配列
			const std::vector<Collider_shape*> get_shapes() const { return shapes; };

			// 自身の関わるpairの情報, u_int : Contact_pair::body[u_int] = 自分
			const std::vector<std::pair<Contacts::Contact_pair*, u_int>>& get_contacted_colliders() const { return contacted_colliders; };

			// 衝突したcolliderのtagを保存
			void add_oncoll_bits(Collider_tagbit bit) { oncoll_bits |= bit; };

			// 保存したtagを0にリセット
			void reset_data_per_frame() {
				oncoll_bits = 0;
				if (is_save_pair_)contacted_colliders.clear();
			};


		private:
			//::: ComponentがアタッチされたColliderへのイテレータ :::
			Collider* coll_itr;

			//::: Physicsへのポインタ :::
			ALP_Physics* ALPphysics = nullptr;

			//::: アタッチされたGOへのポインタ :::
			Gameobject* gameobject = nullptr;

		public:
			Collider* get_collptr() const { return coll_itr; };
			ALP_Physics* get_ALPphysics() const { return ALPphysics; };
			Gameobject* get_gameobject() const { return gameobject; };

		public:

			//::: 毎フレーム呼ぶもの ::::::
			//oncollision enter そのcolliderと接触していたらtrue
			const bool concoll_enter(const unsigned int tag_name);

			// gameobjectのtransformからcolliderのworld空間での情報を更新
			void update_world_trans();

			//座標,姿勢によるworld情報の更新
			void integrate(float duration, Vector3 linear_velocity, Vector3 anglar_velocity);

			//::: 親のtagを獲得 :::::::::::::
			const Collider_tagbit get_tag() const; //自身のtag(bit)
			const Collider_tagbit get_ignore_tags() const; //衝突しないtags

			//:::::::::::::::::::::::::::
			//規定のshapeをアタッチする
			template<typename T>
			T* add_shape() {

				static_assert(std::is_base_of<Collider_shape, T>::value == true, "template T must inherit ALP_shape");

				T* shape = newD T(this);

				shapes.emplace_back(shape);
				return shape;
			};
			Meshcoll_part* add_mesh_shape(const char* filepass, Physics_function::Meshcollider_data* mesh_data);


			//このcolliderが属するjointを追加
			void add_joint(ALP_Joint* joint);

			//jointから外された
			void remove_joint(ALP_Joint* joint);

			//is_save_pairを変更したりしなかったり
			bool is_save_pair() const { return is_save_pair_; };
			bool is_save_pair(bool flag) {
				if (is_save_pair_ == flag)return flag;
				if (is_save_pair_ == false)contacted_colliders.reserve(5); //trueになったとき とりあえず適当な数だけ枠を確保
				else {
					contacted_colliders.clear();
					contacted_colliders.resize(0); //falseになったとき 保存する配列を小さく
					contacted_colliders.shrink_to_fit();
				}
				is_save_pair_ = flag;
				return flag;
			};
			//自身のかかわるpairへのポインタを保存
			void add_contacted_colliders(Contacts::Contact_pair* pair, u_int num) {
				if (!is_save_pair_)return;
				contacted_colliders.emplace_back(std::pair<Contacts::Contact_pair*, u_int>(pair, num));
			};
			//contacted_collidersをリセット
			void clear_contacted_colliders() { contacted_colliders.clear(); };

			//ヒエラルキー描画用
			void Update_hierarchy();

			//managerから自身のremove & shapesのdelete
			void destroy();


		private:
			//::: shapeを包むAABBの更新 ::::::
			//void update_AABB();
		};
	}
}
