#pragma once

#include <map>

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
#include <Windows.h>


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

			float bias = 0.15f;//めり込みを直す力
			float slop = 0.003f;//衝突の許容値

			int solver_iteration = 10; //衝突の精度
			int calculate_iteration = 5; //計算の精度

			int sweep_and_prune_divide_value = 100; //SweepAndPruneのX軸分割の分母

			float timescale = 1.0f;
			float timeStep = inv120;
			//float caluculate_time = inv120; //timeStepがこれ以上の時に更新が入る
			float caluculate_time = inv120; //timeStepがこれ以上の時に更新が入る
			float max_timeStep = 0.05f;


			//::: Physicsの初期値部分 :::
			float inertial_mass = 1.f; //質量
			float linear_drag = 0.1f; //空気抵抗
			float anglar_drag = 0.1f; //空気抵抗
			float dynamic_friction = 0.8f;//動摩擦
			float static_friction = 0.8f; //静摩擦
			float restitution = 0.1f;	 //反発係数
			float linear_sleep_threrhold = 0.2f; //freezeの閾値
			float angula_sleep_threrhold = 0.1f; //freezeの閾値

			bool is_fallable = true; //落ちるか
			bool is_kinmatic_anglar = true; //ほかの物体からの影響で回転速度が変化しない
			bool is_kinmatic_linear = true; //ほかの物体からの影響で並進速度が変化しない
			bool is_moveable = true;//動くか
			bool is_hitable = true;  //衝突しない
			bool is_static = false; //static同士は衝突せず oncoll_enterが発生しない けど軽くなる

			void set_default_physics_data(Physics_data& physics_data) {
				//::: 初期値をいれる :::
				physics_data.inertial_mass      = inertial_mass; //質量
				physics_data.drag               = linear_drag; //空気抵抗
				physics_data.anglar_drag        = anglar_drag; //空気抵抗
				physics_data.dynamic_friction   = dynamic_friction; //動摩擦
				physics_data.static_friction    = static_friction; //静摩擦
				physics_data.restitution        = restitution;	 //反発係数
				physics_data.linear_sleep_threrhold = linear_sleep_threrhold; //freezeの閾値
				physics_data.angula_sleep_threrhold = angula_sleep_threrhold; //freezeの閾値

				physics_data.is_fallable        = is_fallable; //落ちない
				physics_data.is_kinmatic_anglar = is_kinmatic_anglar;//影響うけない(fallはする)
				physics_data.is_kinmatic_linear = is_kinmatic_linear;//影響うけない(fallはする)
				physics_data.is_moveable        = is_moveable; //動かない
				physics_data.is_hitable         = is_hitable;  //衝突しない
			}
		};

		class Physics_manager
		{
		private:
			static LARGE_INTEGER frame_count; //
			static LARGE_INTEGER frame_count_stop; //
			static LARGE_INTEGER update_start_time; //
			static LARGE_INTEGER update_end_time; //
			static float update_time; //

			static std::mutex mtx; //主にadd_collder,phsics,jointと added_dataの扱いの時

			static u_int collider_index_count; //colliderにuniqueな値を割り振るため作成総数を保存

			// 各dataの実態配列
			static std::list<Physics_function::ALP_Collider*> ALP_colliders;
			static std::list<Physics_function::ALP_Physics*>  ALP_physicses;
			static std::list<Physics_function::ALP_Collider*> added_buffer_ALP_colliders; //マルチスレッド用 処理の途中で追加された要素
			static std::list<Physics_function::ALP_Physics*>  added_buffer_ALP_physicses; //マルチスレッド用 処理の途中で追加された要素
			static std::list<Physics_function::ALP_Collider*> deleted_buffer_ALP_colliders; //マルチスレッド用 処理の途中でGOが削除された要素
			static std::list<Physics_function::ALP_Physics*>  deleted_buffer_ALP_physicses; //マルチスレッド用 処理の途中でGOが削除された要素

			static std::list<Physics_function::ALP_Joint*> ALP_joints;
			static std::list<Physics_function::ALP_Joint*> added_buffer_ALP_joints; //マルチスレッド用 処理の途中で追加された要素
			static std::list<Physics_function::ALP_Joint*> deleted_buffer_ALP_joints; //マルチスレッド用 処理の途中で追加された要素

			static std::vector<Physics_function::Contacts::Contact_pair*> pairs[2];
			static u_int pairs_new_num; //pairsのどっちが新しい衝突なのか

			// そのフレーム間で変化したもの(broad_phase用)
			static std::vector<Physics_function::ALP_Collider*> moved_collider_for_insertsort; //挿入ソート用 動いた
			static std::vector<Physics_function::ALP_Collider*> added_collider_for_insertsort; //挿入ソート用 追加された

			static std::thread physics_thread; //trueになったときthread_updateを止める
			static bool is_stop_physics_thread; //trueになったときthread_updateを止める

		public:
			static volatile bool is_added_ALPcollider;    //Colliderが追加されたときtrueになる

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

			static ColliderPhysics_ptrs add_collider(Collider* coll, void* GO_ptr) {
				std::lock_guard <std::mutex> lock(mtx);

				is_added_ALPcollider = true;

				ColliderPhysics_ptrs ret;
				ALP_Collider*& ALPcollider_ptr = ret.ALPcollider_ptr;
				ALP_Physics*& ALPphysics_ptr = ret.ALPphysics_ptr;

				{
					//itrがほしいため空のポインタで枠だけ取る
					Physics_function::ALP_Collider* null_coll = nullptr;
					Physics_function::ALP_Physics* null_phy = nullptr;

					added_buffer_ALP_colliders.emplace_back(null_coll);
					added_buffer_ALP_physicses.emplace_back(null_phy);

					auto collider_itr = added_buffer_ALP_colliders.end();
					collider_itr--;
					auto physics_itr = added_buffer_ALP_physicses.end();
					physics_itr--;

					__int64 ID = reinterpret_cast<__int64>(GO_ptr);
					//colliderのアドレスだけ生成 (ALPphysics,ALPcolliderのコンストラクタにお互いのアドレスが必要なため)
					ALPcollider_ptr = newD Physics_function::ALP_Collider(ID, coll, collider_itr, nullptr, collider_index_count);

					//phsics中身を入れつつ生成
					ALPphysics_ptr = newD Physics_function::ALP_Physics(ID, physics_itr, ALPcollider_ptr, collider_index_count);

					//colliderのALPphysics_ptrに値を入れる
					ALPcollider_ptr->set_ALPphysics_ptr(ALPphysics_ptr);
					//*ALPcollider_ptr = Physics_function::ALP_Collider(coll->gameobject, coll, collider_itr, ALPphysics_ptr, Sce, collider_index_count[Sce]);

					//イテレーターでmanagerの配列にポインタを保存
					*collider_itr = ALPcollider_ptr;
					*physics_itr = ALPphysics_ptr;
				}

				//::: 初期値をいれる :::
				physicsParams.set_default_physics_data(coll->physics_data);
				//coll->physics_data.inertial_mass = physicsParams.inertial_mass; //質量
				//coll->physics_data.drag = physicsParams.linear_drag; //空気抵抗
				//coll->physics_data.anglar_drag = physicsParams.anglar_drag; //空気抵抗
				//coll->physics_data.dynamic_friction = physicsParams.dynamic_friction; //動摩擦
				//coll->physics_data.static_friction = physicsParams.static_friction; //静摩擦
				//coll->physics_data.restitution = physicsParams.restitution;	 //反発係数

				//coll->physics_data.is_fallable = physicsParams.is_fallable; //落ちない
				//coll->physics_data.is_kinmatic_anglar = physicsParams.is_kinmatic_anglar;//影響うけない(fallはする)
				//coll->physics_data.is_kinmatic_linear = physicsParams.is_kinmatic_linear;//影響うけない(fallはする)
				//coll->physics_data.is_moveable = physicsParams.is_moveable; //動かない
				//coll->physics_data.is_hitable = physicsParams.is_hitable;  //衝突しない

				collider_index_count++;

				return ret;
			}

			static ALP_Joint* add_Joint() {
				std::lock_guard <std::mutex> lock(mtx);

				is_added_ALPcollider = true;

				//空っぽのポインタで枠だけ確保(itrをコンストラクタで使いたいから)
				ALP_Joint* null_ptr;
				added_buffer_ALP_joints.emplace_back(null_ptr);
				auto itr = added_buffer_ALP_joints.end();
				itr--;

				//計算用の共通のクラスを生成
				ALP_Joint* ALPjoint = newD ALP_Joint(itr);
				//中身を入れる
				*itr = ALPjoint;

				ALPjoint->set_this_itr(itr);

				return ALPjoint;
			};


			//::: マルチスレッド化に 削除する物をbufferにいったんしまう :::
			static void add_delete_buffer_ALPCollider_and_ALPPhsics(Physics_function::ALP_Collider* coll, Physics_function::ALP_Physics* phys) {
				std::lock_guard <std::mutex> lock(mtx);
				deleted_buffer_ALP_colliders.emplace_back(coll);
				coll->is_deleted = true;

				deleted_buffer_ALP_physicses.emplace_back(phys);
			}
			static void add_delete_buffer_ALPJoint(Physics_function::ALP_Joint* joint) {
				std::lock_guard <std::mutex> lock(mtx);
				deleted_buffer_ALP_joints.emplace_back(joint);
			}

			//::: 配列からの削除。deleteはしない(physics_managerのloopから呼ぶためmutexは不要) :::
			static void remove_Joint(std::list<Physics_function::ALP_Joint*>::iterator joint_itr) {
				{
					// jointの削除とそのjointのついたcolliderの削除が同時に発生した時 deleteが2回行われて死ぬため
					// jointの削除時に保存されるbufferから対応するptrをnullptrにする
					auto itr = deleted_buffer_ALP_joints.begin();
					auto itr_end = deleted_buffer_ALP_joints.end();
					for (; itr != itr_end; ++itr) {
						if (*itr == *joint_itr) {
							//deleted_buffer_ALP_joints.erase(joint_itr);
							*itr = nullptr;
							//return;
						}
					}
				}

				if ((*joint_itr)->is_added)
					ALP_joints.erase(joint_itr);
				else
					added_buffer_ALP_joints.erase(joint_itr);
			};
			static void remove_ALPcollider(
				std::list<Physics_function::ALP_Collider*>::iterator ALPcoll_itr
			) {
				remove_collider_broad_phase(*ALPcoll_itr);

				if ((*ALPcoll_itr)->is_added) {
					// メインの配列に引っ越したが Broadphaseが呼ばれていない場合added_colliderがポインタを持っているためnullptrにする
					// 追加して、broadphaseが呼ばれる前にdeleteされる必要があるためおこる可能性が低い
					// addedの数が常に多くなる可能性も低いため 総当たりにする
					for (auto& coll : added_collider_for_insertsort) if (coll == *ALPcoll_itr)coll = nullptr;
					for (auto& coll : moved_collider_for_insertsort) if (coll == *ALPcoll_itr)coll = nullptr;

					ALP_colliders.erase(ALPcoll_itr);
				}
				else {
					added_buffer_ALP_colliders.erase(ALPcoll_itr);
				}
			}
			static void remove_ALPphysics(
				std::list<Physics_function::ALP_Physics*>::iterator ALPphs_itr
			) {
				if ((*ALPphs_itr)->is_added)
					ALP_physicses.erase(ALPphs_itr);
				else
					added_buffer_ALP_physicses.erase(ALPphs_itr);
			}

		public:
			//::::::
			//raycastを行う
			// ray_pos : rayの始点
			// ray_dir : rayの向き
			// tag : rayの衝突するtag
			// ray_min
			// Sce  : Rayの衝突するcolliderの存在するscene
			// ---out---
			// tmin : rayの最近点
			// tmax : rayの最遠点
			// normal : 衝突した面のnormal
			// coll : 衝突したcoiiderへのポインタ
			//:::::::
			static bool ray_cast(
				const Vector3& Ray_pos, const Vector3& Ray_dir,
				u_int tag,
				const float& ray_min,
				float& tmin, float& tmax,
				Vector3& normal,
				Collider*& coll
			);

			//::::::
			//spherecastを行う
			// ray_pos : rayの始点
			// ray_dir : rayの向き
			// tag : rayの衝突するtag
			// ray_min
			// Sce  : Rayの衝突するcolliderの存在するscene
			// ---out---
			// tmin : rayの最近点
			// tmax : rayの最遠点
			// normal : 衝突した面のnormal
			// coll : 衝突したcoiiderへのポインタ
			//:::::::
				static bool sphere_cast(
				const Vector3& Ray_pos, const Vector3& Ray_dir,
				const float& radius,
				Vector3& contact_point,
				u_int tag,
				const float& ray_min,
				float& tmin, float& tmax,
				Vector3& normal,
				Collider*& coll
				);

			// 動いたものとしてmoved_colliderに登録(sweep&pruneでの挿入ソートにて使う)
			static void add_moved(Physics_function::ALP_Collider* coll) {
				moved_collider_for_insertsort.push_back(coll);
			}

		private:
			// スレッド用 update while分でupdateを回している
			static void thread_update();

			// 追加されたものを適応する(マルチスレッドだと処理途中に追加されるためbufferを挟む)
			static void adapt_added_data(bool is_mutex_lock = true);

			// 削除されたものを適応する(マルチスレッドだと処理途中にGOが削除されるためbufferを挟む)
			static void dadapt_delete_data(bool is_mutex_lock = true);

		public:
			//// gameobject.transformをALPcollider.transformで更新する gameobjectのtransformが変化したらtrueを返す
			//static bool adapt_transform_to_gameobject();

			//// gameobjectのtransformをコピーする
			//static void copy_gameobject_transform();

			// 別threadでupdateを回す
			static void thread_start();

			// 別threadでのupdateを止めて、joinを行う
			static void thread_stop_and_join();

			static void mutex_lock() { mtx.lock(); };
			static void mutex_unlock() { mtx.unlock(); };

			static void timer_stop();
			static void timer_start();

		public:
			// main threadから呼ばれる

			// 毎フレーム呼ばれる更新処理
			static bool update();

			// Guiの表示
			static bool update_Gui();

			// colliderの表示
			static bool render_collider();
			// kdopの表示
			static bool render_dop();
			// jointの表示
			static bool render_joint();

			static void destroy();

		};

	}
}