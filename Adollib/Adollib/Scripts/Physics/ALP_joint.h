#pragma once
#include "../Math/math.h"
#include "../Scene/scene_list.h"
#include "ALP_contact.h"
#include "ALP_anchor.h"

#include "ALP_struct_world_transform.h"

namespace Adollib {
	class Joint_base;

	namespace Physics_function {

		//physics_managerに保存されている
		class ALP_Joint {

		public:
			explicit ALP_Joint(const std::list<ALP_Joint*>::iterator l_this_itr) :this_itr(l_this_itr) {};

		public:
			float shrink_bias = 0.1f; // 拘束の強さの調整値(縮)
			float stretch_bias = 0.1f; // 拘束の強さの調整値(伸)
			float slop = 0; // 拘束の許容誤差
			float offset = 0; //拘束の場所のずれ (ex. 布はanchor = 0,0 offset = 頂点の距離 で実装している)
			float limit_bias = 0;

			int anchor_count = 0; //anchorがいくつあるか
			const Anchor* anchor = nullptr; // 剛体のローカル座標系における接続点

			//Collider* collider_comp[2] = { nullptr }; // 剛体のcomponentへのポインタ
			ALP_Collider* ALPcollider[2] = { nullptr }; // 剛体へのポインタ

			Contacts::Constraint constraint[3]; // anchor拘束計算用

			Vector3 limit_constraint_pos[2]; //limit拘束計算用 拘束点
			Contacts::Constraint constraint_limit; // limit拘束計算用

			Adollib::Joint_base* userjoint = nullptr; //ユーザーの触る情報へのポインタ

			//::: addedの配列からメインの配列にadaptされたらtrueにする (itrがaddedを指すのかmainの配列を刺すのかわからないため)
			bool is_added = false;

		private:
			std::list<ALP_Joint*>::iterator this_itr{}; //physics_manager内の自身へのイテレータ

		public:
			void set_this_itr(std::list<ALP_Joint*>::iterator itr) { this_itr = itr; };


		public:
			void reset();

			void adapt_Jointdata();

			void destroy(ALP_Collider* coll_ptr = nullptr);

		};
	}

}