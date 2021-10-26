#pragma once

#include <map>
#include "../Object/component.h"
#include "../Object/gameobject.h"
#include "../Scene/scene_manager.h"

#include "collider.h"
#include "ALP_collider.h"
#include "ALP_physics.h"
#include "joint_base.h"

#include "ALP_broadphase.h"

#include "ALP_contact.h"

#include "collider_shape.h"


#include "ALP_joint.h"
#include "joint_base.h"

#include <map>
#include <mutex>

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
			static constexpr bool is_static = false; //static同士は衝突せず oncoll_enterが発生しない けど軽くなる
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
			float linear_sleep_threrhold = 0.5f; //freezeの閾値
			//float angula_sleep_threrhold = 0.15f; //freezeの閾値
			float angula_sleep_threrhold = 0.5f; //freezeの閾値


			float bias = 0.15f;//めり込みを直す力
			float slop = 0.003f;//衝突の許容値

			int solver_iteration = 5; //衝突の精度
			int calculate_iteration = 3; //計算の精度
			bool hit_backfaces_flag = false;//meshの後ろから衝突するか

			float timeStep = 0.016f;
			//float max_timeStep = 0.032f;
			float max_timeStep = 0.05f;

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
			bool is_static = false; //static同士は衝突せず oncoll_enterが発生しない けど軽くなる
		};

		class Physics_manager
		{
		private:
			static LARGE_INTEGER frame_count; //

			static std::mutex mtx; //主にadd_collder,phsics,jointと added_dataの扱いの時

			static std::unordered_map<Scenelist, u_int> collider_index_count; //colliderにuniqueな値を割り振るため作成総数を保存

			// 各dataの実態配列
			static std::unordered_map<Scenelist, std::list<Physics_function::ALP_Collider*>> ALP_colliders;
			static std::unordered_map<Scenelist, std::list<Physics_function::ALP_Physics*>>  ALP_physicses;
			static std::map<Scenelist, std::list<Physics_function::ALP_Collider*>> added_buffer_ALP_colliders; //マルチスレッド用 処理の途中で追加された要素
			static std::map<Scenelist, std::list<Physics_function::ALP_Physics*>>  added_buffer_ALP_physicses; //マルチスレッド用 処理の途中で追加された要素
			static std::map<Scenelist, std::list<Physics_function::ALP_Collider*>> dalated_buffer_ALP_colliders; //マルチスレッド用 処理の途中でGOが削除された要素
			static std::map<Scenelist, std::list<Physics_function::ALP_Physics*>>  dalated_buffer_ALP_physicses; //マルチスレッド用 処理の途中でGOが削除された要素

			static std::list<Physics_function::ALP_Joint*> ALP_joints;
			static std::list<Physics_function::ALP_Joint*> added_buffer_ALP_joints; //マルチスレッド用 処理の途中で追加された要素
			static std::list<Physics_function::ALP_Joint*> dalated_buffer_ALP_joints; //マルチスレッド用 処理の途中で追加された要素

			static std::vector<Physics_function::Contacts::Contact_pair*> pairs[2];
			static u_int pairs_new_num; //pairsのどっちが新しい衝突なのか

			// そのフレーム間で変化したもの(broad_phase用)
			static std::unordered_map<Scenelist, std::vector<Physics_function::ALP_Collider*>> moved_collider_for_insertsort; //挿入ソート用 動いた
			static std::unordered_map<Scenelist, std::vector<Physics_function::ALP_Collider*>> added_collider_for_insertsort; //挿入ソート用 追加された

			static int count_mainthread;
			static int count_physicsthread;

			static std::thread physics_thread; //trueになったときthread_updateを止める
			static bool is_stop_physics_thread; //trueになったときthread_updateを止める

		public:
			static volatile bool is_updated_mainthread;    //mainthread更新したframeだけtrueになる
			static volatile bool is_updated_physicsthread; //physicsを更新したframeだけtrueになる

			// 生成時のphysicsの値
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
				std::lock_guard <std::mutex> lock(mtx);

				Scenelist Sce = coll->gameobject->get_scene();

				ColliderPhysics_ptrs ret;
				ALP_Collider*& ALPcollider_ptr = ret.ALPcollider_ptr;
				ALP_Physics*& ALPphysics_ptr = ret.ALPphysics_ptr;

				{
					//itrがほしいため空のポインタで枠だけ取る
					Physics_function::ALP_Collider* null_coll = nullptr;
					Physics_function::ALP_Physics* null_phy = nullptr;

					added_buffer_ALP_colliders[Sce].emplace_back(null_coll);
					added_buffer_ALP_physicses[Sce].emplace_back(null_phy);

					auto collider_itr = added_buffer_ALP_colliders[Sce].end();
					collider_itr--;
					auto physics_itr = added_buffer_ALP_physicses[Sce].end();
					physics_itr--;

					//colliderのアドレスだけ生成 (ALPphysics,ALPcolliderのコンストラクタにお互いのアドレスが必要なため)
					ALPcollider_ptr = newD Physics_function::ALP_Collider(coll->gameobject, coll, collider_itr, nullptr, Sce, collider_index_count[Sce]);

					//phsics中身を入れつつ生成
					ALPphysics_ptr = newD Physics_function::ALP_Physics(coll->gameobject, physics_itr, ALPcollider_ptr, Sce, collider_index_count[Sce]);

					//colliderのALPphysics_ptrに値を入れる
					ALPcollider_ptr->set_ALPphysics_ptr(ALPphysics_ptr);
					//*ALPcollider_ptr = Physics_function::ALP_Collider(coll->gameobject, coll, collider_itr, ALPphysics_ptr, Sce, collider_index_count[Sce]);

					//イテレーターでmanagerの配列にポインタを保存
					*collider_itr = ALPcollider_ptr;
					*physics_itr = ALPphysics_ptr;
				}

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

				for (auto& debug : added_buffer_ALP_colliders[Sce]) {
					if (debug == nullptr) {
						int adsfdg = 0;
					}
				}
				return ret;
			}

			static ALP_Joint* add_Joint() {
				std::lock_guard <std::mutex> lock(mtx);

				//空っぽのポインタで枠だけ確保(itrをコンストラクタで使いたいから)
				ALP_Joint* null_ptr;
				added_buffer_ALP_joints.emplace_back(null_ptr);
				auto itr = added_buffer_ALP_joints.end();
				itr--;

				//計算用の共通のクラスを生成
				ALP_Joint* ALPjoint = newD ALP_Joint(itr);
				//中身を入れる
				*itr = ALPjoint;

				return ALPjoint;
			};


			//::: マルチスレッド化に 削除する物をbufferにいったんしまう :::
			static void add_delete_buffer_ALPCollider_and_ALPPhsics(Physics_function::ALP_Collider* coll, Physics_function::ALP_Physics* phys) {
				std::lock_guard <std::mutex> lock(mtx);
				dalated_buffer_ALP_colliders[coll->get_scene()].emplace_back(coll);
				coll->is_deleted = true;

				dalated_buffer_ALP_physicses[phys->get_scene()].emplace_back(phys);
			}
			static void add_delete_buffer_ALPJoint(Physics_function::ALP_Joint* joint) {
				std::lock_guard <std::mutex> lock(mtx);
				dalated_buffer_ALP_joints.emplace_back(joint);
			}

			//::: 配列からの削除。deleteはしない(physics_managerのloopから呼ぶためmutexは不要) :::
			static void remove_Joint(std::list<Physics_function::ALP_Joint*>::iterator joint_itr) {
				if((*joint_itr)->is_added)
					ALP_joints.erase(joint_itr);
				else
					added_buffer_ALP_joints.erase(joint_itr);
			};
			static void remove_ALPcollider(
				const Scenelist Sce,
				std::list<Physics_function::ALP_Collider*>::iterator ALPcoll_itr
			) {
				remove_collider_broad_phase(*ALPcoll_itr);

				if ((*ALPcoll_itr)->is_added) {
					// メインの配列に引っ越したが Broadphaseが呼ばれていない場合added_colliderがポインタを持っているためnullptrにする
					// 追加して、broadphaseが呼ばれる前にdeleteされる必要があるためおこる可能性が低い
					// addedの数が多くなる可能性も低いため 総当たりにする
					for (auto& coll : added_collider_for_insertsort[Sce]) if (coll == *ALPcoll_itr)coll = nullptr;

					ALP_colliders[Sce].erase(ALPcoll_itr);
				}
				else {
					added_buffer_ALP_colliders[Sce].erase(ALPcoll_itr);
				}
			}
			static void remove_ALPphysics(
				const Scenelist Sce,
				std::list<Physics_function::ALP_Physics*>::iterator ALPphs_itr
			) {
				if((*ALPphs_itr)->is_added)
					ALP_physicses[Sce].erase(ALPphs_itr);
				else
					added_buffer_ALP_physicses[Sce].erase(ALPphs_itr);
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
				Scenelist Sce = Scene_manager::get_nowscene());

			// 動いたものとしてmoved_colliderに登録(sweep&pruneでの挿入ソートにて使う)
			static void add_moved(Physics_function::ALP_Collider* coll, Scenelist Sce = Scene_manager::get_nowscene()) {
				moved_collider_for_insertsort[Sce].push_back(coll);
			}

		private:
			// スレッド用 update while分でupdateを回している
			static void thread_update();

			// 追加されたものを適応する(マルチスレッドだと処理途中に追加されるためbufferを挟む)
			static void adapt_added_data(Scenelist Sce, bool is_mutex_lock = true);

			// 削除されたものを適応する(マルチスレッドだと処理途中にGOが削除されるためbufferを挟む)
			static void dadapt_delete_data(bool is_mutex_lock = true);

		public:
			// gameobject.transformをALPcollider.transformで更新する gameobjectのtransformが変化したらtrueを返す
			static bool adapt_transform_to_gameobject(Scenelist Sce);

			// gameobjectのtransformをコピーする
			static void copy_gameobject_transform(Scenelist Sce);

			// 別threadでupdateを回す
			static void thread_start();

			// 別threadでのupdateを止めて、joinを行う
			static void thread_stop_and_join();

		public:
			// main threadから呼ばれる

			// 毎フレーム呼ばれる更新処理
			static bool update(Scenelist Sce = Scene_manager::get_nowscene());

			// Guiの表示
			static bool update_Gui();

			// colliderの表示
			static bool render_collider(Scenelist Sce = Scene_manager::get_nowscene());
			// kdopの表示
			static bool render_dop(Scenelist Sce = Scene_manager::get_nowscene());
			// jointの表示
			static bool render_joint(Scenelist Sce = Scene_manager::get_nowscene());

			static void destroy(Scenelist Sce = Scene_manager::get_nowscene());

		};

	}
}