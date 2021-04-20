#pragma once
#include <unordered_map>
#include <string>
#include "../Math/math.h"
#include "ALP__tags.h"
#include "ALP__shapes.h"
#include "ALP__meshcoll_data.h"

#include "../Scene/scene_list.h"

namespace Adollib {

	class Collider;

	namespace Physics_function {

		class ALP_Physics;
		class ALP_Collider;

		struct ALP_Collider_mesh {

			//14-DOP
			DOP::DOP_14	dop14;	//14DOP データ

			//meshcollider用 vertexes,Edges,Facetsなどの情報
			std::vector<Physics_function::Meshcollider_mesh>::iterator mesh;

			//親
			std::list<ALP_Collider>::iterator ALPcollider;
		};

		class ALP_Collider {
		public:
			u_int index = 0; //このcolliderの番号
			Scenelist scene = Scenelist::scene_null; //このcolldierが存在するscene

		public:
			//::: Collider :::::
			Vector3 local_position;
			Quaternion local_orientation;
			Vector3 local_scale;

		private:
			Vector3	world_position_;
			Quaternion	world_orientation_;
			Vector3	world_scale_;

			Vector3 old_world_position_;
			Quaternion old_world_orientation_;
			Vector3 old_world_scale_;

		public:
			const Vector3	world_position()const { return world_position_; };
			const Quaternion	world_orientation()const {return world_orientation_;};
			const Vector3	world_scale()const { return world_scale_; };

			const Vector3	old_world_position()const { return old_world_position_; };
			const Quaternion	old_world_orientation()const { return old_world_orientation_; };
			const Vector3	old_world_scale()const { return old_world_scale_; };
		public:

			Vector3 half_size; //実際のsize = half_size * local_scale * GO.world_scale

			//::: oncoll_enter :::::::
			u_int oncoll_bits = 0; //oncollision enterで使用するbit情報
			u_int oncoll_check_bits = 0; //on collision enterを行うtagの情報(互いに衝突しないけどoncollenterが必要な場合)

			//::: tag関係 :::::::::::::
			u_int tag = 0; //自身のtag(bit)
			u_int nohit_tag = 0; //衝突しないtags

			//:::
			ALP_Collider_shape shape = ALP_Collider_shape::None; //形情報

			//::: 複数meshに対応 各メッシュのdop,頂点情報などがここに保存される :::
			std::vector<ALP_Collider_mesh> collider_meshes;

			//::: GOに渡すためのバッファ :::::::
			Quaternion offset_CollGO_quat;
			Vector3 offset_CollGO_pos;

			//:::
			Meshcoll_data meshcoll_data; //MeshCollider使用時に使うdata vertices,Edges,Facets

			//::: ComponentがアタッチされたColliderへのイテレータ :::
			std::list<Collider*>::iterator coll_itr;

			//::: Physicsへのイテレータ :::
			std::list<ALP_Physics>::iterator ALPphysics;

			//::: 自身へのイテレータ :::
			std::list<ALP_Collider>::iterator ALPcollider;
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

		private:
			void update_dop14_as_sphere();
			void update_dop14_as_box();
			void update_dop14_as_capsule();
			void update_dop14_as_plane();
			void update_dop14_as_mesh();
		};
	}
}
