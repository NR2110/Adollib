#pragma once

#include <vector>
#include <assert.h>
#include "quaternion.h"
#include "matrix.h"

//貫通しない衝突判定を用いるか(少し重くなる?)
//これを用いる場合integral前に衝突判定を行う
//未完成
//いらない
#define USE_CHECK_CONTACT 0

namespace Adollib {

	class Gameobject;

	class Rigitbody {
	public:
		Gameobject* gameobject;           //親情報

		vector3 world_position;				//ワールド座標
		quaternion world_orientation;			//ワールド姿勢

		vector3 local_position;             //goからの相対座標
		quaternion local_orientation;       //goからの相対姿勢

		vector3 linear_velocity;    //並進速度
		vector3 angular_velocity;   //角速度

		float inertial_mass;           //慣性質量

		vector3 accumulated_force;  //並進移動に加える力

		matrix inertial_tensor;      //慣性モーメント

		vector3 accumulated_torque; //角回転に加える力

		Rigitbody::Rigitbody() :
			local_position(0, 0, 0), local_orientation(0, 0, 0, 1),
			linear_velocity(0, 0, 0), angular_velocity(0, 0, 0),
			inertial_mass(1), accumulated_force(0, 0, 0),
			accumulated_torque(0, 0, 0)
		{
			//inertial_tensor = ();
		}

		void integrate(float duration = 1);
		
		void resolve_gameobject(); //gameobjectへ変化量を渡す

		void update_world_trans(); //gameobjectのtransformからcolliderのworld空間での情報を更新

		void add_force(const vector3& force); //並進移動に力を加える

		void add_torque(const vector3& force); //角回転に力を加える

		bool is_movable() const; //可動オブジェクトかどうか

		float inverse_mass() const; //質量の逆数を返す(不稼働オブジェクトは0を返す)

		matrix inverse_inertial_tensor() const; //慣性モーメントの逆行列を返す

		virtual quaternion get_dimension() const = 0;

	};

	//球体用クラス
	class Sphere : public Rigitbody {
	public:
		float r; //半径

		Sphere(float r, float density, vector3 pos = vector3(0,0,0)) : r(r) {
			//座標
			local_position = pos;

			//質量の計算
			inertial_mass = 4.0f / 3.0f * r * r * r * DirectX::XM_PI * density;

			//慣性モーメントの計算
			inertial_tensor = matrix_identity();
			inertial_tensor._11 = 0.4f * inertial_mass * r * r;
			inertial_tensor._22 = 0.4f * inertial_mass * r * r;
			inertial_tensor._33 = 0.4f * inertial_mass * r * r;


		}

		//サイズの所得関数のオーバーライド
		virtual quaternion get_dimension() const {
			return quaternion(r, r, r);
		}

	};

	//平面用クラス
	class Plane : public Rigitbody {
	public:
		//不動オブジェクトとして生成
		Plane(vector3 n, float d) {

			//質量の計算
			inertial_mass = FLT_MAX;

			//慣性モーメントの計算
			inertial_tensor = matrix_identity();
			inertial_tensor._11 = FLT_MAX;
			inertial_tensor._22 = FLT_MAX;
			inertial_tensor._33 = FLT_MAX;

			n = n.unit_vect();
			local_position = n * d;

			vector3 Y(0, 1, 0);
			float angle = acosf(vector3_dot(Y, n));
			vector3 axis;
			axis = vector3_cross(Y, n);
			axis = axis.unit_vect();
			local_orientation = quaternion_angle_axis(angle, axis);
			local_orientation = local_orientation.unit_vect();
		}

		//サイズの所得関数のオーバーライド
		virtual quaternion get_dimension() const {
			return quaternion(1, 0, 1);
		}

	};

	//Boxクラス
	class Box : public Rigitbody {
	public:
		vector3 half_size;

		//不動オブジェクトとして生成
		Box(vector3 half_size, float density, vector3 pos = vector3(0,0,0)) : half_size(half_size) {
			//座標
			local_position = pos;

			//質量の計算
			inertial_mass = (half_size.x * half_size.y * half_size.z) * 8.0f * density;

			//慣性モーメントの計算
			inertial_tensor = matrix_identity();
			inertial_tensor._11 = 0.3333333f * inertial_mass * ((half_size.y * half_size.y) + (half_size.z * half_size.z));
			inertial_tensor._22 = 0.3333333f * inertial_mass * ((half_size.z * half_size.z) + (half_size.x * half_size.x));
			inertial_tensor._33 = 0.3333333f * inertial_mass * ((half_size.x * half_size.x) + (half_size.y * half_size.y));
		}

		//サイズの所得関数のオーバーライド
		virtual quaternion get_dimension() const {
			return half_size;
		}

	};

	//衝突用のnamespace 他の人があまり触れないように
	namespace Contacts {

		//衝突処理用
		struct Contact {

			Rigitbody* body[2]; //接触したobject

			vector3 point; //衝突点
			vector3 normal; //衝突面の法線
			float penetration; //貫通量
			float restitution; //反射係数

			Contact() : point(0, 0, 0), normal(0, 0, 0), penetration(0), restitution(0) {
				body[0] = body[1] = 0;
			}

			void resolve();
		};

		//衝突の確認
		//bool check_contact_sphere_sphere(Sphere& S1, Sphere& S2);
		//bool check_contact_sphere_plane(Sphere& S1, Plane& S2);
		//bool check_contact_sphere_box(Sphere& S1, Box& S2);
		//bool check_contact_box_plane(Box& S1, Plane& S2);
		//bool check_contact_box_box(Box& S1, Box& S2);

		//衝突の生成
		int generate_contact_sphere_sphere(Sphere& S1, Sphere& S2, std::vector<Contact>& contacts, float restitution);
		int generate_contact_sphere_plane(Sphere& S1, Plane& S2, std::vector<Contact>& contacts, float restitution, bool half_space = true);
		int generate_contact_sphere_box(Sphere& S1, Box& S2, std::vector<Contact>& contacts, float restitution);
		int generate_contact_box_plane(Box& S1, Plane& S2, std::vector<Contact>& contacts, float restitution);
		int generate_contact_box_box(Box& S1, Box& S2, std::vector<Contact>& contacts, float restitution);
	}












}