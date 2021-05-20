#pragma once

#include <map>
#include "../Object/component.h"
#include "../Object/gameobject.h"
#include "../Scene/scene.h"

#include "collider.h"
#include "ALP_collider.h"
#include "ALP_physics.h"

#include "ALP_broadphase.h"

#include "contact.h"

#include "ALP__shapes.h"

namespace Adollib
{
	class Collider;

	namespace Physics_function {
		// physics_managerのstatic変数の初期値
		struct PhysicsParams_default {
		public:

			static constexpr float gravity = 9.8f; //重力

			static constexpr float bounce_threshold = 0.01f; //跳ね返りの閾値
			static constexpr float sleep_threshold = 0.01f; //sleepの閾値

			static constexpr float contact_allowable_error = 0.02f; //これ以上近いと同一衝突点とする
			static constexpr float contact_threrhold_normal = 0.004f; //衝突点の閾値
			static constexpr float contact_threrhold_tangent = 0.02f;//衝突点の閾値



			static constexpr float bias = 0.15f;//めり込みを直す力
			static constexpr float slop = 0.003f;//衝突の許容値

			static constexpr int solver_iterations = 5; //衝突の精度
			static constexpr bool hit_backfaces_flag = false;//meshの後ろから衝突するか

			static constexpr float timeStep = 0.016f;

			//::: Physicsの初期値部分 :::

			static constexpr float inertial_mass = 1.f; //質量
			static constexpr float linear_drag = 0.1f; //空気抵抗
			static constexpr float anglar_drag = 0.1f; //空気抵抗
			static constexpr float dynamic_friction = 0.4f;//動摩擦
			static constexpr float static_friction = 0.4f; //静摩擦
			static constexpr float restitution = 0.1f;	 //反発係数

			static constexpr bool is_fallable = true; //落ちるか
			static constexpr bool is_kinmatic_anglar = true; //ほかの物体からの影響で回転速度が変化しない
			static constexpr bool is_kinmatic_linear = true; //ほかの物体からの影響で並進速度が変化しない
			static constexpr bool is_moveable = true;//動くか
			static constexpr bool is_hitable = true;  //衝突しない
			//::::::::
		};

		struct PhysicsParams {
		public:
			float gravity = 9.8f; //重力

			float bounce_threshold = 0.01f; //跳ね返りの閾値
			float sleep_threshold = 0.01f; //sleepの閾値

			float contact_allowable_error = 0.02f; //これ以上近いと同一衝突点とする
			float contact_threrhold_normal = 0.004f; //衝突点の閾値
			float contact_threrhold_tangent = 0.02f;//衝突点の閾値



			float bias = 0.15f;//めり込みを直す力
			float slop = 0.003f;//衝突の許容値

			int solver_iterations = 5; //衝突の精度
			bool hit_backfaces_flag = false;//meshの後ろから衝突するか

			float timeStep = 0.016f;

			//::: Physicsの初期値部分 :::

			float inertial_mass = 1.f; //質量
			float linear_drag = 0.1f; //空気抵抗
			float anglar_drag = 0.1f; //空気抵抗
			float dynamic_friction = 0.4f;//動摩擦
			float static_friction = 0.4f; //静摩擦
			float restitution = 0.1f;	 //反発係数

			bool is_fallable = true; //落ちるか
			bool is_kinmatic_anglar = true; //ほかの物体からの影響で回転速度が変化しない
			bool is_kinmatic_linear = true; //ほかの物体からの影響で並進速度が変化しない
			bool is_moveable = true;//動くか
			bool is_hitable = true;  //衝突しない
		};
	}

	class Phyisics_manager
	{
	private:
		static int frame_count;

		//collider_componentのポインタ配列
		static std::unordered_map<Scenelist, std::list<Collider*>> colliders;

		static std::unordered_map<Scenelist, u_int> collider_index_count;

		//各dataの実態配列
		static std::unordered_map<Scenelist, std::list<Physics_function::ALP_Collider*>> ALP_colliders;
		static std::unordered_map<Scenelist, std::list<Physics_function::ALP_Physics*>> ALP_physicses;

		static std::vector<Physics_function::Contacts::Contact_pair> pairs;
		static std::vector<Physics_function::Contacts::Collider_2> broad_mid_pair;

		//そのフレーム間で変化したもの(broad_phase用)
		static std::unordered_map<Scenelist, std::vector<Physics_function::ALP_Collider*>> moved_collider;   //動いた
		static std::unordered_map<Scenelist, std::vector<Physics_function::ALP_Collider*>> added_collider;   //追加された

	public:
		//生成時のphysicsの値
		static Physics_function::PhysicsParams physicsParams;

		//static float gravity; //重力

		//static float bounce_threshold; //跳ね返りの閾値
		//static float sleep_threshold; //sleepの閾値

		//static float contact_allowable_error; //これ以上近いと同一衝突点とする
		//static float contact_threrhold_normal; //衝突点の閾値
		//static float contact_threrhold_tangent;//衝突点の閾値

		//static float bias;//めり込みを直す力
		//static float slop;//衝突の許容値

		//static int solver_iterations; //衝突の精度
		//static bool hit_backfaces_flag;//meshの後ろから衝突するか

		//static float timeStep;

		//static float inertial_mass; //質量
		//static float linear_drag; //空気抵抗
		//static float anglar_drag; //空気抵抗
		//static float dynamic_friction;//動摩擦
		//static float static_friction; //静摩擦
		//static float restitution;	 //反発係数

