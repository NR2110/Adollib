#pragma once
#include "../Math/math.h"
#include "contact.h"
#include "../Scene/scene_list.h"

namespace Adollib {

	namespace Physics_function {
		class Joint_base;

		//phasics_managerに保存されている
		class ALP_Joint {

		public:
			ALP_Joint(std::list<ALP_Joint*>::iterator l_this_itr) :this_itr(l_this_itr) {};

		public:
			float bias = 0; // 拘束の強さの調整値
			Vector3 anchor[2]; // 剛体のローカル座標系における接続点

			Collider* collider_comp[2] = { nullptr }; // 剛体のcomponentへのポインタ
			ALP_Collider* ALPcollider[2] = { nullptr }; // 剛体へのポインタ

			Contacts::Constraint constraint; // 拘束計算用


			Joint_base* joint = nullptr; //ユーザー触る情報へのポインタ

		private:
			std::list<ALP_Joint*>::iterator this_itr; //physics_manager内の自身へのイテレータ



		public:
			void reset();

			void adapt_Jointdata();

			void destroy(ALP_Collider* coll_ptr = nullptr, const bool is_delete_userjoint = true);

		};
	}

}