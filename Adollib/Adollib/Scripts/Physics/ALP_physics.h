#pragma once

#include "../Math/math.h"
#include "ALP_collider.h"

#include "collider_types.h"

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
				gameobject(l_go), this_itr(l_itr), ALPcollider(_ALPcollider), scene(l_scene), index(l_index) {};

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

		public:
			ALP_Solverbody* solve = nullptr; //衝突解決用

			void set_default();

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

			//::: 見せるだけ :::::::::::::::::::::::::
			float speed = 0;//debugしやすいように
			Vector3 linear_velocity;//並進速度
			Vector3 angula_velocity; //回転速度

			Matrix33 inertial_tensor; //慣性テンソル

			bool is_sleep = false; //sleep状態かのflag
			float sleep_timer = 0;

		private:
			Vector3 accumulated_force;  //並進速度
			Vector3 accumulated_torque; //回転速度

			Vector3 linear_acceleration; //並進加速度
			Vector3 angula_acceleration; //回転加速度

			Vector3 barycenter; //GOのlocal空間の重心座標

		public:
			//並進移動に力を加える
			void add_force(const Vector3& force);

			//角回転に力を加える
			void add_torque(const Vector3& force);

			//可動オブジェクトかどうか
			bool is_movable() const;

			//質量の逆数を返す(不稼働オブジェクトは0を返す)
			float inverse_mass() const;

			//慣性モーメントの逆行列を返す
			Matrix33 inverse_inertial_tensor() const;

			//慣性モーメントをユーザー指定のものに固定する
			void set_tensor(const Matrix33& tensor) {
				is_user_tensor = true;
				inertial_tensor = tensor;
			}

			//重心を返す
			const Vector3 get_barycenter() const;
			void set_barycenter(const Vector3& cent);

			//::: 毎フレーム呼ぶもの ::::::::::::
			//速度、加速度を0にする
			void reset_force();

			//外力の更新
			void apply_external_force(float duration = 1);

			//座標,姿勢の更新
			void integrate(float duration = 1);

			//アタッチされたshapesから慣性モーメントと質量、ついでに重心の更新
			void update_tensor_and_barycenter(const std::vector<Collider_shape*>& shapes, const std::list<ALP_Joint*>& joints);


			//::: collider:Component の massなどが変更されたときに呼ぶもの :::
			// Colliderから情報の獲得
			void update_physics_data();

			//マネージャーからこのクラスのremove itrがprivateなためメンバ関数でremoveする
			void destroy();

		};
	}
}
