#pragma once

#include <vector>
#include <assert.h>
#include <string>
#include "math.h"

namespace Adollib {

	class Gameobject;
	namespace  physics_function {
		class Solverbody;
	}
	namespace Contacts {
		class Contact;
	}

	enum class Collider_shape {
		shape_box,
		shape_sphere,
		shape_plane,
		shape_mesh,

		shape_size

	};

	class Collider {
	public:
		bool move = true; //可動オブジェクトカどうかのフラグ
		bool fall = true;

		Gameobject* gameobject = nullptr;	//親情報

		Collider_shape shape = Collider_shape::shape_size;	//形情報
		DOP::DOP_14 dop14; //DOP_7データ
		physics_function::Solverbody* solve;

		std::string tag; //自身のタグ
		std::vector<std::string> No_hit_tag; //衝突しないタグ

		Vector3 world_position = Vector3();		     //ワールド空間での座標
		Quaternion world_orientation = Vector3();    //ワールド空間での姿勢
		Vector3 world_scale = Vector3();              //ワールド空間での大きさ

		Vector3 local_position = Vector3();             //goからの相対座標
		Quaternion local_orientation = quaternion_identity();       //goからの相対姿勢
		Vector3 local_scale = Vector3();				//goとの相対scale

		float density = 1;						//密度
		float restitution = 0.1; 						//反発係数
		float friction = 0.4;					//摩擦力

		Quaternion offset_CollGO_quat;
		Vector3 offset_CollGO_pos;

		Vector3 linear_velocity = Vector3();    //並進速度
		Vector3 angula_velocity = Vector3();   //角速度

		Vector3 linear_acceleration = Vector3();  //加速
		Vector3 angula_acceleration = Vector3();//角加速

		float inertial_mass = 0;           //慣性質量

		Vector3 accumulated_force = Vector3();  //並進移動に加える力

		matrix inertial_tensor = matrix();      //慣性モーメント

		Vector3 accumulated_torque = Vector3(); //角回転に加える力

		Collider() :
			local_position(0, 0, 0), local_orientation(1, 0, 0, 0), local_scale(1,1,1),
			linear_velocity(0, 0, 0), angula_velocity(0, 0, 0),
			inertial_mass(1), accumulated_force(0, 0, 0),
			accumulated_torque(0, 0, 0), solve(nullptr)
		{
		}

		//外力の更新
		void apply_external_force(float duration = 1);

		//速度、加速度を0にする
		void reset_force();

		//座標,姿勢の更新
		void integrate(float duration = 1);

		//gameobjectへの変化量を求める
		void solv_resolve();

		//gameobjectへ変化量を渡す
		void resolve_gameobject();

		//gameobjectのtransformからcolliderのworld空間での情報を更新
		virtual void update_world_trans() = 0;

		//並進移動に力を加える
		void add_force(const Vector3& force);

		//角回転に力を加える
		void add_torque(const Vector3& force);

		//可動オブジェクトかどうか
		bool is_movable() const;

		//落下オブジェクトかどうか
		bool is_fallable() const;

		//質量の逆数を返す(不稼働オブジェクトは0を返す)
		float inverse_mass() const;

		//慣性モーメントの逆行列を返す
		matrix inverse_inertial_tensor() const;

		//サイズ所得関数
		virtual Quaternion get_dimension() const = 0;

		//サイズ変更などに対応するため毎フレーム慣性テンソルなどを更新
		virtual void update_inertial(const Vector3& size, float density = 1) = 0;

		//オブジェクトが動くたびに呼び出す　のが効率よいが適当に毎フレーム呼ぶ
		//DOP_14データの更新
		virtual void update_dop14() = 0;

	};
}