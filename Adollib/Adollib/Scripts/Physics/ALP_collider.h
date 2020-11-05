#pragma once
#include <unordered_map>
#include <string>
#include "../Math/math.h"
#include "ALP__tags.h"
#include "ALP__shapes.h"
#include "ALP__meshcoll_data.h"

namespace Adollib {

	class Collider;

	namespace physics_function {

		class ALP_Physics;

		class ALP_Collider {
		public:
			//::: Collider :::::
			Vector3 local_position;
			Quaternion local_orientation;
			Vector3 local_scale;

			Vector3	world_position;
			Quaternion	world_orientation;
			Vector3	world_scale;

			Vector3 half_size;

			//::: GOに渡すためのバッファ :::::::
			Quaternion offset_CollGO_quat;
			Vector3 offset_CollGO_pos;

			//::: oncoll_enter :::::::
			u_int oncoll_bits; //oncollision enterで使用するbit情報
			u_int oncoll_check_bits; //on collision enterを行うtagの情報(互いに衝突しないけどoncollenterが必要な場合)

			//::: tag関係 :::::::::::::
			u_int tag; //自身のtag(bit)
			u_int not_hitable_tags; //衝突しないtags

			//::: 14-DOP ::::::::::
			DOP::DOP_14	dop14;	//14DOP データ
			DOP::DOP_14	dopbase; //meshcollider用 読み込み時のdopデータ meshcoliderはこれを基準にDOPを生成

			//:::
			ALP_Collider_shape shape; //形情報

			//::: ComponentがアタッチされたColliderへのポインタ :::
			Collider* coll;

			//::: Physicsへのポインタ :::
			ALP_Physics* ALPphysics;

			//:::
			Meshcoll_data meshcoll_data; //MeshCollider使用時に使うdata vertices,Edges,Facets

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

		public:
			// DOP_14データの更新
			void update_dop14();

		private:
			void update_dop14_as_sphere();
			void update_dop14_as_box();
			void update_dop14_as_plane();
			void update_dop14_as_mesh();
		};
	}
}
