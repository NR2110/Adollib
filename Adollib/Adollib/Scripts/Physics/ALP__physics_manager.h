#pragma once
#include <map>
#include "../Object/component.h"
#include "../Scene/scene.h"

#include "collider__base.h"
#include "ALP_collider.h"
#include "ALP_physics.h"

#include "contact.h"

#include "ALP__shapes.h"

namespace Adollib
{
	class Collider;

	namespace physics_function {
		// physics_managerのstatic変数の初期値
		class physics_manager_default {
		public:
			static constexpr float gravity = 9.8f; //重力

			//static physics_function::ALP_Physics default_physics; //何も設定していないときのpyisicsの値 cppで設定

			static constexpr float bounce_threshold = 0.01f; //跳ね返りの閾値
			static constexpr float sleep_threshold = 0.01f; //sleepの閾値

			static constexpr float contact_max_per_pair = 4; //一つのpairで衝突の解散を行う最大の数
			static constexpr float contact_allowable_error = 0.02f; //これ以上近いと同一衝突点とする
			static constexpr float contact_threrhold_normal = 0.004f; //衝突点の閾値
			static constexpr float contact_threrhold_tangent = 0.02f;//衝突点の閾値

			static constexpr float bias = 0.15f;//めり込みを直す力
			static constexpr float slop = 0.003f;//衝突の許容値

			static constexpr int solver_iterations = 5; //衝突の精度
			static constexpr bool hit_backfaces_flag = false;//meshの後ろから衝突するか

			static constexpr float timeStep = 0.016f;
		};
	}

	class Phyisics_manager
	{
	private:
		static int frame_count;

		//collider_componentのポインタ配列
		static std::unordered_map<Scenelist, std::list<Collider*>> colliders;

		//各dataの実態配列
		static std::unordered_map<Scenelist, std::list<physics_function::ALP_Collider>> ALP_colliders;
		static std::unordered_map<Scenelist, std::list<physics_function::ALP_Physics>> ALP_physicses;

		static std::vector<physics_function::Contacts::Contact_pair> pairs;
		static std::vector<physics_function::Contacts::Collider_2> broad_mid_pair;

		static bool is_changed_coll; //新たにColliderが追加された、削除された場合true
	public:
		static bool is_changed_colliders() { return is_changed_colliders; };

	public:
		static float gravity; //重力

		static physics_function::ALP_Physics default_physics; //何も設定していないときのpyisicsの値

		static float bounce_threshold; //跳ね返りの閾値
		static float sleep_threshold; //sleepの閾値

		static float contact_max_per_pair; //一つのpairで衝突の解散を行う最大の数
		static float contact_allowable_error; //これ以上近いと同一衝突点とする
		static float contact_threrhold_normal; //衝突点の閾値
		static float contact_threrhold_tangent;//衝突点の閾値

		static float bias;//めり込みを直す力
		static float slop;//衝突の許容値

		static int solver_iterations; //衝突の精度
		static bool hit_backfaces_flag;//meshの後ろから衝突するか

		static float timeStep; 

	public:

		static physics_function::ColliderPhysicsShape_itrs add_collider(Collider* coll, Scenelist Sce = Scene::now_scene) {

			physics_function::ColliderPhysicsShape_itrs ret;
			{
				colliders[Sce].emplace_back(coll);
				ret.coll_itr = colliders[Sce].end();
				ret.coll_itr--;
			}
			{
				physics_function::ALP_Collider ALPcollider;
				ALP_colliders[Sce].emplace_back(ALPcollider);
				ret.ALPcollider_itr = ALP_colliders[Sce].end();
				ret.ALPcollider_itr--;
			}
			{
				physics_function::ALP_Physics ALPphysics;
				ALP_physicses[Sce].emplace_back(ALPphysics);
				ret.ALPphysics_itr = ALP_physicses[Sce].end();
				ret.ALPphysics_itr--;
			}

			return ret;
		}

		static void remove_collider(Adollib::Collider* R, Scenelist Sce = Scene::now_scene) {
			ALP_colliders[Sce].erase(R->get_itrs().ALPcollider_itr);
			ALP_physicses[Sce].erase(R->get_itrs().ALPphysics_itr);

			colliders[Sce].erase(R->get_itrs().coll_itr);
		}

	public:

		// 毎フレーム呼ばれる更新処理
		static bool update(Scenelist Sce = Scene::now_scene);

		static void destroy(Scenelist Sce = Scene::now_scene);

	};

}