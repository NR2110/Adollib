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

		Gameobject* gameobject = nullptr;	//親情報

		Collider_shape shape = Collider_shape::shape_size;	//形情報
		DOP_14 dop14; //DOP_7データ
		physics_function::Solverbody* solve;

		std::string tag; //自身のタグ
		std::vector<std::string> No_hit_tag; //衝突しないタグ

		vector3 world_position = vector3();		     //ワールド空間での座標
		quaternion world_orientation = vector3();    //ワールド空間での姿勢
		vector3 world_size = vector3();              //ワールド空間での大きさ

		vector3 local_position = vector3();             //goからの相対座標
		quaternion local_orientation = quaternion_identity();       //goからの相対姿勢
		vector3 local_scale = vector3();				//goとの相対scale

		float density = 1;						//密度
		float restitution = 0.1; 						//反発係数
		float friction = 0.4;					//摩擦力

		vector3 linear_velocity = vector3();    //並進速度
		vector3 angula_velocity = vector3();   //角速度

		vector3 liner_acceleration = vector3();  //加速
		vector3 angular_acceleration = vector3();//角加速

		float inertial_mass = 0;           //慣性質量

		vector3 accumulated_force = vector3();  //並進移動に加える力

		matrix inertial_tensor = matrix();      //慣性モーメント

		vector3 accumulated_torque = vector3(); //角回転に加える力

		Collider() :
			local_position(0, 0, 0), local_orientation(0, 0, 0, 1),
			linear_velocity(0, 0, 0), angula_velocity(0, 0, 0),
			inertial_mass(1), accumulated_force(0, 0, 0),
			accumulated_torque(0, 0, 0), solve(nullptr)
		{
		}

		//外力の更新
		void apply_external_force(float duration = 1);

		//座標,姿勢の更新
		void integrate(float duration = 1);

		//gameobjectへ変化量を渡す
		void resolve_gameobject();

		//gameobjectのtransformからcolliderのworld空間での情報を更新
		virtual void update_world_trans() = 0;

		//並進移動に力を加える
		void add_force(const vector3& force);

		//角回転に力を加える
		void add_torque(const vector3& force);

		//可動オブジェクトかどうか
		bool is_movable() const;

		//質量の逆数を返す(不稼働オブジェクトは0を返す)
		float inverse_mass() const;

		//慣性モーメントの逆行列を返す
		matrix inverse_inertial_tensor() const;

		//サイズ所得関数
		virtual quaternion get_dimension() const = 0;

		//サイズ変更などに対応するため毎フレーム慣性テンソルなどを更新
		virtual void update_inertial(const vector3& size, float density = 1) = 0;

		//オブジェクトが動くたびに呼び出す　のが効率よいが適当に毎フレーム呼ぶ
		//DOP_14データの更新
		virtual void update_dop14() = 0;

	};
}