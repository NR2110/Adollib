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
		class ALP_Collider;

		struct ALP_Collider_mesh {

			//14-DOP
			DOP::DOP_14	dop14;	//14DOP データ

			//meshcollider用 vertexes,Edges,Facetsなどの情報
			std::vector<physics_function::Meshcollider_mesh>::iterator mesh;

			//親
			std::list<ALP_Collider>::iterator ALPcollider;

			////meshの番号(ふ)
			//u_int mesh_num;
		};

		class ALP_Collider {
		public:
			//::: Collider :::::
			Vector3 local_position;
			Quaternion local_orientation;
			Vector3 local_scale;

			Vector3	world_position;
			Quaternion	world_orientation;
			Vector3	world_scale;

			Vector3 half_size; //実際のsize = half_size * local_scale * GO.world_scale

			//::: GOに渡すためのバッファ :::::::
			Quaternion offset_CollGO_quat;
			Vector3 offset_CollGO_pos;

			//::: oncoll_enter :::::::
			u_int oncoll_bits = 0; //oncollision enterで使用するbit情報
			u_int oncoll_check_bits = 0; //on collision enterを行うtagの情報(互いに衝突しないけどoncollenterが必要な場合)

			//::: tag関係 :::::::::::::
			u_int tag = 0; //自身のtag(bit)
			u_int not_hitable_tags = 0; //衝突しないtags

			//:::
			ALP_Collider_shape shape = ALP_Collider_shape::None; //形情報

			//::: ComponentがアタッチされたColliderへのイテレータ :::
			std::list<Collider*>::iterator coll_itr;

			//::: Physicsへのイテレータ :::
			std::list<ALP_Physics>::iterator ALPphysics;

			//::: 複数meshに対応 各メッシュのdop,頂点情報などがここに保存される :::
			std::vector<ALP_Collider_mesh> collider_meshes;

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

			//::: collider:Component の local_positionなどが変更されたときに呼ぶもの :::
			// collider::get_Colliderdata()からlocal_posなどの情報を更新
			void refresh_ALP_from_data();

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
