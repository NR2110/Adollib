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

		//座標,姿勢の更新
		void integrate(float duration = 1);

		//gameobjectへの変化量を求める
		void solv_resolve();

		//gameobjectへ変化量を渡す
		void resolve_gameobject();

		//gameobjectのtransformからcolliderのworld空間での情報を更新
		void update_world_trans();

		//サイズ変更などに対応するため毎フレーム慣性テンソルなどを更新
		void update_inertial(const Vector3& size, float density = 1);

		//オブジェクトが動くたびに呼び出す　のが効率よいが適当に毎フレーム呼ぶ
		//DOP_14データの更新
		void update_dop14();

		};
	}
}
