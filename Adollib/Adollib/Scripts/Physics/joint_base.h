#pragma once

#include "../Math/math.h"
#include "../Scene/scene_list.h"

#include <vector>
#include "ALP_anchor.h"

namespace Adollib {
	class Collider;

	namespace Physics_function {
		class ALP_Joint;

		class Joint_base {
		public:
			Joint_base(Collider* l_colliderA_comp, Collider* l_colliderB_comp, ALP_Joint* l_ALPjoint)
				: ALPjoint(l_ALPjoint)
			{
				collider_comp[0] = l_colliderA_comp;
				collider_comp[1] = l_colliderB_comp;
			};

			float bias = 0; // 拘束の強さの調整値
			//std::vector<Vector3> anchor; // 剛体のローカル座標系における接続点
			//Vector3 anchor[2]; // 剛体のローカル座標系における接続点


		//protected:
			Anchor anchors[3]; //接続点(最大3つ)

			int anchor_count = 0; //接続点の数

		protected:
			Collider* collider_comp[2] = { nullptr }; // 剛体のcomponentへのポインタ

			ALP_Joint* ALPjoint = nullptr; // 計算用Jointへのポインタ

		public:
			const Collider* get_colliderA() const { return collider_comp[0]; };
			const Collider* get_colliderB() const { return collider_comp[1]; };

			//anchorの値を派生クラスから更新する
			virtual void adapt_anchor() = 0;

			//jointのtensorへの影響をかえす
			virtual Matrix33 tensor_effect(const Collider* coll) const = 0;

			//limitの影響を返す
			virtual bool limit_effect(Vector3& contactP0, Vector3& contactP1, float& penetrate) const = 0;

			void destroy();
		};



	}
}