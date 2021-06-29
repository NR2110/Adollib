#pragma once

#include "../Math/math.h"
#include "collider.h"
#include "ALP_joint_base.h"

namespace Adollib {

	namespace Physics_function {
		class ALP_Collider;

		class Joint_data {
		public:
			Joint_data(ALP_Joint_base* base) {

				anchorA = &base->anchorA;
				anchorB = &base->anchorB;
				bias = &base->bias;

				colliderA = base->colliderA;
				colliderB = base->colliderB;
			}

		public:
			//実体のデータへのポインタ
			float* bias = 0; // 拘束の強さの調整値
			Vector3* anchorA = nullptr; // 剛体Aのローカル座標系における接続点
			Vector3* anchorB = nullptr; // 剛体Bのローカル座標系における接続点

		private:
			Physics_function::ALP_Collider* colliderA = nullptr; // 剛体Aへのインデックス
			Physics_function::ALP_Collider* colliderB = nullptr; // 剛体Bへのインデックス

		public:
			Physics_function::ALP_Collider* get_colliderA() { return colliderA; };
			Physics_function::ALP_Collider* get_colliderB() { return colliderB; };

			void destroy();
		};

		class Joint {
			static Joint_data add_balljoint(
				Collider* colliderA, Vector3 anchorA,
				Collider* colliderB, Vector3 anchorB,
				float bias = 1
				);
		};



	}
}