		//static bool is_fallable; //落ちるか
		//static bool is_kinmatic_anglar; //ほかの物体からの影響で回転速度が変化しない
		//static bool is_kinmatic_linear; //ほかの物体からの影響で並進速度が変化しない
		//static bool is_moveable;//動くか
		//static bool is_hitable;  //衝突しない


		static bool is_draw_collider; //COlliderの表示
		static bool is_draw_dop; //dopの表示(14-DOPを表示するのがめんどくさいためAABBを表示する)

	public:

		static Physics_function::ColliderPhysics_ptrs add_collider(Collider* coll) {

			Scenelist Sce = coll->gameobject->get_scene();

			Physics_function::ColliderPhysics_ptrs ret;
			{
				colliders[Sce].emplace_back(coll);
				ret.coll_itr = colliders[Sce].end();
				ret.coll_itr--;
			}
			{
				//itrがほしいため空のポインタで枠だけ取る
				Physics_function::ALP_Collider* null = nullptr;
				ALP_colliders[Sce].emplace_back(null);

				auto itr = ALP_colliders[Sce].end();
				itr--;

				//中身を入れつつ生成
				*itr = D_new Physics_function::ALP_Collider(coll->gameobject, itr, Sce, collider_index_count[Sce]);

				ret.ALPcollider_ptr = *itr;
			}

			{
				//itrがほしいため空のポインタで枠だけ取る
				Physics_function::ALP_Physics* null = nullptr;
				ALP_physicses[Sce].emplace_back(null);

				auto itr = ALP_physicses[Sce].end();
				itr--;

				//中身を入れつつ生成
				*itr = D_new Physics_function::ALP_Physics(coll->gameobject, itr, Sce, collider_index_count[Sce]);

				//phsicsの初期値の入力
				(*itr)->set_default();

				ret.ALPphysics_ptr = *itr;
			}

			ret.ALPcollider_ptr->ALPphysics = ret.ALPphysics_ptr;
			ret.ALPphysics_ptr->ALPcollider = ret.ALPcollider_ptr;

			//追加されたcollider
			added_collider[Sce].emplace_back(ret.ALPcollider_ptr);

			//::: 初期値をいれる :::
			(*ret.coll_itr)->physics_data.inertial_mass =	physicsParams.inertial_mass; //質量
			(*ret.coll_itr)->physics_data.drag =			physicsParams.linear_drag; //空気抵抗
			(*ret.coll_itr)->physics_data.anglar_drag =		physicsParams.anglar_drag; //空気抵抗
			(*ret.coll_itr)->physics_data.dynamic_friction = physicsParams.dynamic_friction; //動摩擦
			(*ret.coll_itr)->physics_data.static_friction = physicsParams.static_friction; //静摩擦
			(*ret.coll_itr)->physics_data.restitution =		physicsParams.restitution;	 //反発係数

			(*ret.coll_itr)->physics_data.is_fallable =		physicsParams.is_fallable; //落ちない
			(*ret.coll_itr)->physics_data.is_kinmatic_anglar = physicsParams.is_kinmatic_anglar;//影響うけない(fallはする)
			(*ret.coll_itr)->physics_data.is_kinmatic_linear = physicsParams.is_kinmatic_linear;//影響うけない(fallはする)
			(*ret.coll_itr)->physics_data.is_moveable =		physicsParams.is_moveable; //動かない
			(*ret.coll_itr)->physics_data.is_hitable =		physicsParams.is_hitable;  //衝突しない

			collider_index_count[Sce]++;
			return ret;
		}

		//static void remove_collider(
		//	std::list<Collider*>::iterator coll_itr,
		//	Physics_function::ALP_Collider* ALPcoll_itr,
		//	Physics_function::ALP_Physics* ALPphys_itr,
		//	Scenelist Sce = Scene::now_scene);


		//static void remove_collider(
		//	std::list<Collider*>::iterator coll_itr,
		//	Physics_function::ALP_Collider* ALPcoll_itr,
		//	Scenelist Sce = Scene::now_scene);

		static void remove_Collider(
			const Scenelist Sce,
			std::list<Collider*>::iterator coll_itr)
		{
			colliders[Sce].erase(coll_itr);
		}

		//deleteはしない
		static void remove_ALPcollider(
			const Scenelist Sce,
			std::list<Physics_function::ALP_Collider*>::iterator ALPcoll_itr
		) {
			remove_collider_broad_phase(*ALPcoll_itr);
			ALP_colliders[Sce].erase(ALPcoll_itr);
		}

		//deleteはしない
		static void remove_ALPphysics(
			const Scenelist Sce,
			std::list<Physics_function::ALP_Physics*>::iterator ALPphs_itr
		) {
			ALP_physicses[Sce].erase(ALPphs_itr);
		}

	public:
		static bool ray_cast(
			const Vector3& Ray_pos, const Vector3& Ray_dir,
			u_int tag,
			const float ray_min,
			float& tmin, float& tmax,
			Vector3& normal,
			Collider* coll,
			Scenelist Sce = Scene::now_scene);

		static void add_moved(Physics_function::ALP_Collider* coll, Scenelist Sce = Scene::now_scene) {
			moved_collider[Sce].push_back(coll);
		}

	public:

		static bool init();

		// 毎フレーム呼ばれる更新処理
		static bool update(Scenelist Sce = Scene::now_scene);

		static bool update_Gui();

		// colliderの表示
		static bool render_collider(Scenelist Sce = Scene::now_scene);
		// kdopの表示
		static bool render_dop(Scenelist Sce = Scene::now_scene);

		static void destroy(Scenelist Sce = Scene::now_scene);

	};

}