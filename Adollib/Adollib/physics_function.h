#pragma once
#include "contact.h"

namespace Adollib {
	namespace physics_function {
		struct Solverbody {
			vector3 delta_LinearVelocity; // 並進速度差分
			vector3 delta_AngularVelocity; // 回転速度差分
			quaternion    orientation; // 姿勢
			matrix inv_inertia; // 慣性テンソルの逆行列
			float   inv_mass; // 質量の逆数
		};

		//衝突の確認
		//bool check_contact_sphere_sphere(Sphere& S1, Sphere& S2);
		//bool check_contact_sphere_plane(Sphere& S1, Plane& S2);
		//bool check_contact_sphere_box(Sphere& S1, Box& S2);
		//bool check_contact_box_plane(Box& S1, Plane& S2);
		//bool check_contact_box_box(Box& S1, Box& S2);

		//外力による速度などの更新
		void applyexternalforce(std::vector<Collider*>& coll);

		//大雑把な当たり判定(Boardphase)
		void Boardphase(const std::vector<Collider*>& coll, std::vector<Contacts::Contact_pair>& pairs);

		//衝突生成(Narrowphase)
		void generate_contact(std::vector<Contacts::Contact_pair*>& pairs);
		//衝突生成
		bool generate_contact_sphere_sphere(const Sphere& S1, const Sphere& S2, Contacts::Contact_pair& pair);
		bool generate_contact_sphere_plane(const Sphere& S1, const Plane& S2, Contacts::Contact_pair& pair);
		bool generate_contact_sphere_box(const Sphere& S1, const  Box& S2, Contacts::Contact_pair& pair);
		bool generate_contact_box_plane(const Box& S1, const Plane& S2, Contacts::Contact_pair& pair);
		bool generate_contact_box_box(const Box& S1, const  Box& S2, Contacts::Contact_pair& pair);

		//衝突解決
		void resolve_contact(std::vector<Collider*> colliders, std::vector<Contacts::Contact_pair>& pairs);

		void integrate(std::vector<Collider*>& coll);

		
	}
}