#pragma once
#include <unordered_map>
#include <string>
#include "../Math/math.h"
#include "ALP__tags.h"
#include "ALP__meshcoll_data.h"

#include "../Scene/scene_list.h"
#include "collider_shape.h"

namespace Adollib {

	class Collider;
	class Gameobject;

	namespace Physics_function {

		class ALP_Physics;
		class ALP_Collider;
		class Meshcoll_part;
		struct ALP_Solverbody;

		class ALP_Collider {
		public:
			//コンストラクタ
			ALP_Collider(
				Gameobject* l_go, std::list<Collider*>::iterator l_collitr, std::list<ALP_Collider*>::iterator l_itr, ALP_Physics* l_ALPphysics, Scenelist l_scene, u_int l_index
			) :
				gameobject(l_go), coll_itr(l_collitr), this_itr(l_itr), ALPphysics(l_ALPphysics), scene(l_scene), index(l_index) {};

		private:
			//::: GOに渡すためのバッファ :::::::
			Quaternion buffer_quat_chang;
			Vector3 buffer_pos_chang;

			//::: 自身へのイテレータ(remove用) :::
			std::list<ALP_Collider*>::iterator this_itr;

		public:
			u_int index = 0; //このcolliderの番号
			Scenelist scene = Scenelist::scene_null; //このcolldierが存在するscene

			//::: アタッチされたshapeの配列 :::
			std::vector<Collider_shape*> shapes;

			//::: oncoll_enter :::::::
			Collider_tagbit oncoll_check_bits = 0; //on collision enterを行うtagの情報(互いに衝突しないけどoncollenterが必要な場合)
			Collider_tagbit oncoll_bits = 0; //oncollision enterで使用するbit情報

		public:
			//このcolliderの番号
			const u_int get_index() const { return index; };

			//このcolldierが存在するscene
			const Scenelist get_scene() const { return scene; };

			//on collision enterを行うtagの情報
			const Collider_tagbit get_oncoll_check_bits() const { return oncoll_check_bits; };

			//アタッチされたshapeの配列
			const std::vector<Collider_shape*> get_shapes() const { return shapes; };

			//衝突したcolliderのtagを保存
			void add_oncoll_bits(Collider_tagbit bit) { oncoll_bits |= bit; };

			//保存したtagを0にリセット
			void reset_oncoll_bits() { oncoll_bits = 0; };



		private:
			//::: ComponentがアタッチされたColliderへのイテレータ :::
			std::list<Collider*>::iterator coll_itr;

			//::: Physicsへのポインタ :::
			ALP_Physics* ALPphysics = nullptr;

			//::: アタッチされたGOへのポインタ :::
			Gameobject* gameobject = nullptr;
		public:
			std::list<Collider*>::iterator get_collitr() const { return coll_itr; };
			ALP_Physics* get_ALPphysics() const { return ALPphysics; };
			Gameobject* get_gameobject() const { return gameobject; };
		public:

			//oncollision enter
			const bool concoll_enter(const unsigned int tag_name);

			//::: 毎フレーム呼ぶもの :::::
			//gameobjectへの変化量を求める
			void solv_resolve();

			// gameobjectへ変化量を渡す
			void resolve_gameobject();

			// gameobjectのtransformからcolliderのworld空間での情報を更新
			void update_world_trans();

			//::: collider:Component の local_positionなどが変更されたときに呼ぶもの :::
			// DOP_14データの更新
			void update_dop14();

			//座標,姿勢によるworld情報の更新
			void integrate(float duration, Vector3 linear_velocity, Vector3 anglar_velocity);

			void set_inertial_mass(const Matrix& m);

			////::: 親のtagを獲得 :::::::::::::
			const Collider_tagbit get_tag() const; //自身のtag(bit)
			const Collider_tagbit get_ignore_tags() const; //衝突しないtags

			//:::::::::::::::::::::::::::
			//規定のshapeをアタッチする
			template<typename T>
			T* add_shape() {

				static_assert(std::is_base_of<Collider_shape, T>::value == true, "template T must inherit ALP_shape");

				T* shape = newD T(this);

				ALPphysics->add_tensor_type(shape->get_tensor_type());

				shapes.emplace_back(shape);
				return shape;
			};

			Meshcoll_part* add_mesh_shape(const char* filepass, Physics_function::Meshcollider_data* mesh_data);

			//ヒエラルキー描画用
			void Update_hierarchy();

			//managerからクラスの削除 & shapesのdelete
			void destroy();


			//private:
			//	void update_dop14_as_sphere();
			//	void update_dop14_as_box();
			//	void update_dop14_as_capsule();
			//	void update_dop14_as_plane();
			//	void update_dop14_as_mesh();
		};
	}
}
