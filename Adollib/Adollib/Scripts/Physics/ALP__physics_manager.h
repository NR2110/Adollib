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

	namespace Physics_function {
		// physics_managerのstatic変数の初期値
		class Physics_manager_default {
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
			static constexpr float drag = 0.1f; //空気抵抗
			static constexpr float anglar_drag = 0.1f; //空気抵抗
			static constexpr float dynamic_friction = 0.4f;//動摩擦
			static constexpr float static_friction = 0.4f; //静摩擦
			static constexpr float restitution = 0.1f;	 //反発係数

			static constexpr bool is_fallable = true; //落ちるか
			static constexpr bool is_kinematic = true; //影響うけるか(fallはする)
			static constexpr bool is_moveable = true;//動くか
			static constexpr bool is_hitable = true;  //衝突しない

			//::::::::
		};
	}

	class Phyisics_manager
	{
	private:
		static int frame_count;

		//collider_componentのポインタ配列
		static std::unordered_map<Scenelist, std::list<Collider*>> colliders;

		//各dataの実態配列
		static std::unordered_map<Scenelist, std::list<Physics_function::ALP_Collider>> ALP_colliders;
		static std::unordered_map<Scenelist, std::list<Physics_function::ALP_Physics>> ALP_physicses;

		static std::vector<Physics_function::Contacts::Contact_pair> pairs;
		static std::vector<Physics_function::Contacts::Collider_2> broad_mid_pair;

		static bool is_changed_coll; //新たにColliderが追加された、削除された場合true
	public:
		static bool is_changed_colliders() { return is_changed_coll; };

	public:
		static float gravity; //重力

		static Physics_function::ALP_Physics default_physics; //何も設定していないときのpyisicsの値

		static float bounce_threshold; //跳ね返りの閾値
		static float sleep_threshold; //sleepの閾値

		static float contact_allowable_error; //これ以上近いと同一衝突点とする
		static float contact_threrhold_normal; //衝突点の閾値
		static float contact_threrhold_tangent;//衝突点の閾値

		static float bias;//めり込みを直す力
		static float slop;//衝突の許容値

		static int solver_iterations; //衝突の精度
		static bool hit_backfaces_flag;//meshの後ろから衝突するか

		static float timeStep; 

		static bool is_draw_collider; //COlliderの表示
		static bool is_draw_dop; //dopの表示(14-DOPを表示するのがめんどくさいためAABBを表示する)

	public:

		static Physics_function::ColliderPhysics_itrs add_collider(Collider* coll, Scenelist Sce = Scene::now_scene) {

			is_changed_coll = true;

			Physics_function::ColliderPhysics_itrs ret;
			{
				colliders[Sce].emplace_back(coll);
				ret.coll_itr = colliders[Sce].end();
				ret.coll_itr--;
			}
			{
				Physics_function::ALP_Collider ALPcollider;

				ALP_colliders[Sce].emplace_back(ALPcollider);
				ret.ALPcollider_itr = ALP_colliders[Sce].end();
				ret.ALPcollider_itr--;
			}
			{
				Physics_function::ALP_Physics ALPphysics;
				ALPphysics = Phyisics_manager::default_physics;

				ALP_physicses[Sce].emplace_back(ALPphysics);
				ret.ALPphysics_itr = ALP_physicses[Sce].end();
				ret.ALPphysics_itr--;
			}

			ret.ALPcollider_itr->ALPphysics = ret.ALPphysics_itr;
			ret.ALPphysics_itr->ALP_coll = ret.ALPcollider_itr;

			//::: 初期値をいれる :::
			(*ret.coll_itr)->inertial_mass = Phyisics_manager::default_physics.inertial_mass; //質量
			(*ret.coll_itr)->drag = Phyisics_manager::default_physics.drag; //空気抵抗
			(*ret.coll_itr)->anglar_drag = Phyisics_manager::default_physics.anglar_drag; //空気抵抗
			(*ret.coll_itr)->dynamic_friction = Phyisics_manager::default_physics.dynamic_friction; //動摩擦
			(*ret.coll_itr)->static_friction = Phyisics_manager::default_physics.static_friction; //静摩擦
			(*ret.coll_itr)->restitution = Phyisics_manager::default_physics.restitution;	 //反発係数

			(*ret.coll_itr)->is_fallable = Phyisics_manager::default_physics.is_fallable; //落ちない
			(*ret.coll_itr)->is_kinematic = Phyisics_manager::default_physics.is_kinematic;//影響うけない(fallはする)
			(*ret.coll_itr)->is_moveable = Phyisics_manager::default_physics.is_moveable; //動かない
			(*ret.coll_itr)->is_hitable = Phyisics_manager::default_physics.is_hitable;  //衝突しない

			return ret;
		}

		static void remove_collider(
			std::list<Collider*>::iterator coll_itr, 
			std::list<Physics_function::ALP_Collider>::iterator ALPcoll_itr, 
			std::list<Physics_function::ALP_Physics>::iterator ALPphys_itr, 
			Scenelist Sce = Scene::now_scene) {
			is_changed_coll = true;
			ALP_colliders[Sce].erase(ALPcoll_itr);
			ALP_physicses[Sce].erase(ALPphys_itr);

			colliders[Sce].erase(coll_itr);
		}

		static void remove_collider(
			std::list<Collider*>::iterator coll_itr,
			std::list<Physics_function::ALP_Collider>::iterator ALPcoll_itr,
			Scenelist Sce = Scene::now_scene) {
			is_changed_coll = true;
			ALP_colliders[Sce].erase(ALPcoll_itr);

			colliders[Sce].erase(coll_itr);
		}

		public:
			static bool ray_cast(
				const Vector3& Ray_pos, const Vector3& Ray_dir,
				u_int tag,
				float& tmin, float& tmax,
				Vector3& normal,
				Collider* coll,
				Scenelist Sce = Scene::now_scene);

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