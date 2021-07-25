#pragma once

#include <map>
#include "../Object/component.h"
#include "../Object/gameobject.h"
#include "../Scene/scene.h"

#include "collider.h"
#include "ALP_collider.h"
#include "ALP_physics.h"
#include "joint_base.h"

#include "ALP_broadphase.h"

#include "contact.h"

#include "collider_shape.h"


#include "ALP_joint.h"
#include "joint_base.h"

namespace Adollib
{
	class Collider;

	namespace Physics_function {
		// physics_managerのstatic変数の初期値
		struct PhysicsParams_default {
		public:

			static constexpr float gravity = 30; //重力

			static constexpr float bounce_threshold = 0.01f; //跳ね返りの閾値
			static constexpr float sleep_threshold = 0.01f; //sleepの閾値

			static constexpr float contact_allowable_error = 0.02f; //これ以上近いと同一衝突点とする
			static constexpr float contact_threrhold_normal = 0.004f; //衝突点の閾値
			static constexpr float contact_threrhold_tangent = 0.02f;//衝突点の閾値



			static constexpr float bias = 0.15f;//めり込みを直す力
			static constexpr float slop = 0.003f;//衝突の許容値

			static constexpr int solver_iterations = 10; //衝突の精度
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
			//float gravity = 9.8f; //重力
			float gravity = 30; //重力

			float bounce_threshold = 0.01f; //跳ね返りの閾値
			float sleep_threshold = 0.01f; //sleepの閾値

			float contact_allowable_error = 0.01f; //これ以上近いと同一衝突点とする
			float contact_threrhold_normal = 0.01f; //衝突点の閾値
			float contact_threrhold_tangent = 0.002f;//衝突点の閾値

			//float linear_sleep_threrhold = 0.2f; //freezeの閾値
			//float angula_sleep_threrhold = 0.02f; //freezeの閾値
			float linear_sleep_threrhold = 0.6f; //freezeの閾値
			float angula_sleep_threrhold = 0.15f; //freezeの閾値




			float bias = 0.15f;//めり込みを直す力
			float slop = 0.003f;//衝突の許容値

			int solver_iterations = 5; //衝突の精度
			bool hit_backfaces_flag = false;//meshの後ろから衝突するか

			float timeStep = 0.016f;
			float max_timeStep = 0.032f;

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

		class Phyisics_manager
		{
		private:
			static float frame_count;


			static std::unordered_map<Scenelist, u_int> collider_index_count;

			//各dataの実態配列
			static std::unordered_map<Scenelist, std::list<Physics_function::ALP_Collider*>> ALP_colliders;
			static std::unordered_map<Scenelist, std::list<Physics_function::ALP_Physics*>> ALP_physicses;
			static std::list<Physics_function::ALP_Joint*> ALP_joints;

			static std::vector<Physics_function::Contacts::Contact_pair*> pairs[2];
			static u_int pairs_new_num; //pairsのどっちが新しい衝突なのか

			//そのフレーム間で変化したもの(broad_phase用)
			static std::unordered_map<Scenelist, std::vector<Physics_function::ALP_Collider*>> moved_collider;   //動いた
			static std::unordered_map<Scenelist, std::vector<Physics_function::ALP_Collider*>> added_collider;   //追加された

		public:
			//生成時のphysicsの値
			static Physics_function::PhysicsParams physicsParams;

			static bool is_draw_collider; //COlliderの表示
			static bool is_draw_dop; //dopの表示(14-DOPを表示するのがめんどくさいためAABBを表示する)
			static bool is_draw_joint; //jointの表示

		public:
			struct ColliderPhysics_ptrs {
				ALP_Collider* ALPcollider_ptr = nullptr;
				ALP_Physics* ALPphysics_ptr = nullptr;
			};

