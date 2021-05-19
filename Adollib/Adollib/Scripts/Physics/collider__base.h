//#pragma once
//
//#include <vector>
//#include <assert.h>
//#include <string>
//#include <unordered_map>
//#include "../Math/math.h"
//#include "../Object/component.h"
//
//#include "ALP__meshcoll_data.h"
//#include "ALP_collider.h"
//#include "ALP_physics.h"
//
//#include "../Imgui/imgui_all.h"
//
//namespace Adollib {
//
//	namespace Physics_function {
//
//		//表示用のphysics_data ユーザーが簡単に変更できるように
//		struct Physics_data {
//			float inertial_mass = 0; //質量
//			float drag = 0; //空気抵抗
//			float anglar_drag = 0; //空気抵抗
//			float dynamic_friction = 0; //動摩擦
//			float static_friction = 0; //静摩擦
//			float restitution = 0;	 //反発係数
//
//			bool is_fallable = 0; //落ちない
//			bool is_kinematic = 0;//影響うけない(fallはする)
//			bool is_kinmatic_anglar = 0; //ほかの物体からの影響で回転速度が変化しない
//			bool is_kinmatic_linear = 0; //ほかの物体からの影響で並進速度が変化しない
//			bool is_moveable = 0; //動かない
//			bool is_hitable = 0;  //衝突しない
//		};
//		//:::::::::::::::::::::::::
//
//		//physics_managerのadd_colliderしたときの返り値
//		struct ColliderPhysics_itrs {
//			std::list<ALP_Collider>::iterator ALPcollider_itr;
//			std::list<ALP_Physics>::iterator ALPphysics_itr;
//
//			std::list<Collider*>::iterator coll_itr;
//
//		};
//
//	}
//
//	class Collider : public Component {
//
//	public:
//
//		//::: tagの初期値 ::::::::
//		u_int tag = 0; //自身のtag(bit)
//		u_int ignore_tags = 0; //衝突しないtags
//
//		//::: unityのphysics部分 分ける必要なんてないやろ ::::
//		Physics_function::Physics_data physics_data;
//
//		//static同士で衝突判定を行わない
//		bool is_static = false;
//
//	protected:
//		//physics_managerが実態を持つ。removeを実装するためlistの配列 したがってitrを保存。きもいので要修正
//		//実態をcolliderが持つべき? ->managerがポインタ配列を持つ? きもい?
//		std::list<Physics_function::ALP_Collider>::iterator ALPcollider_itr;
//		std::list<Physics_function::ALP_Physics>::iterator ALPphysics_itr;
//
//		std::list<Collider*>::iterator coll_itr;//自身へのitr
//
//	private:
//		bool removed = false; //二重にremoveをするのを防ぐ
//
//		//このcolliderをmanagerから削除
//		void remove_collider();
//
//	public:
//		//::: 後で変更する :::
//		const Vector3 linear_velocity() const { return ALPphysics_itr->linear_velocity; }
//		const Vector3 linear_velocity(Vector3 v) {
//			ALPphysics_itr->linear_velocity = v;
//			return ALPphysics_itr->linear_velocity;
//		}
//		const Vector3 linear_anglar_velocity() const { return ALPphysics_itr->anglar_velocity; }
//		const Vector3 linear_anglar_velocity(Vector3 v) {
//			ALPphysics_itr->anglar_velocity = v;
//			return ALPphysics_itr->anglar_velocity;
//		}
//
//	public:
//		//on collision enter
//		const bool concoll_enter(const unsigned int tag_name) const;
//
//		//並進移動に力を加える
//		void add_force(const Vector3& force);
//
//		//角回転に力を加える
//		void add_torque(const Vector3& force);
//
//
//	public:
//		void Update_hierarchy();
//
//		void awake() override;
//		void finalize() override;
//
//	};
//}