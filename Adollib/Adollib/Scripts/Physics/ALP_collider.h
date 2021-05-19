#pragma once
#include <unordered_map>
#include <string>
#include "../Math/math.h"
#include "ALP__tags.h"
#include "ALP__meshcoll_data.h"

#include "../Scene/scene_list.h"
#include "ALP__shapes.h"

namespace Adollib {

	class Collider;
	class Gameobject;

	namespace Physics_function {

		class ALP_Physics;
		class ALP_Collider;

		class Meshcoll_part;

		enum class ALP_Collider_inertial_shape {
			box,
			sphere
		};


		class ALP_Collider {
		public:
			//コンストラクタ
			ALP_Collider(std::list<ALP_Collider*>::iterator l_itr, Scenelist l_scene, u_int l_index)
				: this_itr(l_itr), scene(l_scene), index(l_index) {};

		private:
			//::: GOに渡すためのバッファ :::::::
			Quaternion offset_CollGO_quat;
			Vector3 offset_CollGO_pos;

			//::: 自身へのイテレータ(remove用) :::
			std::list<ALP_Collider*>::iterator this_itr;

			u_int index = 0; //このcolliderの番号
			Scenelist scene = Scenelist::scene_null; //このcolldierが存在するscene

		public:
			//このcolliderの番号
			const u_int get_index() const { return index; };

			//このcolldierが存在するscene
			const Scenelist get_scene() const { return scene; };

		public:
			//::: oncoll_enter :::::::
			u_int oncoll_bits = 0; //oncollision enterで使用するbit情報
			u_int oncoll_check_bits = 0; //on collision enterを行うtagの情報(互いに衝突しないけどoncollenterが必要な場合)

			//::: tag関係 :::::::::::::
			u_int tag = 0; //自身のtag(bit)
			u_int nohit_tag = 0; //衝突しないtags

			//::: アタッチされたshapeの配列 :::
			std::vector<ALP_shape*> shapes;

			////::: 型情報 :::
			ALP_Collider_inertial_shape shape = ALP_Collider_inertial_shape::box;

			//::: 子オブジェクトの影響を受けるか 影響を受ける場合,子オブジェクトの相対座標などは変更されない :::
			bool is_chiled_relative_fixed = false;

			//::: ComponentがアタッチされたColliderへのイテレータ :::
			std::list<Collider*>::iterator coll_itr;

			//::: Physicsへのポインタ :::
			ALP_Physics* ALPphysics;

			//::: 自身へのポインタ :::
			ALP_Collider* ALPcollider;

			//::: アタッチされたGOへのポインタ :::
			Gameobject* gameobject = nullptr;
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

			// collider::get_Colliderdata()からlocal_posなどの情報を更新
			void refresh_ALP_from_data();

			//::: collider:Component の local_positionなどが変更されたときに呼ぶもの :::
			// DOP_14データの更新
			void update_dop14();

			//座標,姿勢によるworld情報の更新
			void integrate(float duration , Vector3 linear_velocity, Vector3 anglar_velocity);



			//:::::::::::::::::::::::::::
			//規定のshapeをアタッチする
			template<typename T>
			T* add_shape() {
				//if (dynamic_cast<ALP_shape>(T) == nullptr) assert("ALP_shapeを継承したclassをアタッチしてください");

				T* shape = new T;

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
