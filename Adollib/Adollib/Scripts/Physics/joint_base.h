#pragma once

#include "../Math/math.h"
#include "contact.h"
#include "../Scene/scene_list.h"

namespace Adollib {

	namespace Physics_function {
		class ALP_Joint;

		class Joint_base {
		public:
			Joint_base(Collider* l_colliderA_comp, Collider* l_colliderB_comp, ALP_Joint* l_ALPjoint)
				:  ALPjoint(l_ALPjoint)
			{
				collider_comp[0] = l_colliderA_comp;
				collider_comp[1] = l_colliderB_comp;
			};

			float bias = 0; // 拘束の強さの調整値
			Vector3 anchor[2]; // 剛体のローカル座標系における接続点

		protected:
			Collider* collider_comp[2] = { nullptr }; // 剛体のcomponentへのポインタ

			ALP_Joint* ALPjoint = nullptr;

		public:
			const Collider* get_colliderA() const { return collider_comp[0]; };
			const Collider* get_colliderB() const { return collider_comp[1]; };

			void destroy();
		};



	}
}