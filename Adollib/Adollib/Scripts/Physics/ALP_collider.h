#pragma once
#include <unordered_map>
#include <string>
#include "../Math/math.h"
#include "../Object/gameobject_tags.h"
#include "collider__base.h"

namespace Adollib {
	namespace physics_function {

		class ALP_Collider {
		public:
			Vector3 local_position;
			Vector3 local_orientation;
			Vector3 local_scale;

			Vector3	world_position;
			Vector3	world_prientation;
			Vector3	world_scale;

			Quaternion offset_CollGO_quat;
			Vector3 offset_CollGO_pos;

			std::unordered_map<GO_Tag, bool>concollflags;

			Collider* coll;


		private:

		//on collision enter
		bool concoll_enter(GO_Tag tag_name);

		//座標,姿勢の更新
		void integrate(float duration = 1);

		//gameobjectへの変化量を求める goをALP_colliderが持っていないためここに記述
		void solv_resolve();

		//gameobjectへ変化量を渡す goをALP_colliderが持っていないためここに記述
		void resolve_gameobject();

		//gameobjectのtransformからcolliderのworld空間での情報を更新 goをALP_colliderが持っていないためここに記述
		virtual void update_world_trans() = 0;

		//gameobjectのtransformからcolliderのworld空間での情報を更新
		void update_world_trans();

		//サイズ変更などに対応するため毎フレーム慣性テンソルなどを更新
		void update_inertial(const Vector3& size, float density = 1);

		};
	}
}