			static ColliderPhysics_ptrs add_collider(Collider* coll) {

				Scenelist Sce = coll->gameobject->get_scene();

				ColliderPhysics_ptrs ret;
				ALP_Collider*& ALPcollider_ptr = ret.ALPcollider_ptr;
				ALP_Physics*& ALPphysics_ptr = ret.ALPphysics_ptr;

				{
					//itrがほしいため空のポインタで枠だけ取る
					Physics_function::ALP_Collider* null_coll = nullptr;
					Physics_function::ALP_Physics* null_phy = nullptr;

					ALP_colliders[Sce].emplace_back(null_coll);
					ALP_physicses[Sce].emplace_back(null_phy);

					auto collider_itr = ALP_colliders[Sce].end();
					collider_itr--;
					auto physics_itr = ALP_physicses[Sce].end();
					physics_itr--;

					//colliderのアドレスだけ生成 (ALPphysics,ALPcolliderのコンストラクタにお互いのアドレスが必要なため)
					ALPcollider_ptr = newD Physics_function::ALP_Collider(coll->gameobject, coll, collider_itr, nullptr, Sce, collider_index_count[Sce]);

					//phsics中身を入れつつ生成
					ALPphysics_ptr = newD Physics_function::ALP_Physics(coll->gameobject, physics_itr, ALPcollider_ptr, Sce, collider_index_count[Sce]);

					//colliderの中身を入れる
					*ALPcollider_ptr = Physics_function::ALP_Collider(coll->gameobject, coll, collider_itr, ALPphysics_ptr, Sce, collider_index_count[Sce]);

					//イテレーターでmanagerの配列にポインタを保存
					*collider_itr = ALPcollider_ptr;
					*physics_itr = ALPphysics_ptr;

					//phsicsの初期値の入力
					ALPphysics_ptr->set_default();
				}

				//追加されたcollider
				added_collider[Sce].emplace_back(ALPcollider_ptr);

				//::: 初期値をいれる :::
				coll->physics_data.inertial_mass = physicsParams.inertial_mass; //質量
				coll->physics_data.drag = physicsParams.linear_drag; //空気抵抗
				coll->physics_data.anglar_drag = physicsParams.anglar_drag; //空気抵抗
				coll->physics_data.dynamic_friction = physicsParams.dynamic_friction; //動摩擦
				coll->physics_data.static_friction = physicsParams.static_friction; //静摩擦
				coll->physics_data.restitution = physicsParams.restitution;	 //反発係数

				coll->physics_data.is_fallable = physicsParams.is_fallable; //落ちない
				coll->physics_data.is_kinmatic_anglar = physicsParams.is_kinmatic_anglar;//影響うけない(fallはする)
				coll->physics_data.is_kinmatic_linear = physicsParams.is_kinmatic_linear;//影響うけない(fallはする)
				coll->physics_data.is_moveable = physicsParams.is_moveable; //動かない
				coll->physics_data.is_hitable = physicsParams.is_hitable;  //衝突しない

				collider_index_count[Sce]++;

				return ret;
			}

			static ALP_Joint* add_Joint() {
				//static_assert(std::is_base_of<Joint_base, T>::value == true, "template T must inherit ALP_Joint_base");

				//空っぽのポインタで枠だけ確保(itrをコンストラクタで使いたいから)
				ALP_Joint* null_ptr;
				ALP_joints.emplace_back(null_ptr);
				auto itr = ALP_joints.end();
				itr--;

				//計算用の共通のクラスを生成
				ALP_Joint* ALPjoint = newD ALP_Joint(itr);
				//中身を入れる
				*itr = ALPjoint;

				return ALPjoint;
			};

			static void remove_Joint(std::list<Physics_function::ALP_Joint*>::iterator joint_itr) {
				ALP_joints.erase(joint_itr);
			};

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
			//::::::
			//raycastを行う
			// ray_pos : rayの始点
			// ray_dir : rayの向き
			// tag : rayの衝突するtag
			// ray_min
			// ---out---
			// tmin : rayの最近点
			// tmax : rayの最遠点
			// normal : 衝突した面のnormal
			// coll : 衝突したcoiiderへのポインタ
			// Sce  : Rayの衝突するcolliderの存在するscene
			//:::::::
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

			//static bool init();

			// 毎フレーム呼ばれる更新処理
			static bool update(Scenelist Sce = Scene::now_scene);

			static bool update_Gui();

			// colliderの表示
			static bool render_collider(Scenelist Sce = Scene::now_scene);
			// kdopの表示
			static bool render_dop(Scenelist Sce = Scene::now_scene);
			// jointの表示
			static bool render_joint(Scenelist Sce = Scene::now_scene);

			static void destroy(Scenelist Sce = Scene::now_scene);

		};

	}
}