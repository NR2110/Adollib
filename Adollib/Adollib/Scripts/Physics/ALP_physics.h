#pragma once

#include "../Math/math.h"
#include "ALP_collider.h"

#include "collider_types.h"

#include "../Object/transform.h"
#include <mutex>

//SIMDを使うかどうか バグっているため使用不可
#define PHYICSE_USED_SIMD


namespace Adollib {
	namespace Physics_function {

		struct ALP_Solverbody {
#ifndef PHYICSE_USED_SIMD
			Vector3 delta_LinearVelocity; // 並進速度差分
			Vector3 delta_AngulaVelocity; // 回転速度差分
			//Quaternion orientation; // 姿勢
			Matrix33 inv_tensor; // 慣性テンソルの逆行列
#else
			DirectX::XMVECTOR delta_LinearVelocity; // 並進速度差分
			DirectX::XMVECTOR delta_AngulaVelocity; // 回転速度差分
			//DirectX::XMVECTOR orientation; // 姿勢
			DirectX::XMMATRIX inv_inertia; // 慣性テンソルの逆行列
#endif
			float  inv_mass = 0; // 質量の逆数
			//int num = 0;
		};

		class ALP_Physics {
		public:
			//コンストラクタ
			ALP_Physics(
				Gameobject* l_go, std::list<ALP_Physics*>::iterator l_itr, ALP_Collider* _ALPcollider ,Scenelist l_scene, u_int l_index
			) :
				gameobject(l_go), this_itr(l_itr), ALPcollider(_ALPcollider), scene(l_scene), index(l_index) {
			};

		private:
			//::: 自身へのイテレータ(remove用) :::
			std::list<ALP_Physics*>::iterator this_itr;

			u_int index = 0; //このcolliderの番号
			Scenelist scene = Scenelist::scene_null; //このcolldierが存在するscene

			//::: 慣性モーメントがユーザー定義されたものか :::
			bool is_user_tensor = false;

			//::: 重心がユーザーに定義されたものか :::
			bool is_user_barycnter = false;

			//::: Colliderへのポインタ :::
			ALP_Collider* ALPcollider = nullptr;

			//::: アタッチされたGOへのポインタ :::
			Gameobject* gameobject = nullptr;

			//::: マルチスレッド用にcolliderにコピーされたtransformのポインタ physicsからは読み取りのみ :::
			world_trans* transform = nullptr;

			std::mutex mtx;

		public:
			// このphysicsが存在するscene
			Scenelist get_scene() const { return scene; };

			// マルチスレッド化するにあたり、add_colliderした時点ではメインのlistに入れずbufferのlistに入れるため 自身のitrが生成時に決まらないため set関数を準備
			void set_this_itr(std::list<ALP_Physics*>::iterator itr) { this_itr = itr; };

		public:
			ALP_Solverbody* solve = nullptr; //衝突解決用

			//::: addedの配列からメインの配列にadaptされたらtrueにする (itrがaddedを指すのかmainの配列を刺すのかわからないため)
			bool is_added = false;

			//::: 変更可 :::::::::::::::::::::::::::::
			float inertial_mass = 0; //質量
			float linear_drag = 0; //空気抵抗
			float angula_drag = 0; //空気抵抗
			float dynamic_friction = 0;//動摩擦
			float static_friction = 0; //静摩擦
			float restitution = 0;	 //反発係数

			bool is_fallable = false; //落ちない
			bool is_kinmatic_anglar = false; //ほかの物体からの影響で回転速度が変化しない
			bool is_kinmatic_linear = false; //ほかの物体からの影響で並進速度が変化しない
			bool is_moveable = false;//動かない
			bool is_hitable = false;  //衝突しない
			bool is_static = false;  //static同士はoncoll_enterが使えない けど軽くなる



		private:
			//::: 見せるだけ :::::::::::::::::::::::::
			Vector3 linear_velocity_;//並進速度
			Vector3 angula_velocity_; //回転速度

			Vector3 old_linear_velocity_;//並進速度
			Vector3 old_angula_velocity_; //回転速度

			bool is_sleep_ = false; //sleep状態かのflag
			float sleep_timer = 0; //

			Matrix33 inertial_tensor_; //慣性テンソル

