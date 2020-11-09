#pragma once

#include "../Math/math.h"
#include "ALP_collider.h"

namespace Adollib {
	namespace physics_function {

		struct ALP_Solverbody {
			Vector3 delta_LinearVelocity; // 並進速度差分
			Vector3 delta_AngulaVelocity; // 回転速度差分
			Quaternion orientation; // 姿勢
			Matrix inv_inertia; // 慣性テンソルの逆行列
			float  inv_mass = 0; // 質量の逆数
			int num = 0;
		};

		class ALP_Physics {
		public:
			ALP_Physics() {};
			ALP_Physics(
				float inertial_mass, //質量
				float drag, //空気抵抗
				float anglar_drag, //空気抵抗
				float dynamic_friction,//動摩擦
				float static_friction, //静摩擦
				float restitution,	 //反発係数

				bool is_fallable, //落ちない
				bool is_kinematic, //影響うけない(fallはする)
				bool is_moveable,//動かない	
				bool is_hitable //衝突しない
			) :
				inertial_mass(inertial_mass), //質量
				drag(drag), //空気抵抗
				anglar_drag(anglar_drag), //空気抵抗
				dynamic_friction(dynamic_friction),//動摩擦
				static_friction(static_friction), //静摩擦
				restitution(restitution),	 //反発係数

				is_fallable(is_fallable), //落ちない
				is_kinematic(is_kinematic), //影響うけない(fallはする)
				is_moveable(is_moveable),//動かない	
				is_hitable(is_hitable)
			{};

			//::: 変更可 :::::::::::::::::::::::::::::
			float inertial_mass = 0; //質量
			float drag = 0; //空気抵抗
			float anglar_drag = 0; //空気抵抗
			float dynamic_friction = 0;//動摩擦
			float static_friction = 0; //静摩擦
			float restitution = 0;	 //反発係数

			bool is_fallable = false; //落ちない
			bool is_kinematic = false; //影響うけない(fallはする)
			bool is_moveable = false;//動かない
			bool is_hitable = false;  //衝突しない

			//::: 見せるだけ :::::::::::::::::::::::::
			float speed = 0;//debugしやすいように
			Vector3 linear_velocity;//並進速度
			Vector3 anglar_velocity; //回転速度

			Matrix inertial_tensor;//慣性テンソル

			bool sleep_state = false; //sleep状態かのflag

			//::: 見せない :::::::::::::::::::::::::::
			Vector3 accumulated_force;//並進速度
			Vector3 accumulated_torque; //回転速度

			Vector3 linear_acceleration;//並進加速度
			Vector3 angula_acceleration; //回転加速度

			std::list<ALP_Collider>::iterator ALP_coll;

			ALP_Solverbody* solve = nullptr; //衝突解決用

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
			Matrix inverse_inertial_tensor() const;

			//::: 毎フレーム呼ぶもの :::::
			//速度、加速度を0にする
			void reset_force();

			//外力の更新
			void apply_external_force(float duration = 1);

			//座標,姿勢の更新
			void integrate(float duration = 1);

			//サイズ変更などに対応するため毎フレーム慣性テンソルなどを更新 慣性テンソルの更新にALP_collider::scaleとALP_Physics::massが必要なためここに記述
			void update_inertial();

			//::: collider:Component の massなどが変更されたときに呼ぶもの :::
			// Colliderから情報の獲得 
			void refresh_ALP_from_data();

		};
	}
}
