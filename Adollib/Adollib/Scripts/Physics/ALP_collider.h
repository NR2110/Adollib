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

		class ALP_Collider {
		public:
			Vector3 local_position;
			Quaternion local_orientation;
			Vector3 local_scale;

			Vector3	world_position;
			Quaternion	world_orientation;
			Vector3	world_scale;

			Quaternion offset_CollGO_quat;
			Vector3 offset_CollGO_pos;

			unsigned int oncoll_bits; //oncollision enterで使用するbit情報

			ALP_Collider_shape shape; //形情報
			
			Collider* coll;

			Meshcoll_data meshcoll_data; //MeshCollider使用時に使うdata vertices,Edges,Facets

		public:

		//oncollision enter
		const bool concoll_enter (const unsigned int tag_name) const;

		//::: 毎フレーム呼ぶもの :::::
		//gameobjectへの変化量を求める
		void solv_resolve();

		//gameobjectへ変化量を渡す
		void resolve_gameobject();

		//gameobjectのtransformからcolliderのworld空間での情報を更新
		void update_world_trans();

		};
	}
}