		public:
			const Vector3& linear_velocity() const { return linear_velocity_; };
			const Vector3& angula_velocity() const { return angula_velocity_; };
			const Vector3& old_linear_velocity() const { return old_linear_velocity_; };
			const Vector3& old_angula_velocity() const { return old_angula_velocity_; };
			const bool& is_sleep()const { return is_sleep_; };

		private:
			float max_linear_velocity = FLT_MAX; //並進速度の制限
			float max_angula_velocity = FLT_MAX; //回転速度の制限

			Vector3 accumulated_force;  //並進速度
			Vector3 accumulated_torque; //回転速度

			Vector3 linear_acceleration; //並進加速度
			Vector3 angula_acceleration; //回転加速度

			Vector3 barycenter; //GOのlocal空間の重心座標

		public:
			//::: userのタイミングで呼ぶもの(だいたいmutexのlockが必要) :::::::

			// 並進移動に力を加える
			void add_force(const Vector3& force);
			void add_force(const Vector3& force, const Vector3& position, const bool& is_position_local = false,const bool& is_force_local = false);
			// 角回転に力を加える
			void add_torque(const Vector3& force);

			// 並進加速に値を加える
			void add_linear_acc(const Vector3& acc);
			//角加速に値を加える
			void add_angula_acc(const Vector3& acc);

			// 並進速度の指定
			void set_linear_velocity(const Vector3& vec) { std::lock_guard <std::mutex> lock(mtx); linear_velocity_ = vec;};
			// 角速度の指定
			void set_angula_velocity(const Vector3& vec) { std::lock_guard <std::mutex> lock(mtx); angula_velocity_ = vec; };
			// old並進速度の指定
			void set_old_linear_velocity(const Vector3& old_vec) { std::lock_guard <std::mutex> lock(mtx); old_linear_velocity_ += old_vec; };
			// old角速度の指定
			void set_old_angula_velocity(const Vector3& old_vec) { std::lock_guard <std::mutex> lock(mtx); old_angula_velocity_ += old_vec; };

			// 速度制限を行う
			void set_max_linear_velocity(const float& max_scalar) {/*std::lock_guard <std::mutex> lock(mtx);*/ max_linear_velocity = max_scalar; }; //max_linear_velocityにphsicsが値を入れないためmutexでlockしない
			// 速度制限を行う
			void set_max_angula_velocity(const float& max_scalar) {/*std::lock_guard <std::mutex> lock(mtx);*/ max_angula_velocity = max_scalar; };//max_angula_velocityにphtsicsが値を入れないためmutexでlockしない

			Matrix33 get_tensor();
			Matrix33 get_tensor_contain_added(); // 慣性モーメントを得るためにcollider,spaheのadaptなどいろいろしている
			void set_tensor(const Matrix33& tensor); //慣性モーメントをユーザー指定のものに固定する

			// 重心を返す
			const Vector3 get_barycenter() const; //shapeのみの重心
			const Vector3 get_barycenter_contain_added(); //added_shapeを含めた重心(mutexをlockするためconstにできない)
			void set_barycenter(const Vector3& cent); //重心をユーザー指定のものに固定する

			// 速度、加速度を0にする
			void reset_force();

			//::: 毎フレームphysics_managerが呼ぶもの(mutexのlockが必要ない) ::::::::::::

			// transformの更新(transformの値をコピーする 最終的には速度の影響量をtransformに与えるためmutexのlockは必要ないと判断)
			void copy_transform_ptr();

			// 外力の更新
			void apply_external_force(float duration = 1,float timeratio_60 = 1);

			// 座標,姿勢の更新
			void integrate(float duration = 1);

			// アタッチされたshapesから慣性モーメントと質量、ついでに重心の更新
			void update_tensor_and_barycenter(const std::list<Collider_shape*>& shapes, const std::list<ALP_Joint*>& joints);

			// Colliderから情報の獲得
			void adapt_collider_component_data(); //component::colliderの情報をコピーする mutexのlockは必要ないと判断


			//::: 必要な時に呼ぶもの(mutexのlockが必要ない) :::::::::

			// 可動オブジェクトかどうか
			bool is_movable() const;

			// 質量の逆数を返す(不稼働オブジェクトは0を返す)
			float inverse_mass() const;

			// 慣性モーメントの逆行列を返す
			Matrix33 inverse_inertial_tensor() const;

			// マネージャーからこのクラスのremove, itrがprivateなためメンバ関数でremoveする
			void destroy();

		};
	}
}
