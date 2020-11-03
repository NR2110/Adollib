#pragma once
#include <unordered_map>
#include <string>
#include "../Math/math.h"
#include "ALP_tags.h"

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

			std::unordered_map<ALP_tags, bool> oncoll_checkmap; //on collision enterのflag保存
			std::vector<ALP_tags> oncoll_enter_names; //on collision enterのtag保存

			ALP_Collider_shape shape; //形情報
			
			Collider* coll;


		public:

		//on collision enter
		bool concoll_enter(ALP_tags tag_name);

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
