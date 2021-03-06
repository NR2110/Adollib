#pragma once

#include "../Math/math.h"
#include "contact.h"


namespace Adollib {

	namespace Physics_function {
		class ALP_Collider;

		class ALP_Joint_base {
		public:

			float bias = 0; // 拘束の強さの調整値
			ALP_Collider* colliderA = nullptr; // 剛体Aへのインデックス
			ALP_Collider* colliderB = nullptr; // 剛体Bへのインデックス
			Vector3 anchorA; // 剛体Aのローカル座標系における接続点
			Vector3 anchorB; // 剛体Bのローカル座標系における接続点
			Contacts::Constraint constraint; // 拘束

		};



	}
}