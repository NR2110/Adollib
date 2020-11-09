#pragma once

#include <vector>
#include <assert.h>
#include <string>
#include <unordered_map>
#include "../Math/math.h"
#include "../Object/component.h"

#include "ALP__meshcoll_data.h"
#include "ALP_collider.h"
#include "ALP_physics.h"

namespace Adollib {

	namespace physics_function {
		//::: ŒvZ‚È‚Ç‚Ég‚¤î•ñ :::
		struct Collider_data {
			Vector3 local_position;
			Quaternion local_orientation;
			Vector3 local_scale;

			Vector3 half_size;

			physics_function::ALP_Collider_shape shape = physics_function::ALP_Collider_shape::None; //Œ`î•ñ

			Meshcoll_data meshcoll_data; //meshcollider—pdata
		};
		struct Physics_data {
			float inertial_mass; //¿—Ê
			float drag; //‹ó‹C’ïR
			float anglar_drag; //‹ó‹C’ïR
			float dynamic_friction;//“®–€C
			float static_friction; //Ã–€C
			float restitution;	 //”½”­ŒW”

			bool is_fallable; //—‚¿‚È‚¢
			bool is_kinematic; //‰e‹¿‚¤‚¯‚È‚¢(fall‚Í‚·‚é)
			bool is_moveable;//“®‚©‚È‚¢
			bool is_hitable;  //Õ“Ë‚µ‚È‚¢
		};
		//:::::::::::::::::::::::::

		struct ColliderPhysics_itrs {
			std::list<ALP_Collider>::iterator ALPcollider_itr;
			std::list<ALP_Physics>::iterator ALPphysics_itr;

			std::list<Collider*>::iterator coll_itr;

		};

	}

	class Collider : public Component {

	public:
		//::: tagŠÖŒW ::::::::
		u_int tag = 0; //©g‚Ìtag(bit)
		u_int not_hitable_tags = 0; //Õ“Ë‚µ‚È‚¢tags

		//::: unity‚Ìphysics•”•ª •ª‚¯‚é•K—v‚È‚ñ‚Ä‚È‚¢‚â‚ë ::::
		float inertial_mass = 0; //¿—Ê
		float drag = 0; //‹ó‹C’ïR
		float anglar_drag = 0; //‹ó‹C’ïR
		float dynamic_friction = 0; //“®–€C
		float static_friction = 0; //Ã–€C
		float restitution = 0;	 //”½”­ŒW”

		bool is_fallable = 0; //—‚¿‚È‚¢
		bool is_kinematic = 0;//‰e‹¿‚¤‚¯‚È‚¢(fall‚Í‚·‚é)
		bool is_moveable = 0; //“®‚©‚È‚¢
		bool is_hitable = 0;  //Õ“Ë‚µ‚È‚¢

	protected:
		std::list<physics_function::ALP_Collider>::iterator ALPcollider_itr;
		std::list<physics_function::ALP_Physics>::iterator ALPphysics_itr;

	//	std::vector<physics_function::ColliderPhysics_itrs> ALP_itrs;

		std::list<Collider*>::iterator coll_itr;//©g‚Ö‚Ìitr

	private:
		bool removed = false; //“ñd‚Éremove‚ğ‚·‚é‚Ì‚ğ–h‚®

	public:
		//on collision enter
		const bool concoll_enter(const unsigned int tag_name) const;

		//•ÀiˆÚ“®‚É—Í‚ğ‰Á‚¦‚é
		void add_force(const Vector3& force);

		//Šp‰ñ“]‚É—Í‚ğ‰Á‚¦‚é
		void add_torque(const Vector3& force);

		//ŒvZ—p‚Ìstruct
		virtual physics_function::Collider_data get_Colliderdata() const = 0;
		virtual physics_function::Meshcoll_data get_Meshdata() { return physics_function::Meshcoll_data(); };
		physics_function::Physics_data  get_Physicsdata() {
			physics_function::Physics_data data;
			data.inertial_mass = inertial_mass;
			data.drag = drag;
			data.anglar_drag = anglar_drag;
			data.dynamic_friction = dynamic_friction;
			data.static_friction = static_friction;
			data.restitution = restitution;

			data.is_fallable = is_fallable;
			data.is_kinematic = is_kinematic;
			data.is_moveable = is_moveable;
			data.is_hitable = is_hitable;
			return data;
		};

		//‚±‚Ìcollider‚ğmanager‚©‚çíœ
		void remove_collider();

	public:
		virtual  void awake() = 0;

		void finalize() override;

	};
}